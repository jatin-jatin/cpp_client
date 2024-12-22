#include "client.h"
#include "connection.h"
#include "nlohmann/json.hpp"

void Client::build_order_book()
{
    stream_packets.clear();
    remaining_packets.clear();
    remaining_sequence_numbers.clear();

    receive_packets_using_stream_api();
    receive_remaining_packets_using_resend_api();
    write_received_orders_as_json();
}

void  Client::write_received_orders_as_json()
{
    json orders_json = json::array();
    std::vector<ResponsePacket>::size_type stream_idx = 0, resend_idx =0 ;

    // merge process to ensure response objects are ordered in increasing sequence
    while(stream_idx != stream_packets.size() && resend_idx != remaining_packets.size())
    {
        if ( stream_packets[stream_idx].sequence_number < remaining_packets[resend_idx].sequence_number )
        {
            orders_json.push_back( stream_packets[stream_idx++].toJson() );
        }
        else
        {
            orders_json.push_back( remaining_packets[resend_idx++].toJson() );
        }
    }

    // In case there are packets remaining in any of the two vectors
    while(stream_idx != stream_packets.size())
    {
        orders_json.push_back( stream_packets[stream_idx++].toJson() );
    }

    while(resend_idx != remaining_packets.size())
    {
        orders_json.push_back( remaining_packets[resend_idx++].toJson() );
    }

    // writing all the response objects as json
    std::ofstream out_file(output_file);
    if(out_file.is_open())
    {
        out_file << orders_json.dump(2); // json with 2 spaces
        out_file.close();
        L_(linfo) <<"Order book written successfully as json file: "<< output_file ;
    }
    else
    {
        L_(lerror) << "Error opening file" << output_file << " for writing" ;
    }
}

void Client::receive_remaining_packets_using_resend_api()
{
    static const int RESEND_CODE = 2;
    TcpConnection connection(host, port);
    L_(linfo) << "Creating connection for resend API to receive missing orders " ;
    if( !connection.start() ) exit(EXIT_FAILURE);
    for( auto &seq_no: remaining_sequence_numbers)
    {
        RequestPacket resend_request(RESEND_CODE,seq_no);
        L_(linfo) << "Sending resend request for seq:"<<seq_no ;
        connection.send_msg( (char *)&resend_request, sizeof(RequestPacket));

        ResponsePacket resend_response;
        L_(linfo) << "Receiving resend response for seq:" ;
        connection.receive_msg( (char*)&resend_response, nullptr, sizeof(ResponsePacket));
        resend_response.convert_to_system_endianess();
        if( resend_response.sequence_number != seq_no )
        {
            L_(lerror) <<"The requested resend sequence number:"<< seq_no 
            << " does not match with received one: " << resend_response.sequence_number ;
            exit(EXIT_FAILURE);
        }
        L_(linfo) << "Received response for seq:"<<seq_no<<" correctly" ;
        remaining_packets.push_back(resend_response);
    }
}

void Client::receive_packets_using_stream_api()
{
    static const int MARGIN_OF_SAFETY = 2;
    static const int MAX_POSSIBLE_STREAM_PACKETS = std::pow(2,sizeof(decltype(RequestPacket::call_type))*8 + MARGIN_OF_SAFETY); 
    static const int SIZE_OF_SINGLE_RESPONSE_PACKET = sizeof(ResponsePacket);
    //multiply by 8 in exponent to convert byte to bits
    TcpConnection connection(host, port);
    L_(linfo) << "Creating connection for stream API to receive all orders";
    if( !connection.start() ) exit(EXIT_FAILURE);
    RequestPacket stream_request;
    // send stream_reqeuest

    L_(linfo) << "Sending stream API request" ;
    if( !connection.send_msg((char *) &stream_request, sizeof(stream_packets)) ) exit(EXIT_FAILURE);

    // buffer to hold the response
    char buffer[SIZE_OF_SINGLE_RESPONSE_PACKET * MAX_POSSIBLE_STREAM_PACKETS];
    int size_received=0; 

    // receive stream_request response
    L_(linfo) << "Receiving stream API response" ;
    if( !connection.receive_msg(buffer, &size_received) ) exit(EXIT_FAILURE);
    if(size_received % SIZE_OF_SINGLE_RESPONSE_PACKET !=0 )
    {
        L_(lerror) << "Incomplete Messages Received by exchanage_stream_api" ;
        exit(1);
    }
    int num_packets = size_received / SIZE_OF_SINGLE_RESPONSE_PACKET;

    ResponsePacket *response = (ResponsePacket*)buffer;
    for(int pkt=0, expected_seq_num = 1; pkt < num_packets; pkt++, response++, expected_seq_num++ ){
        response->convert_to_system_endianess();
        while( expected_seq_num != response->sequence_number)
        {
            remaining_sequence_numbers.push_back(expected_seq_num);
            expected_seq_num++;     
        }
        stream_packets.push_back(*response);
    }
    L_(linfo) << "stream API response gathered successfully" ;
}