/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Config/Config.h"
#include "findsymbol/Demangle/Demangle.h"
#include "findsymbol/Driver/Driver.h"
#include "findsymbol/Driver/Configurator.h"
#include "findsymbol/Support/Error.h"
#include "findsymbol/Support/Warning.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/StringMap.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/Format.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Support/raw_os_ostream.h"
#include <chrono>
#include <cstdio>
#include <string>

using namespace findsymbol;
using namespace llvm;

namespace
{

// symbol name
cl::list<std::string>
    SymbolList(cl::Positional, cl::desc("<symbol-names>"), cl::ZeroOrMore);

// --show-stats, -s
cl::opt<bool> Statistics("show-stats", cl::desc("Show statistics summary"));
cl::alias StatisticsA("s",
                      cl::desc("Alias for '--show-stats'"),
                      cl::aliasopt(Statistics));

// --demangle, -d
cl::opt<bool> Demangle("demangle",
                       cl::desc("Demangle C++ mangled names"),
                       cl::init(false));
cl::alias
    DemangleA("d", cl::desc("Alias for '--demangle'"), cl::aliasopt(Demangle));

// --libs
cl::list<std::string> Libraries("libs",
                                cl::desc("Only search specified libaries"),
                                cl::value_desc("libs"),
                                cl::CommaSeparated);

// --dirs
cl::list<std::string> Directories("dirs",
                                  cl::desc("Only search specified directories"),
                                  cl::value_desc("dirs"),
                                  cl::CommaSeparated);

// --add-dirs, -L
cl::list<std::string>
    AddDirectories("add-dirs",
                   cl::desc("Add specified directories to the searched ones"),
                   cl::value_desc("dirs"),
                   cl::CommaSeparated);
cl::alias AddDirectoriesA("L",
                          cl::desc("Alias for '--add-dirs'"),
                          cl::aliasopt(AddDirectories));

// --depth
cl::opt<int> RecursionDepth("depth",
                            cl::desc("Set recursion depth (default: 20)"),
                            cl::value_desc("D"),
                            cl::init(20));

// --config
cl::opt<std::string> Config("config",
                            cl::desc("Path to the config file"),
                            cl::value_desc("path"));

// --config-add
cl::list<std::string>
    ConfigAdd("config-add",
              cl::desc("Add specified directories to the config file"),
              cl::value_desc("dirs"),
              cl::CommaSeparated);

// --config-remove
cl::list<std::string>
    ConfigRemove("config-remove",
                 cl::desc("Remove specified directories from the config file"),
                 cl::value_desc("dirs"),
                 cl::CommaSeparated);

// --config-list
cl::opt<bool> ConfigList("config-list",
                         cl::desc("List all directories in the config file"),
                         cl::init(false));
}

/// Specify --version of this program
static void printVersion()
{
    outs() << "findsymbol version " FINDSYMBOL_VERSION_STRING
              " (based on LLVM " LLVM_VERSION_STRING ")\n";
}

/// Could it be a demangled symbol?
static void checkForDemangledSymbols(const std::string& symbol)
{
    if(!Demangle && (symbol.find("(") != std::string::npos
                     || symbol.find(")") != std::string::npos
                     || symbol.find(" ") != std::string::npos))
        warning("This looks like a demangled C++ symbol. Consider rerunning "
                "with '-demangle'");
}

int main(int argc, char* argv[])
{
    // Print a stack trace if we signal out
    sys::PrintStackTraceOnErrorSignal();
    PrettyStackTraceProgram trace(argc, argv);

    // Call llvm_shutdown() on exit.
    llvm_shutdown_obj shutdown;

    // Parse CommandLine options
    StringMap<cl::Option*> map;
    cl::getRegisteredOptions(map);

    for(auto& m : map)
        m.getValue()->setHiddenFlag(cl::Hidden);

    // Reenable desired options

    map["help"]->setHiddenFlag(cl::NotHidden);
    map["help"]->setDescription("Display this information");
    map["version"]->setHiddenFlag(cl::NotHidden);
    map["version"]->setDescription("Display this program's version number");
    map["show-stats"]->setHiddenFlag(cl::NotHidden);
    map["demangle"]->setHiddenFlag(cl::NotHidden);
    map["libs"]->setHiddenFlag(cl::NotHidden);
    map["dirs"]->setHiddenFlag(cl::NotHidden);
    map["config"]->setHiddenFlag(cl::NotHidden);
    map["config-add"]->setHiddenFlag(cl::NotHidden);
    map["config-remove"]->setHiddenFlag(cl::NotHidden);
    map["config-list"]->setHiddenFlag(cl::NotHidden);
    map["depth"]->setHiddenFlag(cl::NotHidden);
    map["add-dirs"]->setHiddenFlag(cl::NotHidden);

    cl::SetVersionPrinter(printVersion);
    cl::ParseCommandLineOptions(argc, argv);

    // Options for this run
    std::shared_ptr<Options> options(new Options);
    options->demangle = Demangle;
    options->recursive = Libraries.empty();
    options->gatherStatistics = Statistics;
    options->recursionDepth = RecursionDepth;

    // Statistics for this run
    std::shared_ptr<Stats> stats(new Stats);

    // Setup configurator
    Configurator config(Config);

    bool exitEarly = false;
    if((exitEarly |= ConfigList))
        config.listConfigDirectories();

    if((exitEarly |= !ConfigAdd.empty()))
        config.addConfigDirectories(ConfigAdd.begin(), ConfigAdd.end());

    if((exitEarly |= !ConfigRemove.empty()))
        config.removeConfigDirectories(ConfigRemove.begin(),
                                       ConfigRemove.end());

    if(exitEarly)
        return 0;

    if(SymbolList.empty())
        error("no symbol specified");

    auto tStart = std::chrono::high_resolution_clock::now();

    for(const auto& Symbol : SymbolList)
    {
        checkForDemangledSymbols(Symbol);

        std::vector<std::string> entryPoints;

        // Only look in the specified libraries
        if(!Libraries.empty())
        {
            entryPoints.insert(entryPoints.begin(), Libraries.begin(),
                               Libraries.end());
        }
        else
        {
            // Only look in the specified directories
            if(!Directories.empty())
                entryPoints.insert(entryPoints.begin(), Directories.begin(),
                                   Directories.end());
            // Use config file to determine directories
            else
            {
                entryPoints.insert(entryPoints.begin(), config.dirItBegin(),
                                   config.dirItEnd());

                if(!AddDirectories.empty())
                    entryPoints.insert(entryPoints.begin(),
                                       AddDirectories.begin(),
                                       AddDirectories.end());
            }
        }

        Driver driver(entryPoints, Symbol, options, stats);
        driver.printResult();
    }

    auto tEnd = std::chrono::high_resolution_clock::now();

    if(Statistics)
    {
        auto elapsedTime
            = std::chrono::duration<double, std::milli>(tEnd - tStart).count();
        stats->printStatisticSummary(elapsedTime);
    }

    return 0;
}

