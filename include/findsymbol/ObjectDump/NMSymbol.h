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
#ifndef FINDSYMBOL_NM_SYMBOL_H
#define FINDSYMBOL_NM_SYMBOL_H

#include "findsymbol/Config/Config.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Object/ObjectFile.h"

namespace findsymbol
{

class NMSymbol
{
private:
    /// We only consider global text symbols
    bool isGlobalTextSymbol_;

    /// Name of the Symbol (stored as StringRef (!))
    llvm::StringRef symbolName_;

public:
    /// Construct the symbol object
    NMSymbol(llvm::object::SymbolicFile& Obj,
             llvm::object::basic_symbol_iterator I);

    /// Get the name of the symbol
    llvm::StringRef getSymbolName() const { return symbolName_; }

    /// Get the type of the symbol
    bool isGlobalTextSymbol() const { return isGlobalTextSymbol_; }

    /// Set the name of the symbol
    void setSymbolName(llvm::StringRef symbolName) { symbolName_ = symbolName; }
};

} // end namespace findsymbol

#endif

