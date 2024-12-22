#include "connection.h"

bool TcpConnection::start()
{
    // this function creates a tcp connection with
    // the exchange server for reliable communication

    // create a socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd < 0) 
    {
        L_(lerror) << "Socket Creation failed: "<< std::strerror(errno) ;
        return false;
    }

    // Server address structure
    sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);

    // ipv4 address to binary conversion
    if (inet_pton(AF_INET, host.c_str() , &server_addr.sin_addr) <= 0) 
    {
        L_(lerror) <<"Invalid address or address not supported: "<<host<<":"<<port<<" :"<< std::strerror(errno) ;
        return false;
    }

    // connect to server
    if (connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        L_(lerror) << "Connection to server "<< host<< ":" <<port << " failed. " << "Reason: " << std::strerror(errno) ;
        return false;
    }
    L_(ldebug) << "Connected to the server : " <<host<<":"<<port<<" sucessfully." ;

    return true;
}

bool TcpConnection::send_msg(char *msg, int msg_size)
{
    int bytes_to_send = msg_size;

    // keep sending till the whole packet is not sent
    while(bytes_to_send != 0)
    {
        int bytes_sent = send(client_fd, msg, bytes_to_send, 0);
        if(bytes_sent == -1)
        {
            L_(lerror) << "Send_msg failed : "<< std::strerror(errno) ;
            return false;
        }
        bytes_to_send -= bytes_sent;
        msg += bytes_sent;
    }
    L_(ldebug) << "send_msg succeeded" ;
    return true;
}

bool TcpConnection::receive_msg(char *buffer_to_write,int *received_size, int expected_size )
{
    if(expected_size==0)
    {
        // Case when message should be read till the server keeps sending.
        // It expects a buffer that is larger than the message sent by server.
        const int SIZE_TO_RECEIVE = 1024; // default size to receive 1KB used
        int total_bytes_received=0;
        int bytes_received=0;
        while((bytes_received = recv(client_fd, buffer_to_write, SIZE_TO_RECEIVE, 0)) > 0)
        {
            buffer_to_write += bytes_received;
            total_bytes_received += bytes_received;
        }

        if(bytes_received == -1 )
        {
            L_(lerror) <<"Server closed unexpectedly: "<< host <<":"<< port<< " " << std::strerror(errno) ;
            return false;
        }

        if(received_size != nullptr)
            *received_size = total_bytes_received; 
        L_(ldebug) << "receive_msg succeeded" ;
        return true;

    }
    // Since the above function returns internally no explicit else required
    // Case when user expects message of particular size 
    // assumes that buffer provided by user can hold the expected_size

    int bytes_to_recieve = expected_size;

    // keep receiving till the expected size is not received
    while(bytes_to_recieve != 0)
    {
        int bytes_received =  recv(client_fd, buffer_to_write, bytes_to_recieve, 0);
        if(bytes_received == -1)
        {
            L_(lerror) << "receive_msg failed : "<< std::strerror(errno) ;
            return false;
        }
        buffer_to_write += bytes_received;
        bytes_to_recieve -= bytes_received;
    }
    L_(ldebug) << "receive_msg succeeded" ;
    return true;

}