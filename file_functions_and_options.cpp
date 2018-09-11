#include "service.h"

/*
find options into two-dimensional array
*/
bool find_options(char** begin, char** end, const std::string& option) {
    return std::find(begin, end, option) != end;
}

/* read pid into file */
pid_t read_from_file(std::ifstream &file) {
    pid_t pid;
    file >> pid;
    return pid;
}

/*kill server process */
void kill_server() {
    std::ifstream file("chatServer.pid");
    pid_t pid = readFromFile(file);
    file.close();
    remove("chatServer.pid");
    kill(pid, SIGUSR1);
}

/* write pid into file*/
void write_pid_into_file(pid_t pid) {
    std::ofstream file("chatServer.pid");
    file << pid;
    file.close();
}

/* check options */
bool cheack_options(int &argc, char **argv) {
    bool isDaemon = false;
    if(findOptions(argv + 2, argv+argc, "-d")) { //daemon
        isDaemon = true;
    }
    else if(findOptions(argv + 2, argv+argc, "-s")) { //stop daemon
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

/* check port */
bool is_digit(char *port) {
    bool isd = true;
    while(*port) {
        if(!isdigit(*port++)) {
            isd = false;
            break;
        }
    }
    return isd;
}

