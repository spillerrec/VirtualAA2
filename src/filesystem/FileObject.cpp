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

FileObject& FileObjectWithChildren::addChild( std::unique_ptr<FileObject> child ){
	objects.emplace_back( std::move(child) );
	return *objects.back();
}
