/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "XXModel.hpp"

#include "BufferReader.hpp"
#include "debug.hpp"

#include <iostream>

unsigned mesh_size = 0;
unsigned bone_size = 0;
unsigned dupe_size = 0;

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
		std::cout << reader.tell() << std::endl;
	std::cout << "Meshes take: " << mesh_size << std::endl;
	std::cout << "Bones  take: " << bone_size << std::endl;
	std::cout << "Dupes  take: " << dupe_size << std::endl;
}

static void coutName( const char* prefix, NotArrayView view ){
	std::cout << prefix;
	for( unsigned i=0; i<view.size(); i++ )
		std::cout << view[i];
	std::cout << std::endl;
}

XXFrame::XXFrame( BufferReader& reader, int format ){
	name = reader.readName();
	coutName( "Frame name: ", name );
	
	auto children_count = reader.read32u();
	header1 = reader.read( 4*4*4 + (format >= 7 ? 32 : 16) );
	auto submeshes_count = reader.read32u();
	header2 = reader.read( 3*4 * 2 + (format >= 7 ? 64 : 16 ) );
	
	if( format >= 6 ){
		auto extra_name = reader.readName();
	//	coutName( "Frame extra name: ", extra_name );
	//TODO: append to buffer
	}
	
	if( submeshes_count > 0 ){
		auto vector2count = reader.read8u();
		
		for( unsigned i=0; i<submeshes_count; i++ )
			meshes.emplace_back( reader, format, vector2count );
		
		//TODO: vertex dupe
		auto dupe_start = reader.tell();
		auto dupe_amount = reader.read16u();
		reader.read( 4+4 );
		auto vertex_size = (format >= 4 ? 22 : 4) + 12*4;
		reader.read( dupe_amount * vertex_size );
		dupe_size += reader.tell() - dupe_start;
		
		//TODO: bone list
		auto bone_start = reader.tell();
		auto bone_count = reader.read32u();
		for( int i=0; i<bone_count; i++ ){
			auto bone_name = reader.readName();
		//	coutName( "Bone: ", bone_name );
			reader.read32u(); //Index
			reader.read( 4*4*4 ); //Matrix
		}
		bone_size += reader.tell() - bone_start;
	}
	
	children.reserve( children_count );
	for( unsigned i=0; i<children_count; i++ )
		children.emplace_back( reader, format );
//	XXFrame( reader, format );
}

XXMesh::XXMesh( BufferReader& reader, int format, int vector2count ){
	auto start = reader.tell();
	unknown1 = reader.read( format >= 7 ? 64 : 16 );
	std::cout << "Material index: " << reader.read32u() << std::endl;
	
	auto faces_count = reader.read32u();
//	std::cout << "Faces: " << faces_count/3 << std::endl;
	faces = reader.read( faces_count * 2 );
	
	auto vertex_count = reader.read32u();
	auto vertex_size = (format >= 4 ? 22 : 4) + 12*4;
//	std::cout << "Vertexes: " << vertex_count << std::endl;
	vertexes = reader.read( vertex_size * vertex_count );
	
	auto random_shit_size = (format>=7 ? 20 : 0) + vector2count*vertex_count*2*4
		+	(format>=2 ? 100 : 0 )
		+	(format>=3 ?  64 : 0 )
		+	(format>=5 ?  20 : 0 )
		+	(format>=6 ?  28 : 0 )
		+	(format>=7 ? 172 : 0 )
	;
	reader.read( random_shit_size );
	
	if( format >= 8 ){
		auto format2 = reader.read8u();
		auto name = reader.readName();
	//	coutName( "Mesh name: ", name );
		reader.read( 16 );
	}
	
	auto end = reader.tell();
	
	std::cout << "Total mesh size: " << end-start << std::endl << std::endl;
	mesh_size += end-start;
}

