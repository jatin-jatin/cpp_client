// develop a cpp client req -> resp using tcp
// Using req and resp it creates a json file that should be written in a sequence

#include "client.h"
#include "connection.h"
#include "exchange_messages.h"
#include "log.h"
#include "utilities.h"
#include <iostream>
#include <unordered_map>

void set_up_logging()
{
    std::string log_file_path = "logs/log_" + Utilities::get_date() + ".txt";
    initLogger(log_file_path.c_str(), ldebug);
}

int main() {
    set_up_logging();
    
    // loading config
    std::string config_path = "configs/config.txt";
    std::unordered_map<std::string, std::string> config = Utilities::read_config(config_path);
    for(auto &parameter: config){
        L_(linfo) << "Loaded config values " << parameter.first<< "=" << parameter.second;
    }

    // setting parameters
    std::string host = config["HOST"];
    int port = std::stoi(config["PORT"]);
    std::string output_file = config["OUTPUT_FILE"];

    Client client(host, port, output_file);
    client.build_order_book();
    return 0;
}




