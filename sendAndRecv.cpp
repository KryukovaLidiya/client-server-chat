#include "service.h"

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
