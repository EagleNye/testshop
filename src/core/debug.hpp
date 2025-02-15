#pragma once

#ifndef DEBUG_HPP_NEROSHOP
#define DEBUG_HPP_NEROSHOP

#define NEROSHOP_TAG_OUT neroshop::io_write("");
#define NEROSHOP_TAG_IN std::string("\033[1;35;49m[neroshop]: \033[0m") +
#define NEROSHOP_TAG NEROSHOP_TAG_IN

#define NEROSHOP_LOG_PATH ""
#define NEROSHOP_LOG_FILE "log.txt"

#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <iomanip> // std::put_time

namespace neroshop {
    enum log_priority {
        trace, error, warn, info
    };
    static void logger(log_priority priority, const std::string& message) {
        std::ofstream file(std::string(NEROSHOP_LOG_FILE).c_str(), std::ios_base::app);
	    
	    auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now); // current time
	    std::stringstream ss;
	    ss << std::put_time(std::localtime(&in_time_t), std::string("[%Y-%m-%d %H:%M:%S %p]").c_str());
        
        switch (priority) {
            case trace: file << ss.str() << "[Trace]: "; break;
            //case debug: file << ss.str() << "[Debug:] "; break;
            case info: file << ss.str() << "[Info:] "; break;
            case warn: file << ss.str() << "[Warn]: "; break;
            case error: file << ss.str() << "[Error]: "; break;
            //case critical: file << ss.str() << "[Critical]: "; break;
        }
        file << message << "\n";
        file.close();
    }
    inline void print(const std::string& text, int code = 0, bool log_msg = true) { // 0=normal, 1=error, 2=warning, 3=success, 
        log_priority verbosity;
        if(code == 0) { std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;37;49m" << text << "\033[0m\n"; verbosity = log_priority::trace; }
        if(code == 1) { std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;91;49m" << text << "\033[0m\n"; verbosity = log_priority::error; }
        if(code == 2) { std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;33;49m" << text << "\033[0m\n"; verbosity = log_priority::warn; }
        if(code == 3) { std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;32;49m" << text << "\033[0m\n"; verbosity = log_priority::info; }
        if(code == 4) { std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;34;49m" << text << "\033[0m\n"; verbosity = log_priority::info; }
        if(log_msg) logger(verbosity, text);
    }    
    inline void io_write(const std::string& text) {// like print but without a newline
        std::cout << "\033[1;35;49m" << "[neroshop]: " << "\033[1;37;49m" << text << "\033[0m";
    }
}
#endif
