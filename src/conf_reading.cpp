#include "conf_reading.h"
#include <fstream>
#include <unordered_map>
#include <sstream>

conf_t read_conf(const char *conf_file_name)
{
    std::ifstream conf_file(conf_file_name);
    if (!conf_file)
        throw std::runtime_error("Couldn't open conf file for reading");

    std::unordered_map <std::string, std::string> u;
    std::string line;
    while (std::getline(conf_file, line)) {
        std::istringstream sline(line);
        std::string key;
        if (std::getline(sline, key, '=')) {
            std::string val;
            if (std::getline(sline, val))
                u[key] = val;
        }
    }

    conf_t conf;

    conf.indir = u.at("indir");

    conf.out_by_a = u.at("out_by_a");
    conf.out_by_n = u.at("out_by_n");

    conf.indexing_threads = std::stoi(u.at("indexing_threads"));
    if (conf.indexing_threads <= 0)
        throw std::runtime_error("indexing_threads must be positive");

    conf.max_file_size = std::stoull(u.at("max_file_size"));

    return conf;
}
