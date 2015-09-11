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
#ifndef FINDSYMBOL_WARNING_H
#define FINDSYMBOL_WARNING_H

#include "findsymbol/Config/Config.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <initializer_list>
#include <string>

namespace findsymbol {

INLINE void warning(std::string message)
{
    llvm::errs() << "findsymbol: warning: " << message << "\n";
    llvm::errs().flush();
}

INLINE void warning(std::initializer_list<std::string> message)
{
    llvm::errs() << "findsymbol: warning: ";
    for(const auto& m : message)
        llvm::errs() << m;
    llvm::errs() << "\n";
    llvm::errs().flush();
}

} // end namespace findsymbol

#endif

