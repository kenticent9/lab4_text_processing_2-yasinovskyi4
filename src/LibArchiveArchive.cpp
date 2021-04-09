#include "LibArchiveArchive.h"
#include <string>
#include <archive.h>
#include <archive_entry.h>

void LibArchiveArchive::init_from_memory(const std::string &data)
{
    if (archive_read_open_memory(arc, &data[0], data.size()) != ARCHIVE_OK) {
        std::string lib_err_msg{archive_error_string(arc)};
        throw ArchiveErrorOpenArchive("Error initializing from memory: " + lib_err_msg);
    }
}

uint64_t LibArchiveArchive::get_file_size()
{
    if (archive_entry_size_is_set(entry) == 0)
        throw ArchiveErrorFile{"File size is not set."};
    return archive_entry_size(entry);
}

std::string LibArchiveArchive::get_file_name()
{
    return archive_entry_pathname(entry);
}

bool LibArchiveArchive::next_file()
{
    res = archive_read_next_header(arc, &entry);
    if (res == ARCHIVE_EOF)
        return false;
    if (res != ARCHIVE_OK) {
        std::string lib_err_msg{archive_error_string(arc)};
        throw ArchiveErrorOpenFile("Error going to next file: " + lib_err_msg);
    }
    return true;
}

std::string LibArchiveArchive::get_whole_file()
{
    auto file_size = archive_entry_size(entry);
    std::string data(file_size, char{});

    la_ssize_t read_size = archive_read_data(arc, &data[0], data.size());
    if (read_size != file_size) {
        std::string lib_err_msg{archive_error_string(arc)};
        throw ArchiveErrorOpenFile("Error reading file: " + lib_err_msg);
    }
    return data;
}

// Other fields will be reassigned later anyway
LibArchiveArchive::LibArchiveArchive() {
    arc = archive_read_new();
    archive_read_support_filter_all(arc);
    archive_read_support_format_all(arc);
}

LibArchiveArchive::~LibArchiveArchive() {
    res = archive_read_free(arc);
}
