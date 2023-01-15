/*!
 * \file Lsvm.h
 * \author Luke Shore <lukethemodded@gmail.com>
 * \version 1.0
 * \date 15/01/2023
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
#include <extend_std/Vector.h>
#include <string>
struct VMData
{
    void (*vmPrintRegCallback)(uint64_t);
    void (*vmErrorCallback)(const char*);
    void (*vmExitCallback)(uint64_t);
};
/*!
 * \param[in] operations
 * \param[in] ops
 * \return losResult
 * \brief compile as it say creates and compiles source
 */
EXPORT_DLL void run(VMData* vm,const std::ReadOnlyVector<uint64_t> ops);