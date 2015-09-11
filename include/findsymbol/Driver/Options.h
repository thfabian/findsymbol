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
#ifndef FINDSYMBOL_OPTIONS_H
#define FINDSYMBOL_OPTIONS_H

namespace findsymbol
{

struct Options
{
    /// Demangle all C++ mangled functions
    bool demangle;

    /// Gather statics of this run (number of parsed symbols etc.)
    bool gatherStatistics;

    /// Traverse the file system recursively (implies recurision depth == 1)
    bool recursive;

    /// Recursion depth (this prevents us from being trapped in cyclic
    /// dependencies)
    int recursionDepth;
};

} // end namespace findsymbol

#endif

