/*!
 * \file Compiler.h
 * \author Luke Shore <lukethemodded@gmail.com>
 * \version 1.0
 * \date 03/01/2023
 * \brief this is the api for the library libLunaScript
 */
#pragma once
#if ON_WINDOWS
#    if __has_include(<windows.h>)
#        include <windows.h>
#    endif
#endif
// LIBOS LICENCE
//
// GNU Lesser General Public License Version 3.0
//
// Copyright Luke Shore (c) 2020, 2022
/*! Importation of libraries*/
#include <libos/Defines.h>
#include <libos/Error.h>
/*!
 *  \brief this is the Library object that is used to compile the source as the user only holds a
 * pointer to the library object that is controlled by the library.
 */
typedef struct Compiler_T *Compiler;

/*!
 * \param[out] compiler
 * \param[in] src
 * \param[in] src_size
 * \return losResult
 * \brief compile as it say creates and compiles source
 */
EXPORT_DLL losResult compile(Compiler *compiler, const char *src, const size_t src_size, const uint8_t debug);
EXPORT_DLL uint8_t hasErrorOnStack(Compiler compiler);
EXPORT_DLL void getErrorOffStack(Compiler compiler, char **str, size_t *str_size);
EXPORT_DLL void astToString(Compiler compiler, char **str, size_t *str_size);
EXPORT_DLL void freeCompiler(Compiler compiler);

//temp
EXPORT_DLL void newCompile(const wchar_t *src, const size_t src_size,wchar_t** data_out,size_t* data_out_size);

//used for compatibility with C#
EXPORT_DLL void freeString(char *src);

#ifdef __cplusplus
#    include <string>
class LunaScriptCompiler
{
    std::wstring src_hold;
  public:
    explicit LunaScriptCompiler(std::wstring src) :src_hold(src)
    {
    }

    ~LunaScriptCompiler()
    {
    }

    losResult didScriptCompile() const noexcept
    {
        return LOS_SUCCESS;
    }

    std::string getErrors()
    {
        return "";
    }

    std::wstring getJsonAST()
    {
        wchar_t *json = nullptr;
        size_t json_size = 0;
        newCompile(src_hold.c_str(),src_hold.size(), &json, &json_size);
        return std::wstring(json, 0, json_size);
    }
};
#endif