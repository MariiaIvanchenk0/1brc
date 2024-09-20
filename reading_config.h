#ifndef INC_1BRC_18_09__READING_CONFIG_H
#define INC_1BRC_18_09__READING_CONFIG_H

#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <chrono>
#include <atomic>
#include <vector>

template<class D>
inline long long to_ms(const D& d) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(d).count();
}

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

std::vector <std::string> read_file(const std::string &filename);
std::map<std::string, std::string> read_config(const std::string &filename);
void handle_file_error(const std::string &filename, int error_code);


#endif //INC_1BRC_18_09__READING_CONFIG_H
