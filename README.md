#client-server-chat
libs: boost, boost asio, std::thread
CC: g++ -std=c++11

# Make
make all

# Server
./server port [-h]
* Daemonized mode
./server port -d (start daemon)
./server port -s (kill daemon)

# Client
./client 127.0.0.1 port
