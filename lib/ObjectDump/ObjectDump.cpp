/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Demangle/Demangle.h"
#include "findsymbol/ObjectDump/ObjectDump.h"
#include "findsymbol/Support/Error.h"
#include "findsymbol/Support/DebugLog.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Object/Archive.h"
#include "llvm/Object/ObjectFile.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Support/MemoryBuffer.h"
#include <cstring>

using namespace findsymbol;
using namespace llvm;
using namespace object;

ObjectDump::ObjectDump(const std::string& symbol,
                       const std::string& filename,
                       const Options& options,
                       Stats& stats)
    : symbol_(symbol), filename_(filename), options_(options), stats_(stats)
{
}

bool ObjectDump::lookForSymbolInObject(SymbolicFile& Obj)
{
    basic_symbol_iterator symItBegin = Obj.symbol_begin();
    basic_symbol_iterator symItEnd = Obj.symbol_end();

    std::string nameBuffer;
    raw_string_ostream OS(nameBuffer);
    std::size_t symbolCounter = 0;

    // Check if we deal with a dynamic symbol table
    if((symItBegin == symItEnd))
    {
        if(!Obj.isELF())
            return false;

        std::pair<symbol_iterator, symbol_iterator> IDyn
            = getELFDynamicSymbolIterators(&Obj);
        symItBegin = IDyn.first;
        symItEnd = IDyn.second;
    }

    // Convert all symbols in the object to NMSymbols
    for(basic_symbol_iterator it = symItBegin; it != symItEnd;
        ++it, ++symbolCounter)
    {
        NMSymbol nmSymbol(Obj, it);

        // we only consider global text symbols
        if(nmSymbol.isGlobalTextSymbol())
        {
            it->printName(OS);
            OS << '\0';
            symbolTable_.push_back(nmSymbol);
        }
    }
    OS.flush();

    if(options_.gatherStatistics)
        stats_.incrementSymbolCounter(symbolCounter);

    // Retrieve symbol name
    const char* p = nameBuffer.c_str();
    for(std::size_t i = 0; i < symbolTable_.size(); ++i)
    {
        symbolTable_[i].setSymbolName(p);
        p += std::strlen(p) + 1;
    }

    // Look for the symbol
    if(!options_.demangle)
    {
        for(const auto& symEntry : symbolTable_)
            if(symEntry.getSymbolName().equals(symbol_))
            {
                symbolTable_.clear();
                return true;
            }
    }
    else
    {
        // Try to demangle all C++ symbols
        for(const auto& symEntry : symbolTable_)
        {
            StringRef symbolName = symEntry.getSymbolName();
            std::unique_ptr<char> symbolDemangled
                = std::move(demangleSymbol(symbolName.data()));

            if(symbolDemangled.get() != nullptr)
            {
                std::string symbolNameDemangled(symbolDemangled.get());
                if(symbolNameDemangled == symbol_)
                {
                    symbolTable_.clear();
                    return true;
                }
            }
            else if(symbolName.equals(symbol_))
            {
                symbolTable_.clear();
                return true;
            }
        }
    }

    symbolTable_.clear();
    return false;
}

bool ObjectDump::lookForSymbol()
{
    if(options_.gatherStatistics)
        stats_.incrementFileCounter();

    // Open the specified file as a MemoryBuffer
    ErrorOr<std::unique_ptr<MemoryBuffer>> bufferOrErr
        = MemoryBuffer::getFileOrSTDIN(filename_);

    if(bufferOrErr.getError())
    {
        DEBUG_LOG_ERR(bufferOrErr.getError().message(), ": '", filename_, "'");
        return false;
    }

    // Get the context to handle data of LLVM's core infrastructure
    LLVMContext& context = getGlobalContext();

    // Create a Binary from Source
    ErrorOr<std::unique_ptr<Binary>> binaryOrErr
        = createBinary(bufferOrErr.get()->getMemBufferRef(), &context);

    if(binaryOrErr.getError())
    {
        DEBUG_LOG_ERR(binaryOrErr.getError().message(), ": '", filename_, "'");
        return false;
    }

    Binary& bin = *binaryOrErr.get();

    // Iterate over objects in the Archive
    if(Archive* A = dyn_cast<Archive>(&bin))
    {
        DEBUG_LOG("dumping archive: '", filename_, "'");

        for(Archive::child_iterator I = A->child_begin(), E = A->child_end();
            I != E; ++I)
        {
            ErrorOr<std::unique_ptr<Binary>> childOrErr
                = I->getAsBinary(&context);

            if(childOrErr.getError())
                continue;

            if(SymbolicFile* O = dyn_cast<SymbolicFile>(&*childOrErr.get()))
            {
                if(lookForSymbolInObject(*O))
                    return true;
            }
        }
        return false;
    }

    if(SymbolicFile* O = dyn_cast<SymbolicFile>(&bin))
    {
        DEBUG_LOG("dumping symbolic: '", filename_, "'");
        return lookForSymbolInObject(*O);
    }

    DEBUG_LOG_ERR("unrecognizable file type: '", filename_, "'");
    return false;
}

