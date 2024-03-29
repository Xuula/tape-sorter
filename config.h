#include <iostream>
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <fstream>
#include <exception>
#include <string>
#include <sstream>

class Config {
public:
    static int MOVE_DELAY_MS;
    static int REWIND_DELAY_MS;
    static int READ_DELAY_MS;
    static int WRITE_DELAY_MS;
    static int MAX_RAM_BYTES;

    static void load_config(const std::string& file_path);
        
};

#endif