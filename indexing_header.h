#ifndef INC_1BRC_18_09__INDEXING_HEADER_H
#define INC_1BRC_18_09__INDEXING_HEADER_H

#include <iostream>
#include <map>
//#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <vector>
//#include <boost/locale.hpp>
#include <thread>
//#include <archive.h>
//#include <archive_entry.h>
#include <set>
//#include <float.h>
#include <tbb/concurrent_unordered_map.h>
#include <tbb/concurrent_vector.h>
#include <tbb/mutex.h>
#include <tbb/concurrent_queue.h>

tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> read(std::map<std::string, std::string> &config);
void create_dict(tbb::concurrent_bounded_queue<std::string> &files_q, tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> &dict);
std::vector<std::pair<std::string, tbb::concurrent_vector<double>>> sort_map(const tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>>& wordsCount, const int& sort_type);
void write_into_file(tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> &wordsCount, const std::string& file_by_key);

#endif //INC_1BRC_18_09__INDEXING_HEADER_H
