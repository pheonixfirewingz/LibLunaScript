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
#include <string>
/*!
 *  \brief this is the Library object that is used to compile the source as the user only holds a
 * pointer to the library object that is controlled by the library.
 */
typedef struct Compiler_T * Compiler;

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
EXPORT_DLL void getErrorOffStack(Compiler compiler,char** str,data_size_t* str_size);
EXPORT_DLL void astToString(Compiler compiler,char** str,data_size_t* str_size);

EXPORT_DLL void freeCompiler(Compiler compiler);