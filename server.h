#pragma ones

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
#include<signal.h>
#include "service.h"

using boost::asio::ip::tcp;
std::vector<tcp::socket> fd;

class ThreadsMessanger;
void threadSend(int msggit);
void threadGive(int msggit, int index);
bool findOptions(char** begin, char** end, const std::string& option);
pid_t readFromFile(std::ifstream &file);
void killServer();
void writePidIntoFile(pid_t pid);
bool cheackOptions(int &argc, char **argv);
bool isDigit(char *port);

class Server {
    int msggit;
    key_t key;
    boost::asio::io_service io_service;
    tcp::acceptor acc;

    static Server *p_instance;
    Server(key_t key, int port) : acc(io_service, tcp::endpoint(tcp::v4(), port)) {
        assert((msggit = msgget(key, IPC_CREAT | 0666)) >= 0);
        acc.listen(10);
    }
public:
    static Server *getInstance(int port, key_t key = 10) {
        if(!p_instance)
            p_instance = new Server(key, port);
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
