/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "FakeDir.hpp"

using namespace std;

void FakeDir::combine( const FileObject& with ){
	for( unsigned i=0; i<with.children(); i++ ){
		auto object = findChild( *this, with[i].name() );
		if( object )
			object->combine( with[i] );
		else
			addChild( with[i].createMerger() );
	}
}

std::unique_ptr<AMergingObject> FakeDir::createMerger() const{
	auto out = std::make_unique<FakeDir>( filename );
	out->objects.reserve( objects.size() );
	for( auto& object : objects )
		out->objects.push_back( object->createMerger() );
	return out;
}

