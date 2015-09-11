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
#ifndef FINDSYMBOL_DRIVER_H
#define FINDSYMBOL_DRIVER_H

#include "findsymbol/Config/Config.h"
#include "findsymbol/Driver/Options.h"
#include "findsymbol/Driver/Stats.h"
#include "tbb/concurrent_queue.h"
#include "tbb/task.h"
#include <memory>
#include <string>
#include <vector>

namespace findsymbol
{

class Driver
{
public:
    typedef tbb::concurrent_queue<std::string> QueueType;

private:
    /// Searched for symbol
    std::string symbol_;

    /// Store the resulting libraries
    std::shared_ptr<QueueType> resultLibaries_;

    /// Options for this run
    std::shared_ptr<Options> options_;

    /// Stats for this run
    std::shared_ptr<Stats> stats_;

public:
    /// Start the recursive traversal at each entry point by spwaning a task
    Driver(const std::vector<std::string>& entryPoints,
           const std::string& symbol,
           std::shared_ptr<Options> options,
           std::shared_ptr<Stats> stats);

    /// Print the result of this run to stdout
    void printResult();
};

} // end namespace findsymbol

#endif

