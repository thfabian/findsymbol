################################################################################
#
#                         FindSymbol - v1.0
#                     (c) 2015 Fabian Thüring         
#
# This file is distributed under the MIT Open Source License. See 
# LICENSE.TXT for details.
#
################################################################################

cmake_minimum_required(VERSION 2.8)

################################################################################
# Compilation
################################################################################
file(GLOB sources "*.cpp")
add_library(FSSupport ${sources})
