#ifndef CLIENT_H
#define CLIENT_H

#include <vector>
#include <cstdint>
#include <string>
#include <cmath>
#include <fstream>
#include "exchange_messages.h"
#include "log.h"

using json = nlohmann::ordered_json;

class Client
{
    private:
        std::string host, output_file;
        int port;
        std::vector<int> remaining_sequence_numbers;
        std::vector<ResponsePacket> remaining_packets;
        std::vector<ResponsePacket> stream_packets;

        void receive_packets_using_stream_api();
        void receive_remaining_packets_using_resend_api();
        void  write_received_orders_as_json();
    
    public:

        void build_order_book();

        Client(const std::string& _host, int _port, const std::string& _output_file)
        : host{_host},output_file{_output_file}, port{_port}  {}
};


#endif 