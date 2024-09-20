#include "reading_config.h"

void handle_file_error(const std::string &filename, int error_code) {
    switch (error_code) {
        case 3:
            std::cerr << "Error: " << "Failed to open input file <" << filename << ">" << std::endl;
            break;
        case 4:
            std::cerr << "Error: " << "Failed to open the configuration file <" << filename << ">" << std::endl;
            break;
        case 5:
            std::cerr << "Error: " << "Failed to read an input file <" << filename << ">" << std::endl;
            break;
        case 6:
            std::cerr << "Error: " << "Failed to write into the file <" << filename << ">" << std::endl;
            break;
        default:
            std::cerr << "Error: " << "Unknown error occurred" << std::endl;
    }
    exit(error_code);
}

std::vector <std::string> read_file(const std::string &filename) {
    std::ifstream filestream(filename);
    std::string line;
    std::vector <std::string> lines;
    if (!filestream) {
        handle_file_error(filename, 4);
    }

    while (std::getline(filestream, line)) {
        if (line.find('=') != std::string::npos) {
            lines.emplace_back(line);
        }
    }

    if (filestream.fail() && !filestream.eof()) {
        handle_file_error(filename, 5);
    }
    return lines;
}

std::map<std::string, std::string> read_config(const std::string &filename) {
    std::vector <std::string> config_content = read_file(filename);
    std::map<std::string, std::string> config_map;
    std::istringstream iss;

    for (size_t i = 0; i < config_content.size(); i++) {
        std::string line = config_content[i];
        std::replace(line.begin(), line.end(), '=', ' ');
        std::replace(line.begin(), line.end(), '"', ' ');

        iss.clear();
        iss.str(line);
        std::string key, value;

        if (iss >> key >> value) {
//            std::cout << key << " " << value << std::endl;
            config_map[key] = value;
        } else {
            std::cerr << "Error parsing line " << i << " : " << line << std::endl;
            exit(5);
        }
    }

    return config_map;
}