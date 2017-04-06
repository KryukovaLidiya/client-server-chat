#include<assert.h>
#include<sys/msg.h>
#include<vector>
#include<iterator>
#include<iostream>
#include<utility>
#include<fstream>
#include<unistd.h>
#include<boost/asio.hpp>
#include<thread>
#include<string>
#include <signal.h>
#define port 1234

using boost::asio::ip::tcp;
std::vector<tcp::socket> fd;

class ThreadsMessanger {
public:
    long type;
    char buf[512];
    ThreadsMessanger(char *buf = nullptr, long type = 1) : type(type) {
	if(buf != nullptr) {
	    strcpy(this->buf, buf);
	}
    }
};

void threadSend(int msggit) {
    while(1) {
	ThreadsMessanger sbuf;
	if((msgrcv(msggit, &sbuf, sizeof(sbuf), 1, 0)) >= 0) {
	    for(auto &socket: fd) {
		boost::asio::write(socket, boost::asio::buffer(sbuf.buf, 512));
	    }
	}
    }
}

void threadGive(int msggit, int index)
{
    char buf[512];
    bool isExit = false;
    memset(buf, 0, 512);
    
    strcpy(buf, "count of client ");
    int count = fd.size();
    sprintf(buf, "%s%d", buf, count);
    ThreadsMessanger sbuf = ThreadsMessanger(buf);
    msgsnd(msggit, &sbuf, sizeof(sbuf), IPC_NOWAIT);

    while(1) {
	try {
            boost::system::error_code error;
	    fd[index].read_some(boost::asio::buffer(buf), error);
            if (error == boost::asio::error::eof) {
                break; // Connection closed cleanly by peer.
	    }
      	    else if (error) {
                continue; // Some other error.
	    }
	}
	catch (std::exception& e) {
    	    std::cerr << "Exception in thread: " << e.what() << "\n";
  	}

	if(strcmp(buf, "-stop") == 0) {
	    strcpy(buf, "one of client go out: bye");
	}
	else if(strcmp(buf, "-count") == 0) {
	    strcpy(buf, "count of client ");
	    int count = fd.size();
	    sprintf(buf, "%s%d", buf, count);
	}
	ThreadsMessanger sbuf = ThreadsMessanger(buf);
    	msgsnd(msggit, &sbuf, sizeof(sbuf), IPC_NOWAIT);

	if(isExit) {
	   std::swap(fd[index], fd[fd.size() - 1]);
	   fd.erase(fd.begin() + fd.size() - 1);
	   break;
	}

    }
}

class Server {
    int msggit;
    key_t key;
    boost::asio::io_service io_service;
    tcp::acceptor acc;

    static Server *p_instance;
    Server(key_t key) : acc(io_service, tcp::endpoint(tcp::v4(),port)) {
    	assert((msggit = msgget(key, IPC_CREAT | 0666)) >= 0);
	acc.listen(10);
    }
public:
    static Server *getInstance(key_t key = 10) {
        if(!p_instance)           
            p_instance = new Server(key);
        return p_instance;
    }

    void request() {
	std::thread thrS(threadSend, msggit);
	int i = 0;
    	while (1) 
	{
            tcp::socket newsock(io_service);
	    acc.accept(newsock);
	    std::thread(threadGive, msggit, i).detach();
	    fd.emplace_back(std::move(newsock));
	    i++;
	}
	thrS.join();
    }
    ~Server() {
	for(auto &socket : fd) {
	   socket.close();
	}
	msgctl(msggit, IPC_RMID, 0);//Удаление очереди
    	delete p_instance;
    }
};

Server* Server::p_instance = 0;

bool findOptions(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
}

void runServer() {
    const std::unique_ptr<Server> serv(Server::getInstance());
    serv->request();
}


pid_t readFromFile(std::ifstream &file) {
    pid_t pid;
    file >> pid;
    return pid;
}

void killServer() {
    std::ifstream file("chatServer.pid");
    pid_t pid = readFromFile(file);
    file.close();
    remove("chatServer.pid");
    kill(pid, SIGUSR1);
}


void writePidIntoFile(pid_t pid) {    
    std::ofstream file("chatServer.pid");
    file << pid;
    file.close();
}

bool cheackOptions(int &argc, char **argv) {
    bool isDaemon = false;
    if(findOptions(argv, argv+argc, "-d")) { //daemon
	isDaemon = true;
    }
    else if(findOptions(argv, argv+argc, "-s")) { //stop
	//stop server
	killServer();
	exit(0);
    }
    else if(argc > 1) {
	std::cout << "You can use:" << std::endl;	
	std::cout << "\t-s - stop deamon process" << std::endl;
	std::cout << "\t-d - start deamon process" << std::endl;
    	exit(0);
    }
    return isDaemon;
}

int main(int argc, char **argv) 
{
    bool isDaemon = cheackOptions(argc, argv);

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

	runServer();
    }
    else if(pid > 0 && !isDaemon) {
    	runServer();
    }
    remove("chatServer.pid");
    return 0;
}
