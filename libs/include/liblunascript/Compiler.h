/*!
 * \file Compiler.h
 * \author Luke Shore <lukethemodded@gmail.com>
 * \version 1.0
 * \date 03/01/2023
 * \brief this is the api for the library libLunaScript
 */
#pragma once
// LIBOS LICENCE
//
// GNU Lesser General Public License Version 3.0
//
// Copyright Luke Shore (c) 2020, 2022
/*! Importation of libraries*/
#include <libos/Defines.h>
/*!
 *  \brief this is the Library object that is used to compile the source as the user only holds a
 * pointer to the library object that is controlled by the library.
 */
typedef struct Compiler_T *Compiler;

/*!
 * \param[out] compiler
 * \param[in] src
 * \param[in] src_size
 * \param[in] filename
 * \param[in] filename_size
 * \return losResult
 * \brief compile as it say creates and compiles source
 */
EXPORT_DLL losResult compile(Compiler *compiler, const char *src, const data_size_t src_size, const char *filename,
                             const data_size_t filename_size);

EXPORT_DLL uint8_t hasErrorOnStack(Compiler compiler);
EXPORT_DLL void getErrorOffStack(Compiler compiler, char **str, data_size_t *str_size);
EXPORT_DLL void astToString(Compiler compiler, char **str, data_size_t *str_size);
EXPORT_DLL void toByteCode(Compiler compiler, char **str, data_size_t *str_size);

EXPORT_DLL void freeCompiler(Compiler compiler);

#ifdef __cplusplus
#include <string>
class LunaScriptCompiler
{
    Compiler compiler = nullptr;
    losResult err;

  public:
    explicit LunaScriptCompiler(std::string src, std::string filename)
    {
        err = compile(&compiler, src.c_str(), src.size(), filename.c_str(), filename.size());
    }

    ~LunaScriptCompiler()
    {
        freeCompiler(compiler);
    }

    losResult didScriptCompile() const noexcept
    {
        return err;
    }

    std::string getErrors()
    {
        std::string error;
        while (hasErrorOnStack(compiler))
        {
            char *buffer = nullptr;
            data_size_t size = 0;
            getErrorOffStack(compiler, &buffer, &size);
            error += std::string(buffer, 0,size);
            error += "\n";
        }
        return error;
    }

    std::string getJsonAST()
    {
        char *json = nullptr;
        data_size_t json_size = 0;
        astToString(compiler, &json, &json_size);
        return std::string(json, 0, json_size);
    }

    std::string getByteCode()
    {
        char *bytecode = nullptr;
        data_size_t bytecode_size = 0;
        toByteCode(compiler, &bytecode, &bytecode_size);
        return std::string(bytecode, 0, bytecode_size);
    }
};
#endif