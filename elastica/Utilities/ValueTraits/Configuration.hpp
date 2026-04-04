//==============================================================================
/*!
//  \file
//  \brief Compile-time feature switches for value-trait math helpers
//
//  \ingroup type_traits
//
//  \details
//  Controls constexpr cmath usage and related compile-time options shared by
//  math traits under `Utilities/ValueTraits/`.
//
//  See LICENSE for details.
*/
//==============================================================================

#pragma once

namespace elastica {

//! Indicates whether the current compiler provides constexpr cmath support.
#if ((__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && \
     defined(ELASTICA_GCC_CXX11))
#define ELASTICA_HAS_CONSTEXPR_CMATH_FUNCTION
#endif

//! Selects builtin constexpr cmath usage when the compiler supports it.
#ifdef ELASTICA_HAS_CONSTEXPR_CMATH_FUNCTION
#define ELASTICA_USE_BUILTIN_CMATH_FUNCTION 1
#else  // #ifndef ELASTICA_CONFIG_DISABLE_BUILTIN_CMATH_FUNCTION
#define ELASTICA_USE_BUILTIN_CMATH_FUNCTION 0
#endif  // #ifndef ELASTICA_CONFIG_DISABLE_BUILTIN_CMATH_FUNCTION

}  // namespace elastica
