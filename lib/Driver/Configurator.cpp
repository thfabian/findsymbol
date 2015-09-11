/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Driver/Configurator.h"
#include "findsymbol/Support/Error.h"
#include "findsymbol/Support/Warning.h"
#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/FileSystem.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/Process.h"
#include "llvm/Support/Program.h"
#include "llvm/Support/raw_ostream.h"
#include <cstring>

using namespace llvm;
using namespace findsymbol;

bool Configurator::validateConfigFile(const std::string& configFile)
{
    // Parse config file
    configFileName_ = configFile;
    if(configFileName_.empty())
    {
        std::string HomeEnvName("HOME");
        auto HomeEnvVal = sys::Process::GetEnv(HomeEnvName);

        if(!HomeEnvVal.getValue().empty())
        {
            SmallString<256> HomeDir(HomeEnvVal.getValue());
            sys::path::append(HomeDir, ".findsymbol/config.xml");
            configFileName_ = HomeDir.str();
            return sys::fs::exists(configFileName_);
        }
        else
            return false;
    }
    else
    {
        if(!sys::fs::exists(configFileName_))
            error({"No such file or directory: '", configFileName_, "'"});
        else
            return true;
    }
}

Configurator::Configurator(const std::string& configFile) : saveXMLDoc_(false)
{
    configFileIsValid_ = validateConfigFile(configFile);
    bool dirsIsValid = false;

    if(configFileIsValid_)
    {
        xmlParseResult_ = xmlDoc_.load_file(configFileName_.c_str());
        dirsIsValid = useXMLDirectories();
    }

    if(!dirsIsValid || dirs_.empty())
        useDefaultDirectories();

    removeInvalidDirectories();
}

Configurator::~Configurator()
{
    if(configFileIsValid_ && saveXMLDoc_)
    {
        if(!xmlDoc_.save_file(configFileName_.c_str()))
            error("Failed to save configuration file");
        else
            outs() << "Successfully updated '" << configFileName_ << "'\n";
    }
}

void Configurator::useDefaultDirectories()
{
    dirs_.push_back("/usr/lib");
    dirs_.push_back("/usr/local/lib");
}

static inline bool checkXMLParsingError(const pugi::xml_parse_result& result,
                                        const std::string& file)
{
    bool hadError = false;
    if((hadError = !result))
    {
        std::string errorString;
        raw_string_ostream errorStream(errorString);
        errorStream << file << ": xml parsing error: '" << result.description()
                    << "'";
        errorStream.flush();
        warning(errorString);
    }
    return hadError;
}

static pugi::xml_node exists(const pugi::xml_document& doc,
                             const std::string& path)
{
    pugi::xml_node dirs = doc.child("directories");
    for(pugi::xml_node dir = dirs.first_child(); dir; dir = dir.next_sibling())
    {
        if(std::strcmp(dir.child_value(), path.c_str()) == 0)
            return dir;
    }
    return pugi::xml_node();
}

bool Configurator::useXMLDirectories()
{
    bool hadError = false;

    pugi::xml_node dirs = xmlDoc_.child("directories");
    for(pugi::xml_node dir = dirs.first_child(); dir && !hadError;
        dir = dir.next_sibling())
    {
        std::string dirString(dir.child_value());
        hadError = checkXMLParsingError(xmlParseResult_, configFileName_);
        dirs_.push_back(dirString);
    }

    if((hadError |= dirs_.empty()))
        dirs_.clear();

    return !hadError;
}

void Configurator::removeInvalidDirectories()
{
    for(auto it = dirs_.begin(); it != dirs_.end();)
    {
        if(sys::fs::exists(*it))
            ++it;
        else
        {
            warning({"No such directory: '", *it, "'"});
            it = dirs_.erase(it);
        }
    }
}

void Configurator::listConfigDirectories() const
{
    if(!configFileIsValid_ || !xmlParseResult_)
        error((configFileName_.empty() ? "No configuration file found"
                                       : "Configuration file is invalid"));

    outs() << "Directories in: '" << configFileName_ << "'\n";
    bool hadError = false;

    pugi::xml_node dirs = xmlDoc_.child("directories");
    for(pugi::xml_node dir = dirs.first_child(); dir && !hadError;
        dir = dir.next_sibling())
    {
        std::string dirString(dir.child_value());
        hadError = checkXMLParsingError(xmlParseResult_, configFileName_);
        outs() << " -- " << dirString << "\n";
    }
}

void Configurator::addConfigDirectories(Configurator::Iterator begin,
                                        Configurator::Iterator end)
{
    if(!configFileIsValid_ || !xmlParseResult_)
        error((configFileName_.empty() ? "No configuration file found"
                                       : "Configuration file is invalid"));

    for(auto it = begin; it != end; ++it)
    {
        if(!sys::fs::exists(*it))
            error({"No such file or directory: '", *it, "'"});

        if(!exists(xmlDoc_, *it))
        {
            pugi::xml_node dirs = xmlDoc_.child("directories");
            pugi::xml_node value = dirs.append_child("dir");
            value.append_child(pugi::node_pcdata).set_value(it->c_str());
            saveXMLDoc_ = true;
        }
        else
            warning({"Directory '", *it, "' already exists"});
    }
}

void Configurator::removeConfigDirectories(Configurator::Iterator begin,
                                           Configurator::Iterator end)
{
    if(!configFileIsValid_ || !xmlParseResult_)
        error((configFileName_.empty() ? "No configuration file found"
                                       : "Configuration file is invalid"));

    pugi::xml_node removeNode; 
    for(auto it = begin; it != end; ++it)
    {
        if(!sys::fs::exists(*it))
            error({"No such file or directory: '", *it, "'"});

        if((removeNode = exists(xmlDoc_, *it)))
        {
            pugi::xml_node dirs = xmlDoc_.child("directories");
            dirs.remove_child(removeNode);
            saveXMLDoc_ = true;
        }
        else
            warning({"Directory '", *it, "' does not exist"});
    }
}

