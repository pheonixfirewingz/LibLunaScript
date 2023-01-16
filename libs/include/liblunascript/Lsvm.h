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
#include <extend_std/LookUpTable.h>
#include <extend_std/Vector.h>
#include <functional>
#include <libos/Defines.h>
#include <stack>
#include <variant>

typedef std::variant<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t, float32_t, float64_t>
    vm_data_t;

struct VMFunctionName
{
    uint64_t hash : 53;
    VMFunctionName(const std::string &name)
        : hash(std::hash<std::string>{}(name))
    {
    }
};

struct VMData
{
    std::ReadOnlyLookupTable<uint64_t, std::function<vm_data_t *(std::stack<vm_data_t> *)>> vmCallbacks;
    void (*vmErrorCallback)(const char *);
};
/*!
 * \param[in] operations
 * \param[in] ops
 * \return losResult
 * \brief compile as it say creates and compiles source
 */
EXPORT_DLL void run(VMData *vm, const std::ReadOnlyVector<uint64_t> ops,uint8_t debug_mode);