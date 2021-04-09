#ifndef PROCESS_TEXT_LIBARCHIVEARCHIVE_H
#define PROCESS_TEXT_LIBARCHIVEARCHIVE_H

#include <stdexcept>
#include <archive.h>
#include <archive_entry.h>

// Errors
struct ArchiveErrorFile : std::runtime_error {
    ArchiveErrorFile(const std::string &text):
        std::runtime_error(text) {}
};

struct ArchiveErrorOpenArchive : ArchiveErrorFile {
    ArchiveErrorOpenArchive(const std::string &text):
            ArchiveErrorFile(text) {}
};

struct ArchiveErrorOpenFile : ArchiveErrorFile {
    ArchiveErrorOpenFile(const std::string &text):
            ArchiveErrorFile(text) {}
};

// The archive itself
class LibArchiveArchive {
public:
    LibArchiveArchive();

    ~LibArchiveArchive();

    void init_from_memory(const std::string &data);

    uint64_t get_file_size();

    std::string get_file_name();

    bool next_file();

    std::string get_whole_file();

private:
    struct archive *arc;
    struct archive_entry *entry;
    int res;
};

#endif //PROCESS_TEXT_LIBARCHIVEARCHIVE_H
