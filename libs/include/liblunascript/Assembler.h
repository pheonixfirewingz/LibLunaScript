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
#include <libos/Defines.h>
#include <extend_std/Vector.h>
#include <string>
/*!
 * \param[out] byte_code
 * \param[in] src
 * \param[in] src_size
 * \return losResult
 * \brief assemble as it say and compiles source to byte_code
 */
EXPORT_CPP_DLL std::ReadOnlyVector<uint64_t> assemble(const char *src, const data_size_t src_size);