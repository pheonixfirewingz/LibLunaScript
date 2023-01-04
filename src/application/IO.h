#pragma once
#include <libos/Defines.h>

void setRoot(const char* root) noexcept;
losResult fileRead(const std::string path, char **buf, data_size_t *buf_size) noexcept;
losResult fileWrite(const std::string path, const char *buf,const data_size_t buf_size,bool create = true) noexcept;
void fileDelete(const char* path,const int path_size);

const inline std::string createP(const std::string extend,const char* file_name)
{
    std::string ret("$[asset_base]/tests/");
    ret += extend;
    ret += "/";
    ret += file_name;
    ret += ".lls";
    return ret;
}