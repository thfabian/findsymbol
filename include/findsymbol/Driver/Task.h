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
#ifndef FINDSYMBOL_TASK_H
#define FINDSYMBOL_TASK_H

#include "findsymbol/Config/Config.h"
#include "findsymbol/Driver/Driver.h"
#include "findsymbol/Driver/Options.h"
#include "findsymbol/Driver/Stats.h"
#include "tbb/task.h"
#include <memory>

namespace findsymbol
{

class Task : public tbb::task
{
private:
    /// Current directory
    std::string curDir_;

    /// Name of the searched for symbol
    std::string symbol_;

    /// Options
    std::shared_ptr<Options> options_;

    /// Statistics
    std::shared_ptr<Stats> stats_;

    /// Store the resulting libraries
    std::shared_ptr<Driver::QueueType> resultLibaries_;
    
    /// Recursion depth
    int recursionDepth_;

public:
    /// Create a Task object
    Task(const std::string& curDir,
         const std::string& symbol,
         std::shared_ptr<Options> options,
         std::shared_ptr<Stats> stats,
         std::shared_ptr<Driver::QueueType> resultLibaries,
         int recursionDepth);

    /// Overrides virtual function task::execute
    tbb::task* execute() override;
};

} // end namespace findsymbol

#endif

