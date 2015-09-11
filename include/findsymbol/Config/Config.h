/*******************************************************************- C++ -****\
 *                               
 *                          FindSymbol v1.0    
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See 
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#pragma once
#ifndef FINDSYMBOL_CONFIG_H
#define FINDSYMBOL_CONFIG_H

#define FINDSYMBOL_VERSION_MAJOR 1
#define FINDSYMBOL_VERSION_MINOR 0
#define FINDSYMBOL_VERSION_STRING "1.0"

// NORETURN compiler intrinsic
#if defined(_MSC_VER)
#define NORETURN __declspec(noreturn)
#elif defined(__GNUC__)
#define NORETURN __attribute__((noreturn))
#else
#define NORETURN
#endif

// NOEXCEPT keyword
#if defined(__GNUC__)
#define NOEXCEPT noexcept
#elif defined(_MSC_VER)
#define NOEXCEPT throw ()
#else
#define NOEXCEPT
#endif

// CONSTEXPR keyword
#if defined(__GNUC__)
#define CONSTEXPR constexpr
#elif defined(_MSC_VER)
#define CONSTEXPR const
#else
#define CONSTEXPR const
#endif  

// INLINE compiler intrinsic
#if defined(__GNUC__)
#define INLINE inline __attribute__((always_inline))
#elif defined(_MSC_VER)
#define INLINE inline __forceinline
#else
#define INLINE inline
#endif

#endif
