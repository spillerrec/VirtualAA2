/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "AMergingObject.hpp"

#include <iostream>

using namespace std;

AMergingObject& AMergingObjectWithChildren::addChild( unique_ptr<AMergingObject> child ){
	objects.emplace_back( std::move(child) );
	return *objects.back();
}

void AMergingObject::combine( const FileObject& with ) {
	std::wcerr << "Attempted to combine: " << with.name().toBasicString() << "\n";
	throw std::runtime_error( "This file object does not support combining!" );
}

