#ifndef PROCESS_TEXT_CONF_READING_H
#define PROCESS_TEXT_CONF_READING_H

#include <string>

struct conf_t {
    std::string indir;
    std::string out_by_a, out_by_n;
    unsigned char indexing_threads; // A really small int
    size_t max_file_size;
};

conf_t read_conf(const char *conf_file_name);

#endif //PROCESS_TEXT_CONF_READING_H
