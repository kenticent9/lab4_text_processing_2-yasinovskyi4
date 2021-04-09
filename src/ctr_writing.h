#ifndef PROCESS_TEXT_CTR_WRITING_H
#define PROCESS_TEXT_CTR_WRITING_H

#include "conf_reading.h"
#include <string>
#include <map>

typedef std::map<std::string, unsigned> counter_t;

void write_ctr_to_files(const counter_t &ctr, const conf_t &conf);

#endif //PROCESS_TEXT_CTR_WRITING_H
