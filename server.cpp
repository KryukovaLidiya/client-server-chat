#include<assert.h>
#include<sys/msg.h>
#include<vector>
#include<iterator>
#include<iostream>
#include<utility>
#include<boost/asio.hpp>
#include<thread>
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

void thread_send(int msggit) {
    while(1) {
	ThreadsMessanger sbuf;
	if((msgrcv(msggit, &sbuf, sizeof(sbuf), 1, 0)) >= 0) {
	    for(auto &socket: fd) {
		boost::asio::write(socket, boost::asio::buffer(sbuf.buf, 512));
	    }
	}
    }
}

void thread_give(int msggit, int index)
{
    char buf[512];
    memset(buf, 0, 512); 
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
	ThreadsMessanger sbuf = ThreadsMessanger(buf);
    	msgsnd(msggit, &sbuf, sizeof(sbuf), IPC_NOWAIT);
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
    }
public:
    static Server *getInstance(key_t key = 10) {
        if(!p_instance)           
            p_instance = new Server(key);
        return p_instance;
    }

    void request() {
	std::thread thr_s(thread_send, msggit);
	int i = 0;
    	while (1) 
	{
            tcp::socket newsock(io_service);
	    acc.accept(newsock);
	    std::thread(thread_give, msggit, i).detach();
	    fd.emplace_back(std::move(newsock));
	    i++;
	}
	thr_s.join();
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

int main(int argc, char **argv) 
{
    bool isDaemon = false;
    if(findOptions(argv, argv+argc, "-d")) {
	isDaemon = true;
    }
    int pid = fork();
    if(pid < 0) exit(0);
    if(pid == 0 && isDaemon) {
	daemon(0, 0);
	runServer();
    }
    else if(pid > 0 && !isDaemon) {
    	runServer();
    }
    return 0;
}
