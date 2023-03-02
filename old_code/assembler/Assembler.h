/*!
 * \file Assembler.h
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
#include <extend_std/Vector.h>
#include <libos/Defines.h>
#include <string>
/*!
 * \param[out] byte_code
 * \param[in] src
 * \param[in] src_size
 * \return losResult
 * \brief assemble as it say and compiles source to byte_code
 */
EXPORT_CPP_DLL std::ReadOnlyVector<uint64_t> assemble(const char *src, const data_size_t src_size);

union LunaScriptByteCodeDecoder {
  private:
    uint64_t value;

  public:
    struct
    {
        uint64_t op : 5;
        uint64_t reg : 3;
        uint64_t is_reg : 1;
        uint64_t is_constant : 1;
        uint64_t reg_or_memory_dest : 53;
        uint64_t reserved_0 : 1 = 0;
    } op;
    explicit LunaScriptByteCodeDecoder(uint64_t value)
        : value(value)
    {
    }
};