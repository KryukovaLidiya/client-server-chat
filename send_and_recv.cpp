#include "service.h"

static int count_of_user = 0;

/* Class for messaging */
class Threads_messanger {
public:
    long type;
    char buf[512];
    
    Threads_messanger(char *buf = nullptr, long type = 1) : type(type) {
        if(buf != nullptr) {
            strcpy(this->buf, buf);
        }
    }
};

/* thread for write message to socket */
void thread_send(int msggit) {
    while(1) {
        Threads_messanger sbuf;
        if((msgrcv(msggit, &sbuf, sizeof(sbuf), 1, 0)) >= 0) {
            for(auto &socket: fd) {
		if(socket.is_open()) {
                	boost::asio::write(socket, boost::asio::buffer(sbuf.buf, 512));
		}
            }
        }
    }
}

/*Thread for read message from socket */
void threadGive(int msggit, int index) {
    count_of_user++;
    char buf[512];
    bool isExit = false;
    memset(buf, 0, 512);

    strcpy(buf, "count of client ");
    sprintf(buf, "%s%d", buf, count_of_user);
    Threads_messanger sbuf = Threads_messanger(buf);
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
	    isExit = true;
            strcpy(buf, "one of client go out: bye");
        }
        else if(strcmp(buf, "-count") == 0) {
            strcpy(buf, "count of client ");
            sprintf(buf, "%s%d", buf, count_of_user);
        }
	
        Threads_messanger sbuf = Threads_messanger(buf);
        msgsnd(msggit, &sbuf, sizeof(sbuf), IPC_NOWAIT);

        if(isExit) {
           fd[index].close();
	   count_of_user--;
           break;
        }
    }
}
