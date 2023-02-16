#pragma once
#include <libos/Defines.h>
#include <libos/FileIO.h>
#include <string>

void setRoot(const char* root) noexcept
{
    losSetAssetPath(root);
}

losResult fileRead(const std::string path, char **buf, data_size_t *buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ;
    file.path = path.c_str();
    file.path_size = path.size();
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losReadFile(handle, (void **)buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

template <typename T>
losResult fileWrite(const std::string path, const T *buf,const data_size_t buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    if (losDoseFileExist(path.c_str()) != LOS_SUCCESS)
        file.fileBits = LOS_FILE_BIT_WRITE | LOS_FILE_BIT_CREATE;
    else
        file.fileBits = LOS_FILE_BIT_WRITE;
    file.path = path.c_str();
    file.path_size = path.size();
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losWriteFile(handle, buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

void fileDelete(const char* path,const int path_size)
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_DELETE_AFTER_CLOSE;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return;
}

const inline std::string createP(const std::string extend,const char* file_name,const char * file_ext = ".lls") noexcept
{
    std::string ret("$[asset_base]/");
    ret += extend;
    ret += file_name;
    ret += file_ext;
    return ret;
}
