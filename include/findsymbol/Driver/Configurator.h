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
#ifndef FINDSYMBOL_CONFIGURATOR_H
#define FINDSYMBOL_CONFIGURATOR_H

#include "findsymbol/Config/Config.h"
#include "pugixml.hpp"
#include <vector>
#include <string>

namespace findsymbol
{

class Configurator
{
public:
    typedef std::vector<std::string>::iterator Iterator;

private:
    /// Initial starting directories
    std::vector<std::string> dirs_;

    /// Name of the configuration file
    std::string configFileName_;

    /// State of the configuration file
    bool configFileIsValid_;
    
    /// XML Document handle
    pugi::xml_document xmlDoc_;
    
    /// XML Parsing handle
    pugi::xml_parse_result xmlParseResult_;
    
    /// Do we need to save the XML document?
    bool saveXMLDoc_;

public:
    /// Validate the configuration file and parse it. If no configuration file
    /// is provided, or the file is invalid, a default set of directories,
    /// consisting of '/usr/lib' and '/usr/local/lib' is being used.
    Configurator(const std::string& configFile);
    
    /// Save the XML configuration file if needed
    ~Configurator();

    /// Get an iterator to the beginning of the directories
    Iterator dirItBegin() { return dirs_.begin(); }

    /// Get an iterator to the end of the directories
    Iterator dirItEnd() { return dirs_.end(); }

    /// Check if config file is valid
    bool configFileIsValid() const { return configFileIsValid_; }

    /// If the config file is valid list all directories in the file otherwise
    /// issue an error
    void listConfigDirectories() const;

    /// Add specified directories to the config file
    void addConfigDirectories(Iterator begin, Iterator end);

    /// Remove specified directories from the config file
    void removeConfigDirectories(Iterator begin, Iterator end);

private:
    /// Check if the configuration file is either provide as an argument or try
    /// to open $HOME/.findsymbol/config.xml.
    bool validateConfigFile(const std::string& configFile);

    /// Remove the invalid directories
    void removeInvalidDirectories();

    /// Use directories '/usr/lib' and '/usr/local/lib'
    void useDefaultDirectories();

    /// Use directories parsed from XML document
    bool useXMLDirectories();
};

} // end namespace findsymbol

#endif

