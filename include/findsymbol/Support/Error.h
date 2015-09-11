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
#ifndef FINDSYMBOL_ERROR_H
#define FINDSYMBOL_ERROR_H

#include "findsymbol/Config/Config.h"
#include "llvm/Support/raw_ostream.h"
#include <cstdlib>
#include <initializer_list>
#include <string>

namespace findsymbol {

NORETURN INLINE void error(std::string message)
{
    llvm::errs() << "findsymbol: error: " << message << "\n";
    llvm::errs().flush();
    std::exit(EXIT_FAILURE);
}

NORETURN INLINE void error(std::initializer_list<std::string> message)
{
    llvm::errs() << "findsymbol: error: ";
    for(const auto& m : message)
        llvm::errs() << m;
    llvm::errs() << "\n";
    llvm::errs().flush();
    std::exit(EXIT_FAILURE);
}

} // end namespace findsymbol

#endif

