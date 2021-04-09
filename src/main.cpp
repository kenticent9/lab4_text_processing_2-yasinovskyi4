#include "conf_reading.h"
#include "ctr_writing.h"
#include "LibArchiveArchive.h"
#include "time_measurement.h"
#include <iostream>
#include <string>
#include <queue>
#include <map>
#include <thread>
#include <boost/filesystem.hpp>
#include <boost/locale.hpp>

namespace fs = boost::filesystem;
namespace ba = boost::locale::boundary;

typedef std::map<std::string, unsigned> counter_t;

std::queue<std::string> get_buffers(const std::string &indir);
void generate_locale();
std::queue<counter_t> get_counters(std::queue<std::string> &buffers,
                                   const conf_t &conf);
void count_words(std::queue<std::string> &buffers,
                 std::queue<counter_t> &counters,
                 size_t max_file_size);
counter_t merge_counters(std::queue<counter_t> &counters);

int main(int argc, char *argv[])
{
    if (argc > 2)
        throw std::runtime_error("Usage: process_text conf_file");

    conf_t conf = read_conf( (argc == 2) ? argv[1] : "../conf.dat" );

    generate_locale();

    auto stage1_start_time = get_current_time_fenced();

    std::queue<std::string> files = get_buffers(conf.indir);

    auto stage2_start_time = get_current_time_fenced();

    std::queue<counter_t> counters = get_counters(files, conf);

    counter_t ctr = merge_counters(counters);

    auto finish_time = get_current_time_fenced();

    write_ctr_to_files(ctr, conf);

    auto total_time = finish_time - stage1_start_time;
    auto stage1_time = stage2_start_time - stage1_start_time;
    auto stage2_time = finish_time - stage2_start_time;

    std::cout << "Total:    " << to_us(total_time) << " microsec\nReading:  "
        << to_us(stage1_time) << " microsec\nIndexing: " << to_us(stage2_time)
        << " microsec" << std::endl;

    return 0;
}

std::queue<std::string> get_buffers(const std::string &indir)
{
    std::queue<std::string> buffers;

    fs::recursive_directory_iterator end;
    for (fs::recursive_directory_iterator it(indir); it != end; it++) {
        if (fs::is_regular_file(it->path())) {
            const std::string &extn = boost::locale::to_lower(it->path().extension().string());
            if (extn == ".zip") {
                const std::string &file_name = it->path().string();

                std::ifstream raw_file(file_name, std::ios::binary);
                if (!raw_file)
                    throw std::runtime_error("Couldn't open raw_file for reading");

                auto buffer = [&raw_file] {
                    std::ostringstream ss{};
                    ss << raw_file.rdbuf();
                    return ss.str();
                }();

                buffers.push(std::move(buffer));
            }
        }
    }

    return buffers;
}

std::queue<counter_t> get_counters(std::queue<std::string> &buffers,
                                   const conf_t &conf)
{
    std::queue<counter_t> counters;

    std::vector<std::thread> threads;
    threads.reserve(conf.indexing_threads);

    for (int i = 0; i < conf.indexing_threads; i++)
        threads.emplace_back(count_words, std::ref(buffers),
                             std::ref(counters), conf.max_file_size);

    for (auto &t : threads)
        t.join();

    return counters;
}

void generate_locale()
{
    boost::locale::generator gen;
    std::locale::global(gen("en_US.UTF-8"));
}

std::mutex m1;
std::mutex m2;

void count_words(std::queue<std::string> &buffers,
                 std::queue<counter_t> &counters,
                 size_t max_file_size)
{
    counter_t ctr;

    // While the queue of the file names is not empty
    for (;;) {
        std::string buffer;
        {
        std::lock_guard<std::mutex> lck{m1};
        if (buffers.empty())
            break;
        buffer = std::move(buffers.front());
        buffers.pop();
        }

        try {
            LibArchiveArchive archive;
            archive.init_from_memory(buffer);

            // While there is next file in the archive
            while (archive.next_file()) {
                auto file_size = archive.get_file_size();
                if (file_size > max_file_size)
                    continue;

                auto cur_file_name = archive.get_file_name();
                std::string extn = boost::locale::to_lower(cur_file_name.substr(cur_file_name.find_last_of('.')+1));
                if (extn != "txt")
                    continue;

                std::string text = archive.get_whole_file();

                ba::ssegment_index map(ba::word, text.begin(), text.end());
                map.rule(ba::word_any);

                for (ba::ssegment_index::iterator it = map.begin(), e = map.end(); it != e; ++it)
                    ctr[boost::locale::normalize(boost::locale::fold_case(it->str()))]++;
            }
        } catch (const std::exception &e) {
            std::cout << e.what() << std::endl;
        }
    }

    {
    std::lock_guard<std::mutex> lck{m2};
    counters.push(ctr);
    }
}

// TODO: Parallel this function
counter_t merge_counters(std::queue<counter_t> &counters)
{
    counter_t ctr = counters.front();
    counters.pop();

    while (!counters.empty()) {
        counter_t cur_ctr = counters.front();
        counters.pop();
        for (const auto &pair : cur_ctr)
            ctr[pair.first] += pair.second;
    }

    return ctr;
}
