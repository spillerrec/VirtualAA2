/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_SYSTEM_HPP
#define FILE_SYSTEM_HPP

#include <string>
#include <vector>

std::vector<std::wstring> getFolders( std::wstring path );
bool makeFolder( std::wstring path, std::wstring name );

#endif
