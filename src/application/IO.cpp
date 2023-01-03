#include "IO.h"
#include <libos/FileIO.h>

void setRoot(const char *root) noexcept
{
    losSetAssetPath(root);
}

losResult fileRead(const char *path, const int path_size, char **buf, data_size_t *buf_size) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    file.fileBits = LOS_FILE_BIT_READ;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losReadFile(handle, (void **)buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

losResult fileWrite(const char *path, const int path_size, const char *buf, data_size_t buf_size, bool create) noexcept
{
    losFileHandle handle;
    losFileOpenInfo file;
    if (create)
        file.fileBits = LOS_FILE_BIT_WRITE | LOS_FILE_BIT_CREATE;
    else
        file.fileBits = LOS_FILE_BIT_WRITE;
    file.path = path;
    file.path_size = path_size;
    losResult res;
    if ((res = losOpenFile(&handle, file)) != LOS_SUCCESS)
        return res;
    if ((res = losWriteFile(handle, buf, buf_size)) != LOS_SUCCESS)
        return res;
    if ((res = losCloseFile(handle)) != LOS_SUCCESS)
        return res;
    return LOS_SUCCESS;
}

void fileDelete(const char *path, const int path_size)
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