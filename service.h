#ifndef SERVICE_H
#define SERVICE_H

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

using boost::asio::ip::tcp;
extern std::vector<tcp::socket> fd;

class Threads_messanger;
void kill_server();
void thread_send(int msggit);
void thread_give(int msggit, int index);
void write_pid_into_file(pid_t pid);
bool cheack_options(int &argc, char **argv);
bool is_digit(char *port);
bool find_options(char** begin, char** end, const std::string& option);
pid_t read_from_file(std::ifstream &file);

#endif
