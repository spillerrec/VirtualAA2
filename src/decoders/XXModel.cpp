/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "XXModel.hpp"

#include "../utils/ByteViewReader.hpp"
#include "../utils/debug.hpp"

#include <iostream>

using namespace XX;

Vector3::Vector3( ByteViewReader reader ){
	x = reader.readFloat();
	y = reader.readFloat();
	z = reader.readFloat();
}

Vector3::Vector3( ByteView data ){
	//TODO: validate size
	ByteViewReader reader( data );
	*this = Vector3{ reader };
}

Model::Model( Buffer file ) : data(std::move(file)) {
	ByteViewReader reader( data );
	
//Read header
	auto format = reader.read32u();
	require( format <= 8 );
	//TODO: validate properly
	
	/*auto unknown1 =*/ reader.read8u();
	/*auto unknown2 =*/ reader.read( format >= 1 ? 21 : 16 );
	
	auto header_end = reader.tell();
	data_header = reader.makeView( 0, header_end );
	
//Read frames
	frame = Frame( reader, format );
	
//Read unknown
	unknown3 = reader.read( 4 );
	
//Read materials
	auto material_count = reader.read32u();
	materials.reserve( material_count );
	for( unsigned i=0; i<material_count; i++ )
		materials.emplace_back( reader );
	
//Read textures
	auto texture_count = reader.read32u();
	for( unsigned i=0; i<texture_count; i++ )
		textures.emplace_back( reader );
	
//Read unknown
	if( format >= 2 )
		unknown4 = reader.read( 10 );
	
	require( reader.left() == 0 );
	
//Statistics
//	std::cout << "Header section:   " << data_header.size() << std::endl;
//	std::cout << "Frame section:    " << data_frames.size() << std::endl;
//		std::cout << "\tMeshes take: " << mesh_size << std::endl;
//		std::cout << "\tBones  take: " << bone_size << std::endl;
//		std::cout << "\tDupes  take: " << dupe_size << std::endl;
//	std::cout << "Material section: " << sumXXSize( materials ) << std::endl;
//	std::cout << "Textures section: " << sumXXSize( textures ) << std::endl;
}

static void coutName( const char* prefix, NotByteView view ){
/*	std::cout << prefix;
	for( unsigned i=0; i<view.size(); i++ )
		std::cout << view[i];
	std::cout << std::endl;*/
}

Bone::Bone( ByteViewReader& reader ){
	name = reader.readName();
	index = reader.read32u();
	matrix = reader.read( 4*4*4 );
}

Frame::Frame( ByteViewReader& reader, int format ){
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
		auto dupe_amount = reader.read16u(); //TODO:
		reader.read( 4+4 ); //TODO:
		auto vertex_size = (format >= 4 ? 22 : 4) + 12*4;
		vertice_dupes = reader.read( dupe_amount * vertex_size );
		
	//Debug total count of vertices in sub-meshes vs. dupes
		auto vertex_count = std::accumulate( meshes.begin(), meshes.end(), 0u
				, [](unsigned acc, auto mesh){ return acc + mesh.vertex_count; }
				);
		if( vertex_count != dupe_amount )
			std::cout << "vertices: " << vertex_count << " - " << dupe_amount << "\n";
		
		/*Compare meshes with dupes
		ByteViewReader dupe_reader( vertice_dupes );
		for( auto& mesh : meshes ){
			auto dupes = dupe_reader.read( vertex_size * mesh.vertex_count );
			bool match = dupes == mesh.vertexes;
			std::cout << "mess matches: " << (match ? "yes" : "no") << "\n";
			
			if( !match ){
				auto count = std::min( dupes.size(), mesh.vertexes.size() );
				unsigned differences = 0;
				for( unsigned i=0; i<count; i++ )
					differences += mesh.vertexes[i] != dupes[i] ? 1 : 0;
				std::cout << "How different: " << differences / (double)count << "\n";
			}
		}
		//*/
		
		//bone list
		auto bone_count = reader.read32u();
		bones.reserve( bone_count );
		for( int i=0; i<bone_count; i++ )
			bones.emplace_back( reader );
	}
	
	children.reserve( children_count );
	for( unsigned i=0; i<children_count; i++ )
		children.emplace_back( reader, format );
}

Mesh::Mesh( ByteViewReader& reader, int format, int vector2count ){
	unknown1 = reader.read( format >= 7 ? 64 : 16 );
	index = reader.read32u();
	
	//Read faces
	faces_count = reader.read32u();
	faces = reader.read( faces_count * 2 );
	//Note: count is only a third, i.e. 6 bytes per face?
	
	//Read vertices
	vertex_count = reader.read32u();
	auto vertex_size = (format >= 4 ? 22 : 4) + 12*4;
	vertexes = reader.read( vertex_size * vertex_count );
	
	//Some stuff I don't remember that didn't seem relevant to model, due to its complexity
	auto random_shit_size = (format>=7 ? 20 : 0) + vector2count*vertex_count*2*4
		+	(format>=2 ? 100 : 0 )
		+	(format>=3 ?  64 : 0 )
		+	(format>=5 ?  20 : 0 )
		+	(format>=6 ?  28 : 0 )
		+	(format>=7 ? 172 : 0 )
	;
	unknown2 = reader.read( random_shit_size );
	
	if( format >= 8 ){
		auto format2 = reader.read8u();
		auto name = reader.readName();
	//	coutName( "Mesh name: ", name );
		reader.read( 16 );
	}
}

Material::Material( ByteViewReader& reader ){
	name = reader.readName();
	coutName( "Material: ", name );
	colors = reader.read( 4*4 * 4 + 4 );
	for( int i=0; i<4; i++ ){
		TextureRef ref;
		ref.name = reader.readName();
		coutName( "Texture Ref: ", ref.name );
		ref.unknown = reader.read( 16 );
		textures.push_back( ref );
	}
	unknown = reader.read( 88 ); //TODO: format < 0 ?
}

Texture::Texture( ByteViewReader& reader ){
	name = reader.readName();
	coutName( "Texture: ", name );
	header = reader.read( 8*4 + 1 );
	
	auto length = reader.read32u();
	data = reader.read( length );
}

