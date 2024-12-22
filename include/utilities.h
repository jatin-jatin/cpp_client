#ifndef UTILITIES_H
#define UTILITIES_H

#include<string>

class Utilities{
    public:
        static std::string get_date()
        {
            time_t t = time(0);
            tm *now = localtime(&t);
            std::string date = std::to_string(now->tm_mday) +"_" + std::to_string(now->tm_mon +1) + "_" + std::to_string(now->tm_year +1900);
            return date;
        }

        static std::string trim(const std::string& str) 
        {
            size_t start = str.find_first_not_of(" \t");
            size_t end = str.find_last_not_of(" \t");
            return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
        }
        
        static std::unordered_map<std::string, std::string> read_config(const std::string& filename) 
        {
            std::unordered_map<std::string, std::string> config;
            std::ifstream file(filename);
            std::string line;

            if (!file.is_open()) {
                std::cerr << "Error: Unable to open config file: " << filename << std::endl;
                return config;
            }

            while (std::getline(file, line)) {
                // Ignore comments or blank lines
                line = trim(line);
                if (line.empty() || line[0] == '#') continue;

                // Split line into key and value
                size_t delimiterPos = line.find('=');
                if (delimiterPos != std::string::npos) {
                    std::string key = trim(line.substr(0, delimiterPos));
                    std::string value = trim(line.substr(delimiterPos + 1));
                    config[key] = value;
                }
            }

            file.close();
            return config;
        }
};

#endif