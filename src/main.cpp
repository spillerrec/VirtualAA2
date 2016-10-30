/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "encoders/PPCompactor.hpp"
#include "decoders/XXModel.hpp"
#include "filesystem-win.hpp"
#include "filesystem/FilePath.hpp"
#include "filesystem/FileSystem.hpp"
#include "filesystem/VirtualDataDir.hpp"
#include "filesystem/PPFile.hpp"
#include "resources/Deduper.hpp"
#include "utils/ByteViewReader.hpp"
#include "utils/File.hpp"
#include "utils/StringView.hpp"

#include <iostream>

static int error( const char* const msg, int code = -1 ){
	std::cerr << msg << std::endl;
	return code;
}

static void copy_file_object( const FileObject& object, const wchar_t* to ){
	auto handle = object.openRead();
	
	File out( to, L"wb" );
	
	const auto buffer_size = 256*4;
	Buffer read_buffer( buffer_size );
	
	uint64_t bytes_read = 0;
	uint64_t offset = 0;
	std::cout << "Starting to read\n";
	while( (bytes_read = handle->read( read_buffer.view(), offset )) > 0 ){
	//	std::cout << "read: " << bytes_read << std::endl;
		out.write( read_buffer.view().left( bytes_read ) );
		offset += bytes_read;
	}
}

static int copy_file( const wchar_t* data_dir_path, const wchar_t* filepath, const wchar_t* to ){
	VirtualDataDir dir( data_dir_path );
	auto file = dir.getFromPath( { filepath } );
	if( !file )
		return error( "File not found" );
	
	copy_file_object( *file, to );
	
	return 0;
}

static int compact_pp_test( const wchar_t* data_dir_path, const wchar_t* filepath, const wchar_t* dir, const wchar_t* name ){
	auto out_path = std::wstring(dir) + name;
	auto copy_err = copy_file( data_dir_path, filepath, out_path.c_str() );
	if( copy_err != 0 )
		return copy_err;
	
	PPCompactor::importPP( out_path );
	
	PPFile pp( std::wstring(dir) + L"[PP] " + name );
	
	copy_file_object( *pp.createMerger(), (out_path + L".virtualized").c_str() );
	
	return 0;
}

constexpr bool WRITE_MESH = false;
constexpr bool WRITE_DUPES = false;
constexpr bool WRITE_BONES = false;
static void write_frame_data( const std::wstring& folder, const XX::Frame& frame, int& frame_count, Deduper& dedupe_meshes ){
	auto base_name = folder + L"frame_" + std::to_wstring( frame_count ) + L"_";
	
	//Output mesh vertices
	int mesh_id = 0;
	for( auto& mesh : frame.meshes ){
		if( mesh.vertexes.size() > 0 ){
			if( WRITE_MESH ){
				auto out_path = base_name + L"mesh_" + std::to_wstring( mesh_id++ );
				File( out_path.c_str(), L"wb" ).write( mesh.vertexes );
			}
			dedupe_meshes.add( mesh.vertexes );
		}
	}
	
	//Output dupes
	if( WRITE_DUPES && frame.vertice_dupes.size() > 0 )
		File( (base_name + L"dupes").c_str(), L"wb" ).write( frame.vertice_dupes );
	
	if( WRITE_BONES )
		for( auto& bone : frame.bones ){
			File out( L"bones.csv", L"ab" );
			out.write( bone.name );
			out.write( makeView( ", " ) );
			out.write( makeView( std::to_string( bone.index ).c_str() ) );
			
			ByteViewReader reader( bone.matrix );
			for( int i=0; i<16; i++ ){
				out.write( makeView( ", " ) );
				out.write( makeView ( std::to_string( reader.readFloat() ).c_str() ) );
			}
			out.write( makeView( "\n" ) );
		}
	
	//Recurse into children
	for( auto& child : frame.children ){
		frame_count++;
		write_frame_data( folder, child, frame_count, dedupe_meshes );
	}
}

