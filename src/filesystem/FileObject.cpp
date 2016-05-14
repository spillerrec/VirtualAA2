/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileObject.hpp"

#include <algorithm>
#include <stdexcept>

//TODO: throw
std::unique_ptr<FileHandle> FileObject::openRead()   const { return { nullptr }; }
std::unique_ptr<FileHandle> FileObject::openWrite()  const { return { nullptr }; }
std::unique_ptr<FileHandle> FileObject::openAppend() const { return { nullptr }; }

const FileObject& FileObject::operator[]( int index ) const
	{ throw std::domain_error( "No children defined!" ); }

void FileObject::combine( FileObject& with )
	{ throw std::runtime_error( "This file object does not support combining!" ); }


static bool compareInsensitive( WStringView a, WStringView b ){
	if( a.size() != b.size() )
		return false;
	return std::equal( a.begin(), a.end(), b.begin()
		,	[]( auto a, auto b ){ return towlower(a) == towlower(b); }
		);
}

const FileObject* FileObject::find( WStringView child_name ) const{
	for( unsigned j=0; j<children(); j++ )
		if( compareInsensitive( (*this)[j].name(), child_name ) )
			return &(*this)[j];
	return nullptr;
}
