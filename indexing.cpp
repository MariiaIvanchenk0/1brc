#include "indexing_header.h"
#include "reading_config.h"

tbb::mutex mx;

void create_dict(tbb::concurrent_bounded_queue<std::string> &files_q, tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> &result_map) {
    while(true) {
        std::string city_info;
        files_q.pop(city_info);
        if (city_info.empty()) { // poison pill handle
            files_q.push(std::move(city_info));
            break;
        }

        std::istringstream stream(city_info);
        std::string current_line;

        while(getline(stream, current_line)) {
            // parse
            std::istringstream line_stream(current_line);
            std::string city_name;
            getline(line_stream, city_name, ';');

            if (!city_name.empty()){
                double city_temp;
                line_stream >> city_temp;

                auto it = result_map.find(city_name);
                if (it != result_map.end()) {
                    tbb::concurrent_vector<double> &current_vect = it->second;
                    tbb::mutex::scoped_lock lock(mx);  // lock when modifying
                    current_vect[0] = std::min(current_vect[0], city_temp);
                    current_vect[2] = std::max(current_vect[2], city_temp);
                    current_vect[1] += city_temp;
                    current_vect[3] += 1.0;
                } else {
                    tbb::concurrent_vector<double> new_vect = {city_temp, city_temp, city_temp, 1.0};
                    result_map.emplace(city_name, new_vect);
                }
            }
        }
    }
}

tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> read(std::map<std::string, std::string> &config) {
    std::string imgPath = config["indir"];
    if (!std::filesystem::exists(imgPath)) {
        std::cerr << "Such directory is not accessible or do not exist." << std::endl;
        exit(26);
    }

    size_t maxSize = std::stoi(config["filenames_queue_size"]);
    size_t numThreads = std::stoi(config["indexing_threads"]);

    tbb::concurrent_bounded_queue<std::string> files_q;
    files_q.set_capacity(maxSize);

    tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> first_map;

    std::ifstream raw_file(imgPath, std::ios::binary);
    if (!raw_file.is_open()) {
        handle_file_error(imgPath, 3);
    }

    std::vector<std::thread> threads;
    threads.reserve(numThreads);
    // create dict
    for (size_t j = 0; j < numThreads; j++) {
        threads.emplace_back(create_dict, std::ref(files_q), std::ref(first_map));
    }

    size_t block_size = 20;
    size_t line_count = 0;
    std::string line;
    std::stringstream buffer;

    while (getline(raw_file, line)) {
        buffer << line << '\n';
        if (++line_count == block_size) {
            files_q.push(std::move(buffer.str()));
            buffer.str("");
            line_count = 0;
        }
    }

    if (line_count > 0) {
        files_q.push(std::move(buffer.str()));
    }

    files_q.push(std::move(""));

    for (size_t j = 0; j < numThreads; j++) {
        threads[j].join();
    }

    raw_file.close();

    return first_map;
}

std::vector<std::pair<std::string, tbb::concurrent_vector<double>>> sort_map(const tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>>& wordsCount, const int& sort_type) {
    std::vector<std::pair<std::string, tbb::concurrent_vector<double>>> key_value_pair;
    key_value_pair.reserve(wordsCount.size());

    for (const auto& it : wordsCount) {
        key_value_pair.emplace_back(it);
    }
    std::sort(key_value_pair.begin(), key_value_pair.end(), [sort_type](const auto& a, const auto& b) {return a.first < b.first;});
    return key_value_pair;
}

void write_into_file(tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> &wordsCount, const std::string& file_by_key) {
    std::vector<std::pair<std::string, tbb::concurrent_vector<double>>> sorted_map_value = sort_map(wordsCount, 1);
    std::ofstream by_key("../" + file_by_key);

    if (!by_key.is_open()) {
        handle_file_error(file_by_key, 3);
    }
    else
        by_key << "";
    for (const auto& entry : sorted_map_value) {
        const std::string& key = entry.first;
        const tbb::concurrent_vector<double>& vec = entry.second;

        try {
            by_key << key << "=" << std::fixed << std::setprecision(1)<< vec[0] << "/"<< std::fixed << std::setprecision(1) << vec[1] << "/"<< std::fixed << std::setprecision(1)<< vec[2] << "\n";
        } catch (std::ios_base::failure &) {
            handle_file_error(file_by_key, 6);
        }
    }
    by_key << "";
    by_key.close();
}
