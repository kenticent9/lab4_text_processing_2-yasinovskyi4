#include "ctr_writing.h"
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>

typedef std::pair<std::string, unsigned> entry_t;

static void write_ctr_to_out_by_a(const counter_t &ctr,
                                  const std::string &out_by_a_name);
static void write_ctr_to_out_by_n(const counter_t &ctr,
                                  const std::string &out_by_n_name);
bool cmp(const entry_t &lhs, const entry_t &rhs);

// We can make this function concurrent by writing to each file in
// parallel
void write_ctr_to_files(const counter_t &ctr, const conf_t &conf)
{
    write_ctr_to_out_by_a(ctr, conf.out_by_a);
    write_ctr_to_out_by_n(ctr, conf.out_by_n);
}

void write_ctr_to_out_by_a(const counter_t &ctr,
                           const std::string &out_by_a_name)
{
    std::ofstream out_by_a(out_by_a_name);
    if (!out_by_a)
        throw std::runtime_error("Couldn't open file for writing");

    for (const auto &pair : ctr)
        out_by_a << pair.first << ": " << pair.second << '\n';

    out_by_a.close();
}

void write_ctr_to_out_by_n(const counter_t &ctr,
                           const std::string &out_by_n_name)
{
    std::ofstream out_by_n(out_by_n_name);
    if (!out_by_n)
        throw std::runtime_error("Couldn't open out_by_n for writing");

    std::vector<entry_t> v;
    for (const auto &pair : ctr)
        v.emplace_back(pair);

    std::sort(v.begin(), v.end(), cmp);

    for (const auto &pair : v)
        out_by_n << pair.first << ": " << pair.second << '\n';

    out_by_n.close();
}

bool cmp(const entry_t &lhs, const entry_t &rhs)
{
    if (lhs.second == rhs.second)
        return lhs.first < rhs.first;
    return lhs.second > rhs.second;
}
