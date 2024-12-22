#ifndef EXCHANGE_MESSAGES_H
#define EXCHANGE_MESSAGES_H

#include <cstdint>
#include <byteswap.h>
#include "nlohmann/json.hpp"

using json = nlohmann::ordered_json;

#pragma pack(push,1)
struct ResponsePacket
{
    char ticker_symbol[4];
    char buy_sell_indicator;
    int32_t quantity;
    int32_t price;
    int32_t sequence_number;
    void convert_to_system_endianess()
    {
        // for system independence
        #if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__

        quantity = bswap_32(quantity);
        price = bswap_32(price);
        sequence_number = bswap_32(sequence_number);

        #endif
    }

    json toJson() const 
    {
        std::string symbol (ticker_symbol, 4);
        return json{{"Symbol",symbol}, {"BuySellIndicator",buy_sell_indicator}, 
        {"Quantity",quantity}, {"Price",price}, {"PacketSequence",sequence_number}};
    }
};
#pragma pack(pop)


#pragma pack(push,1)
struct RequestPacket
{
    uint8_t call_type; 
    uint8_t resend_seq;  
    // the range of packets lie between [0,255] or [1,255]
    RequestPacket(uint8_t _call_type=1, uint8_t _resend_seq=0)
    {
        call_type=_call_type;
        resend_seq=_resend_seq;
    }
};
#pragma pack(pop)

#endif