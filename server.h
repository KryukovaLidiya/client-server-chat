#ifndef SERVER_H
#define SERVER_H

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

class Threads_messanger;
void thread_send(int msggit);
void thread_give(int msggit, int index);
bool find_options(char** begin, char** end, const std::string& option);
pid_t read_from_file(std::ifstream &file);
void kill_server();
void write_pid_into_file(pid_t pid);
bool cheack_options(int &argc, char **argv);
bool is_digit(char *port);

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
    static Server *get_instance(int port, key_t key = 10) {
        if(!p_instance)
            p_instance = new Server(key, port);
        return p_instance;
    }

    void request() {
        std::thread thr_s(threadSend, msggit);
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
	    if(socket.is_open()) {
               socket.close();
            }
        }
        msgctl(msggit, IPC_RMID, 0);//Удаление очереди
        delete p_instance;
    }
};

Server* Server::p_instance = 0;

#endif
