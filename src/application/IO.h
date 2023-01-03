#pragma once
#include <libos/Defines.h>


void setRoot(const char* root) noexcept;
losResult fileRead(const char *path, const int path_size, char **buf, data_size_t *buf_size) noexcept;
losResult fileWrite(const char *path, const int path_size, const char *buf, data_size_t buf_size,bool create = true) noexcept;
void fileDelete(const char* path,const int path_size);