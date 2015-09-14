/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
 \******************************************************************************/

#include "findsymbol/Driver/Driver.h"
#include "findsymbol/Driver/Task.h"
#include "findsymbol/ObjectDump/ObjectDump.h"
#include "findsymbol/Support/Error.h"
#include "findsymbol/Support/Warning.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/raw_os_ostream.h"
#include "tbb/task_scheduler_init.h"

using namespace llvm;
using namespace findsymbol;

Driver::Driver(const std::vector<std::string>& entryPoints,
               const std::string& symbol,
               std::shared_ptr<Options> options,
               std::shared_ptr<Stats> stats)
    : symbol_(symbol),
      resultLibaries_(new QueueType),
      options_(options),
      stats_(stats)
{
    if(!options_->recursive)
    {
        for(const auto& libs : entryPoints)
        {
            if(!sys::fs::exists(libs))
                error({"No such file or directory: '", libs, "'"});
            ObjectDump dumper(symbol_, libs, *options_, *stats_);
            if(dumper.lookForSymbol())
                resultLibaries_->push(libs);
        }
    }
    else
    {
#ifdef FS_SINGLE_THREAD
        tbb::task_scheduler_init scheduler(1);
#endif

        tbb::task_list rootTaskList;
        for(const auto& libs : entryPoints)
        {
            if(!sys::fs::exists(libs))
            {
                warning({"No such directory: '", libs, "'"});
                continue;
            }
            
            Task& rootTask
                = *new(tbb::task::allocate_root())
                      Task(libs, symbol_, options_, stats_, resultLibaries_, 0);
            rootTaskList.push_back(rootTask);
        }
        tbb::task::spawn_root_and_wait(rootTaskList);
    }
}

void Driver::printResult()
{
    if(!resultLibaries_->empty())
    {
        outs() << "Symbol `" << symbol_ << "' found in:\n";
        std::string lib;
        while(resultLibaries_->try_pop(lib))
            outs() << " -- " << lib << "\n";
    }
    else
        outs() << "No libaries found for symbol `" << symbol_ << "'.\n";
}

