/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Driver/Task.h"
#include "findsymbol/ObjectDump/ObjectDump.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/raw_os_ostream.h"
#include <system_error>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

using namespace llvm;
using namespace findsymbol;

// Return true if the file is a symlink
static bool is_symlink(const char* filename)
{
    struct stat p_statbuf;
    if(lstat(filename, &p_statbuf) < 0)
        return false;
    return (S_ISLNK(p_statbuf.st_mode) == 1);
}

Task::Task(const std::string& curDir,
           const std::string& symbol,
           std::shared_ptr<Options> options,
           std::shared_ptr<Stats> stats,
           std::shared_ptr<Driver::QueueType> resultLibaries,
           int recursionDepth)
    : curDir_(curDir),
      symbol_(symbol),
      options_(options),
      stats_(stats),
      resultLibaries_(resultLibaries),
      recursionDepth_(recursionDepth)
{
}

tbb::task* Task::execute()
{
    // Initialize the ref count with 1 as we wait in the end
    set_ref_count(1);

    tbb::task_list taskList;
    std::error_code ec;

    for(sys::fs::directory_iterator dirIt(curDir_, ec);
        !ec && dirIt != sys::fs::directory_iterator();
        dirIt.sys::fs::directory_iterator::increment(ec))
    {
        sys::fs::file_status st;
        if((ec = dirIt->status(st)))
            break;

        // We don't follow any symlinks as we might get trapped in cyclic
        // dependencies
        if(is_directory(st) && !is_symlink(dirIt->path().c_str())
           && recursionDepth_ < options_->recursionDepth)
        {
            Task& child = *new(tbb::task::allocate_child())
                              Task(dirIt->path(), symbol_, options_, stats_,
                                   resultLibaries_, recursionDepth_ + 1);
            increment_ref_count();
            taskList.push_back(child);
        }
        else if(is_regular_file(st) && !is_symlink(dirIt->path().c_str()))
        {
            ObjectDump dumper(symbol_, dirIt->path(), *options_, *stats_);
            if(dumper.lookForSymbol())
                resultLibaries_->push(dirIt->path());
        }
    }

    if(!taskList.empty())
        spawn_and_wait_for_all(taskList);

    return nullptr;
}

