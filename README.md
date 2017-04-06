# client-server-chat
libs: boost, boost asio, std::thread
CC: g++ std=c++11
system: linux

#make
make all

#server
./server port -d (This is deamon process of server)
./server port -s (kill this deamon process)
./server port otherOptions (View all right options)
./server port (This is server without options)
others variant is not right

#client
./client 127.0.0.1 port
other variant is not right


