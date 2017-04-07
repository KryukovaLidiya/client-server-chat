#include "service.h"

bool findOptions(char** begin, char** end, const std::string& option)
{
    return std::find(begin, end, option) != end;
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
    if(findOptions(argv + 2, argv+argc, "-d")) { //daemon
        isDaemon = true;
    }
    else if(findOptions(argv + 2, argv+argc, "-s")) { //stop
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

bool isDigit(char *port) {
    bool isd = true;
    while(*port) {
        if(!isdigit(*port++)) {
            isd = false;
            break;
        }
    }
    return isd;
}

