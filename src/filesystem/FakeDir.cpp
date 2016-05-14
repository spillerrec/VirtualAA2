/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FakeDir.hpp"
#include <iostream>

using namespace std;

FileObject& FakeDir::addChild( unique_ptr<FileObject> child ){
	objects.emplace_back( std::move(child) );
	return *objects.back();
}

void FakeDir::combine( const FileObject& with ){
	for( unsigned i=0; i<with.children(); i++ ){
		auto object = find( with[i].name() );
		if( object )
			object->combine( with[i] );
		else
			objects.push_back( with[i].copy() );
	}
}

std::unique_ptr<FileObject> FakeDir::copy() const{
	auto out = std::make_unique<FakeDir>( filename );
	out->objects.reserve( objects.size() );
	for( auto& object : objects )
		out->objects.push_back( object->copy() );
	return out;
}

const FileObject& FakeDir::operator[]( int index ) const {
	return *objects[index];
}

