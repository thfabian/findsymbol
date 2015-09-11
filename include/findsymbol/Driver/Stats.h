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
#ifndef FINDSYMBOL_STATS_H
#define FINDSYMBOL_STATS_H

#include "findsymbol/Config/Config.h"
#include <atomic>

namespace findsymbol
{

class Stats
{
private:
    /// Number of parsed symbols
    std::atomic<std::size_t> symbolCounter_;
    
    /// Number of scanned files
    std::atomic<std::size_t> fileCounter_;

public:
    /// Initialize all variables
    Stats() : symbolCounter_(0), fileCounter_(0) {}

    /// Increment the symbol counter [thread safe]
    void incrementSymbolCounter(std::size_t inc) { symbolCounter_ += inc; }
    
    /// Increment the file counter [thread safe]
    void incrementFileCounter() { fileCounter_++; }
    
    /// Get the file counter
    std::size_t getFileCounter() const { return fileCounter_.load(); }
    
    /// Print all the gathered informations
    /// @argument elapsedTime Elapsed time in milliseconds
    void printStatisticSummary(double elapsedTime) const;
};

} // end namespace findsymbol

#endif

