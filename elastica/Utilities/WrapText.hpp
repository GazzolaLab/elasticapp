//==============================================================================
/*!
//  \file
//  \brief Utilities to wrap up a string into a nice easy-on-the-eyes format
//
//  Copyright (C) 2020-2020 Tejaswin Parthasarathy - All Rights Reserved
//  Copyright (C) 2026 GazzolaLab - All Rights Reserved
//
//  See LICENSE for details.
//
// Reused from SpECTRE : https://spectre-code.org/
// Distributed under the MIT License.
// See LICENSE.txt for details.
*/
//==============================================================================

#pragma once

//******************************************************************************
// Includes
//******************************************************************************

#include <cstddef>
#include <string>

//******************************************************************************
/* \brief Wrap the string `str` so that it is no longer than `line_length` and
// indent each new line with `indentation`. The first line is also indented.
// \ingroup UtilitiesGroup
//
// Single words longer than `line_length` are hyphenated.
*/
std::string wrap_text(std::string str, size_t line_length,
                      const std::string& indentation = "") noexcept;
//******************************************************************************
