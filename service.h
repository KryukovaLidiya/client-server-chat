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

class ThreadsMessanger;
void threadSend(int msggit);
void threadGive(int msggit, int index);
bool findOptions(char** begin, char** end, const std::string& option);
pid_t readFromFile(std::ifstream &file);
void killServer();
void writePidIntoFile(pid_t pid);
bool cheackOptions(int &argc, char **argv);
bool isDigit(char *port);


#endif
