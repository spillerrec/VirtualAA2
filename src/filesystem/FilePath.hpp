/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef FILE_PATH_HPP
#define FILE_PATH_HPP

#include "../utils/StringView.hpp"

struct FilePath{
	std::vector<WStringView> path;
	
	FilePath( WStringView filepath ) : path( split<const wchar_t>( filepath, L'\\' ) ) { }
	FilePath( const wchar_t* filepath ) : FilePath( makeView( filepath ) ) { }
	
	bool hasRoot() const{ return path.size() > 0 && path[0].size() == 0; }
	bool isRoot() const{ return hasRoot() && path.size() == 1; }
	
	auto filename() const{ return path.back(); }
	std::wstring folderPath() const{
		std::wstring out = path[0].toBasicString();
		for( unsigned i=1; i<path.size()-1; i++ )
			out += L"\\" + path[i].toBasicString();
		return out;
	}
};

#endif
