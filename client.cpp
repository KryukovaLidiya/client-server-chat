/*
client.cpp - client for chat
*/
#include<assert.h>
#include<thread>
#include<iostream>
#include<string.h>
#include <utility>
#include<boost/asio.hpp>

using boost::asio::ip::tcp;
bool isStop = false;

void stop_client(tcp::socket *sock) {
    while(!isStop);
    boost::asio::write(*sock, boost::asio::buffer("-stop", 512));
}

/*
thread for send your message
*/
void send_pth(tcp::socket *sock) {
	char buf[512]; 
   	while(strcmp(buf, "-stop") != 0) {		
    	    std::cout << "Please write less then 512 simvols" << std::endl;
    	    memset(buf, 0, 512);
    	    std::cin.getline(buf, 512);
    	    boost::asio::write(*sock, boost::asio::buffer(buf, 512));
	}
	isStop = true;
}

int main (int argc, char **argv) {
    boost::asio::io_service io_service;
    char buf[512];

    if(argc < 3) {
	std::cout << "./client 127.0.0.1 port" << std::endl;
	return 0;
    }
    
    tcp::socket sock(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(sock, resolver.resolve({argv[1], argv[2]}));
			    
    auto handl = 
      [] (int i) { 
		   isStop = true;
   		   sleep(2);
    		   exit(EXIT_SUCCESS);
		};

    signal (SIGINT, handl); 
    std::cout << "You into chat, you can use options: " << std::endl;
    std::cout << "\t-stop - this is option for go out " << std::endl;
    std::cout << "\t-count - this is option for count of users now " << std::endl; 

    std::thread thr2(stop_client, &sock);
    std::thread thr(send_pth, &sock);

    /*
     * Thread for read message
     */
    while(1) {
    	boost::asio::read(sock, boost::asio::buffer(buf, 512));
	if(isStop) {
	   break;
	}
	std::cout << "receive: " <<  buf << std::endl;
    }
    thr.join();
    return 0;
}
