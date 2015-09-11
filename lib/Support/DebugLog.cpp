/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Support/DebugLog.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/raw_ostream.h"
#include <ctime>
#include <cstdio>
#include <mutex>
#include <string>
#include <thread>

using namespace llvm;

static std::mutex mutexIO;

void findsymbol::log::debugLog(std::initializer_list<std::string> message,
                               const char* file,
                               const int line,
                               const char* function,
                               findsymbol::log::Severity severity)
{
    // Get current time
    time_t currentTime;
    struct tm* localTime;
    std::time(&currentTime);
    localTime = localtime(&currentTime);

    // Get thread ID
    std::thread::id threadID = std::this_thread::get_id();

    // Log to stdout
    std::lock_guard<std::mutex> lock(mutexIO);

    bool changeColor = severity == findsymbol::log::Severity::Error
                       && outs().has_colors();

    if(changeColor)
        outs().changeColor(raw_ostream::Colors::RED, true);

    outs() << llvm::format("[%X] ", threadID);

    if(changeColor)
        outs().changeColor(raw_ostream::Colors::WHITE, true);

    outs() << format("[%02i:%02i:%02i] ", localTime->tm_hour, localTime->tm_min,
                     localTime->tm_sec);
    outs() << "[" << llvm::sys::path::filename(file) << ":" << function << ":"
           << line << "] ";

    for(const auto& m : message)
        outs() << m;

    outs() << "\n";
    outs().resetColor();
    outs().flush();
}

