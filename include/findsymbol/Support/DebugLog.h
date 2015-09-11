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
#ifndef FINDSYMBOL_DEBUG_LOG_H
#define FINDSYMBOL_DEBUG_LOG_H

#include "findsymbol/Config/Config.h"
#include <string>
#include <initializer_list>

#ifdef FINDSYMBOL_LOG_ON
#define DEBUG_LOG(...)                                                         \
    findsymbol::log::debugLog({__VA_ARGS__}, __FILE__, __LINE__, __func__,     \
                              findsymbol::log::Severity::Trivial)
#define DEBUG_LOG_ERR(...)                                                     \
    findsymbol::log::debugLog({__VA_ARGS__}, __FILE__, __LINE__, __func__,     \
                              findsymbol::log::Severity::Error)
#else
#define DEBUG_LOG(...) (static_cast<void>(0))
#define DEBUG_LOG_ERR(...) (static_cast<void>(0))
#endif

namespace findsymbol
{

namespace log
{

enum class Severity
{
    Trivial,
    Error
};

/// logging to stdout [thread safe]
void debugLog(std::initializer_list<std::string> message,
              const char* file,
              const int line,
              const char* function,
              findsymbol::log::Severity severity);

} // end namespace log

} // end namespace findsymbol

#endif

