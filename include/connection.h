#ifndef CONNECTION_H
#define CONNECTION_H

#include <string>
#include <iostream>
#include <unistd.h>
#include <cerrno>
#include <cstring>
#include <sys/socket.h> // For socket functions
#include <netinet/in.h> // For sockaddr_in
#include <arpa/inet.h>  // For inet_pton
#include <algorithm>  // For inet_pton
#include "log.h"


class TcpConnection{
    public:
        TcpConnection(const std::string _host, int _port) : host {_host}, port {_port} {}

        bool start();
        bool send_msg(char *msg, int msg_size);
        bool receive_msg(char *buffer_to_write,int *received_size, int expected_size = 0);

        ~TcpConnection()
        {
            if(close(client_fd) == -1)
                L_(lerror) << " While closing Connection fd: "<< std::strerror(errno) ;
            else
                L_(ldebug) << "Connection closed successfully " ;
        }

    private:
        std::string host;
        int port, client_fd;
};




#endif