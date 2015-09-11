/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/ObjectDump/NMSymbol.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/GlobalAlias.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/Object/ELFObjectFile.h"
#include "llvm/Object/IRObjectFile.h"
#include "llvm/Object/ObjectFile.h"

using namespace findsymbol;
using namespace llvm;
using namespace object;

template <class ELFT>
static char checkForGlobalTextSymbol(ELFObjectFile<ELFT>& Obj,
                                     basic_symbol_iterator I)
{
    typedef typename ELFObjectFile<ELFT>::Elf_Sym Elf_Sym;
    typedef typename ELFObjectFile<ELFT>::Elf_Shdr Elf_Shdr;

    symbol_iterator SymI(I);

    DataRefImpl Symb = I->getRawDataRefImpl();
    const Elf_Sym* ESym = Obj.getSymbol(Symb);
    const ELFFile<ELFT>& EF = *Obj.getELFFile();
    const Elf_Shdr* ESec = EF.getSection(ESym);

    if(ESec && (ESec->sh_type == ELF::SHT_PROGBITS
                || ESec->sh_type == ELF::SHT_DYNAMIC)
       && (ESec->sh_flags == ELF::SHF_ALLOC | ELF::SHF_EXECINSTR))
        return true;
    else
        return false;
}

NMSymbol::NMSymbol(SymbolicFile& obj, basic_symbol_iterator symIt)
    : isGlobalTextSymbol_(false)
{
    uint32_t symflags = symIt->getFlags();
    if((symflags & object::SymbolRef::SF_Weak))
        return;

    if(symflags & object::SymbolRef::SF_Undefined)
        return;

    if(symflags & object::SymbolRef::SF_Common)
        return;
    
    if(symflags & object::SymbolRef::SF_Absolute)
        return;

    if(!(symflags & object::SymbolRef::SF_Global))
        return;
    else if(ELF32LEObjectFile* ELF = dyn_cast<ELF32LEObjectFile>(&obj))
        isGlobalTextSymbol_ = checkForGlobalTextSymbol(*ELF, symIt);
    else if(ELF64LEObjectFile* ELF = dyn_cast<ELF64LEObjectFile>(&obj))
        isGlobalTextSymbol_ = checkForGlobalTextSymbol(*ELF, symIt);
    else if(ELF32BEObjectFile* ELF = dyn_cast<ELF32BEObjectFile>(&obj))
        isGlobalTextSymbol_ = checkForGlobalTextSymbol(*ELF, symIt);
    else if(ELF64BEObjectFile* ELF = dyn_cast<ELF64BEObjectFile>(&obj))
        isGlobalTextSymbol_ = checkForGlobalTextSymbol(*ELF, symIt);
}

