#include"server.h"

/*function for start server configuration */
void runServer(int port) {
    const std::unique_ptr<Server> serv(Server::getInstance(port));
    serv->request();
}

/*  
 *  first: cheack daemon process or not 
 *  second: start server 
 *  the end: delete service file
 */
int main(int argc, char **argv) 
{
    bool isDaemon = false;
    if(argc < 2 || !isDigit(argv[1])) { //daemon
	std::cout << "./server port ...." << std::endl;
	std::cout << ".... - this is options or without" << std::endl;
    	return 0;
    } 
    else if (argc > 2) {
    	isDaemon = cheackOptions(argc, argv);    
    }

    int pid = fork();

    if(pid < 0) exit(0);
    if(pid == 0 && isDaemon) {
	umask(0);
	
	pid_t p = setsid();
	writePidIntoFile(p);
	
	chdir("/");
	close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);

	runServer(atoi(argv[1]));
    }
    else if(pid > 0 && !isDaemon) {
    	runServer(atoi(argv[1]));
    }
    remove("chatServer.pid");
    return 0;
}
