/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Driver/Stats.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_os_ostream.h"
#include <cstdio>

using namespace findsymbol;
using namespace llvm;

void Stats::printStatisticSummary(double elapsedTime) const
{
    std::size_t symbolCounter = symbolCounter_.load();
    std::size_t fileCounter = fileCounter_.load();
    outs().flush();
    std::printf("%-20s %8lu \n", "Parsed symbols: ", symbolCounter);
    std::printf("%-20s %8lu \n", "Scanned files: ", fileCounter);
    std::printf("%-20s %8.0f ms\n", "Elapsed time: ", elapsedTime);
}



