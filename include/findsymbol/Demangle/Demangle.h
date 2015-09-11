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
#ifndef FINDSYMBOL_DEMANGLE_H
#define FINDSYMBOL_DEMANGLE_H

#include "findsymbol/Config/Config.h"
#include <memory>

namespace findsymbol {

/// Demangle C++ mangled symbols if possible. The function returns a pointer
/// to the demangled symbol or, in case of a failure, a nullptr    
std::unique_ptr<char> demangleSymbol(const char* mangled);

} // end namespace findsymbol

#endif