static int split_xx( const wchar_t* xx_dir ){
	Deduper dedupe_meshes;
	Deduper dedupe_textures;
	File text( L"textures.csv", L"wb" );
	
	auto base_folder = std::wstring( xx_dir );
	for( auto file : getFolderContents( base_folder ) ){
		auto xx_path = file.path( base_folder );
		auto filepath = FilePath( makeView( xx_path ) );
		auto filename = filepath.filename().left( filepath.filename().size()-2 ).toBasicString();
		auto folder = filepath.folderPath();
		makeFolder( folder, filename );
		auto out_folder = folder + L"\\" + filename + L"\\";
		
		XX::Model xx( File( xx_path.c_str(), L"rb" ).readAll() );
		
		for( auto& texture : xx.textures ){
			File out( (out_folder + fromJapPath(texture.name.toString())).c_str(), L"wb" );
			
			auto name = texture.name.toString();
			if( name.substr( name.size()-4 ) == std::string("bmp\0", 4) ){
				//TODO: fix \0 in texture name
				texture.data[0] = 0x42;
				texture.data[1] = 0x4D;
			}
			out.write( texture.data );
			
			auto hash = Deduper::calculateHash( texture.data );
			
			text.write( makeView( name.c_str() ) );
			ByteViewReader reader( texture.header );
			for( int i=0; i<8; i++ ){
				text.write( makeView( ", " ) );
				text.write( makeView( std::to_string( reader.read32u() ).c_str() ) );
			}
			text.write( makeView( ", " ) );
			text.write( makeView( std::to_string( reader.read8u() ).c_str() ) );
			
			text.write( makeView( ", " ) );
			text.write( makeView( std::to_string( hash ).c_str() ) );
			
			text.write( makeView( "\n" ) );
		}
		
		int frame_count = 1;
	//	write_frame_data( out_folder, xx.frame, frame_count, dedupe_meshes );
	}
	
	std::cout << "Total mesh size: " << dedupe_meshes.total_size() << "\n";
	std::cout << "Savings: "         << dedupe_meshes.savings()    << "\n";
	std::cout << "Meshes: "          << dedupe_meshes.addedCount() << "\n";
	std::cout << "Unique: "          << dedupe_meshes.savedCount() << "\n";
	
	makeFolder( base_folder, L"\\meshes" );
	int i=0;
	for( auto& resource : dedupe_meshes.resources() ){
		auto name = base_folder + L"\\meshes\\mesh_" + std::to_wstring( i++ ) + L" - files " + std::to_wstring( resource.sources.size() );
		File( name.c_str(), L"wb" ).write( resource.data );
	}
	
	return 0;
}

int wmain( int argc, wchar_t* argv[] ){
	if( argc < 2 )
		return error( "VirtualAA2 <path-to-aa2-data-dir> [mount-point]\n" );
	
	auto param1 = makeView( argv[1] );
	if( param1.startsWith( makeView( L"--" ) ) ){
		if( param1 == makeView( L"--copy" ) ){
			if( argc == 5 )
				return copy_file( argv[2], argv[3], argv[4] );
			else
				return error( "VirtualAA2 --copy <path-to-aa2-data-dir> <file-to-copy> <output-path>" );
		}
		else if( param1 == makeView( L"--compact-pp-test" ) ){
			if( argc == 6 )
				return compact_pp_test( argv[2], argv[3], argv[4], argv[5] );
			else
				return error( "VirtualAA2 --compact-pp-test <path-to-aa2-data-dir> <file-to-copy> <output-directory> <output-name>" );
		}
		else if( param1 == makeView( L"--split-xx" ) ){
			if( argc == 3 )
				return split_xx( argv[2] );
			else
				return error( "VirtualAA2 --split-xx <path-to-xx-file>" );
		}
		else
			return error( "Unknown parameter" );
	}
	else{
		auto data_dir_path = argv[1];
		auto mount_point = argc < 3 ? L"v" : argv[2];
		
		return startFilesystem( data_dir_path, mount_point );
	}
}


