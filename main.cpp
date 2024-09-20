#include "reading_config.h"
#include "indexing_header.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Wrong number of arguments " << "\n" << "Usage: " << argv[0] << "<config_file>" << std::endl;
        exit(1);
    }

    std::string config_file;
    try {
        config_file = argv[1];
    }
    catch (std::invalid_argument&) {
        std::cerr << "Unable to identify the name of the configuration file" << std::endl;
        handle_file_error(config_file, 3);
    }
    std::map<std::string, std::string> config_info = read_config(config_file);

    auto total_start_time = get_current_time_fenced();
    tbb::concurrent_unordered_map<std::string, tbb::concurrent_vector<double>> result = read(config_info);
    auto total_end_time = get_current_time_fenced();

    std::cout << "Time without writting and sorting = " << to_ms(total_end_time - total_start_time) << std::endl;

    write_into_file(result, config_info["out_by_n"]);

    return 0;
}
