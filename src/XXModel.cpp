/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "XXModel.hpp"

#include "BufferReader.hpp"
#include "debug.hpp"

#include <iostream>

XXModel::XXModel( Buffer file ) : data(std::move(file)) {
	BufferReader reader( data );
	format = reader.read32u();
	require( format <= 8 );
	//TODO: validate properly
	
	unknown1 = reader.read8u();
	unknown2 = reader.read( format >= 1 ? 21 : 16 );
	frame = XXFrame( reader, format );
	
	unknown3 = reader.read( 4 );
	auto count = reader.read8u();
	std::cout << "Material count: " << (int)count << std::endl;
}

XXFrame::XXFrame( BufferReader& reader, int format ){
	name = reader.readName();
	std::cout << "Name: ";
	for( unsigned i=0; i<name.size(); i++ )
		std::cout << name[i];
	std::cout << std::endl;
	
	auto children_count = reader.read32u();
	header1 = reader.read( 4*4*4 + (format >= 7 ? 32 : 16) );
	auto submeshes_count = reader.read32u();
	header2 = reader.read( 3*4 * 2 + (format >= 7 ? 64 : 16 ) );
	
	std::cout << "Position: " << reader.tell() << std::endl;
	if( format >= 6 ){
		auto extra_name = reader.readName();
	std::cout << "Extra: ";
	for( unsigned i=0; i<extra_name.size(); i++ )
		std::cout << extra_name[i];
	std::cout << std::endl;
	//TODO: append to buffer
	}
	
	std::cout << "Children: " << children_count << " - submeshes: " << submeshes_count << std::endl;
	std::cout << "Position: " << reader.tell() << std::endl;
	
	children.reserve( children_count );
	for( unsigned i=0; i<children_count; i++ )
		children.emplace_back( reader, format );
//	XXFrame( reader, format );
}

