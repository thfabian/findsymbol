/*******************************************************************- C++ -****\
 *
 *                          FindSymbol v1.0
 *                     (c) 2015 Fabian Thüring
 *
 * This file is distributed under the MIT Open Source License. See
 * LICENSE.TXT for details.
 *
\******************************************************************************/

#include "findsymbol/Demangle/Demangle.h"
#include "libiberty.h"
#include "demangle.h"

std::unique_ptr<char> findsymbol::demangleSymbol(const char* mangled)
{
    unsigned int skipFirst = 0;
    if(mangled[0] == '.' || mangled[0] == '$')
        ++skipFirst;

    return std::unique_ptr<char>(
        cplus_demangle(mangled + skipFirst, DMGL_PARAMS | DMGL_TYPES));
}
