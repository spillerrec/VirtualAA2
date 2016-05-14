/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FileObject.hpp"

#include <algorithm>
#include <stdexcept>
#include <iostream>

//TODO: throw
std::unique_ptr<FileHandle> FileObject::openRead()   const { return { nullptr }; }
std::unique_ptr<FileHandle> FileObject::openWrite()  const { return { nullptr }; }
std::unique_ptr<FileHandle> FileObject::openAppend() const { return { nullptr }; }

const FileObject& FileObject::operator[]( int index ) const
	{ throw std::domain_error( "No children defined!" ); }

void FileObject::combine( const FileObject& with ) {
	std::wcerr << "Attempted to combine: " << with.name().toBasicString() << "\n";
	throw std::runtime_error( "This file object does not support combining!" );
}

FileObject& FileObject::operator[]( int index )
	{ return const_cast<FileObject&>( const_cast<const FileObject&>( *this )[index] ); }


static bool compareInsensitive( WStringView a, WStringView b ){
	if( a.size() != b.size() )
		return false;
	return std::equal( a.begin(), a.end(), b.begin()
		,	[]( auto a, auto b ){ return towlower(a) == towlower(b); }
		);
}

FileObject* FileObject::find( WStringView child_name ){
	//Note, we can't de-duplicate this because operator[] is virtual
	//What we should do instead is make an iterator, so we can simplify both
	for( unsigned j=0; j<children(); j++ )
		if( compareInsensitive( (*this)[j].name(), child_name ) )
			return &(*this)[j];
	return nullptr;
}

const FileObject* FileObject::find( WStringView child_name ) const{
	for( unsigned j=0; j<children(); j++ )
		if( compareInsensitive( (*this)[j].name(), child_name ) )
			return &(*this)[j];
	return nullptr;
}
