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
#ifndef FINDSYMBOL_OBJECT_DUMP_H
#define FINDSYMBOL_OBJECT_DUMP_H

#include "findsymbol/Config/Config.h"
#include "findsymbol/Driver/Options.h"
#include "findsymbol/Driver/Stats.h"
#include "findsymbol/ObjectDump/NMSymbol.h"
#include "llvm/Object/ObjectFile.h"
#include <memory>
#include <string>
#include <vector>

namespace findsymbol 
{

class ObjectDump
{
private:
    /// Name of the searched for symbol
    std::string symbol_;
    
    /// Name of the object/archive
    std::string filename_;
    
    /// Symbol table of the objects
    std::vector<NMSymbol> symbolTable_;
    
    /// Options
    const Options& options_;
    
    /// Statistics
    Stats& stats_;

public:
    /// Initialize object with filename & symbolname
    ObjectDump(const std::string& symbol, 
               const std::string& filename,
               const Options& options,
               Stats& stats);
    
    /// Look for the symbol in the provided library
    bool lookForSymbol();
    
    /// Get the name of the object/archive
    std::string getFileName() const { return filename_; }
    
    /// Get the name of the searched for symbol
    std::string getSymbol() const { return filename_; }
    
private:
    /// Look for the symbol in an object file
    bool lookForSymbolInObject(llvm::object::SymbolicFile& Obj);
};

} // end namespace findsymbol

#endif
