#include "config.h"
#include <exception>

void Config::load_config(const std::string& file_path){
    std::ifstream config_file(file_path);
    if (!config_file.is_open()) {
        std::stringstream err;
        err << "Config file '" << file_path << "' not found." << std::endl;
        throw std::runtime_error(err.str());
    }

    std::string line;
    while (std::getline(config_file, line)) {
        size_t delimiter_pos = line.find('=');
        if (delimiter_pos != std::string::npos) {
            std::string key = line.substr(0, delimiter_pos);
            std::string value = line.substr(delimiter_pos + 1);

            if (key == "MOVE_DELAY_MS") {
                MOVE_DELAY_MS = std::stoi(value);
            } else if (key == "REWIND_DELAY_MS") {
                REWIND_DELAY_MS = std::stoi(value);
            } else if (key == "MAX_RAM_BYTES"){
                MAX_RAM_BYTES = std::stoi(value);
            } else if (key == "READ_DELAY_MS"){
                READ_DELAY_MS = std::stoi(value);
            } else if (key == "WRITE_DELAY_MS"){
                WRITE_DELAY_MS = std::stoi(value);
            } else{
                throw std::runtime_error("Error loading config: unknown key: " + key);
            }
        }
    }

    config_file.close();
}