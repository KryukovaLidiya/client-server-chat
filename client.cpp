#include<stdio.h>
#include<assert.h>
#include<thread>
#include<iostream>
#include<string.h>
#include <utility>
#include<boost/asio.hpp>
#define port 1234

using boost::asio::ip::tcp;

void send_pth(tcp::socket *sock) {
	char buf[512]; 
   	while(strcmp(buf, "bye") != 0) {		
    	    printf("Please write less then 512 simvols\n");
    	    memset(buf, 0, 512);
    	    std::cin.getline(buf, 512);
    	    boost::asio::write(*sock, boost::asio::buffer(buf, 512));
	}
}

int main () 
{
    boost::asio::io_service io_service;
    char buf[512];
    
    tcp::socket sock(io_service);
    tcp::resolver resolver(io_service);
    boost::asio::connect(sock, resolver.resolve({"127.0.0.1", "1234"}));
    std::thread thr(send_pth, &sock);
    while(1) {
    	boost::asio::read(sock, boost::asio::buffer(buf, 512));
	printf("recv: %s\n", buf);
    }
    thr.join();
    return 0;
}
