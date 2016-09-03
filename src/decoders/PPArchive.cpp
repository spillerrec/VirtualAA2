/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPArchive.hpp"
#include "PPCrypto.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/File.hpp"
#include "../utils/debug.hpp"

#include <cstdio>

const uint8_t PPArchive::magic[] = {0x5B, 0x50, 0x50, 0x56, 0x45, 0x52, 0x5D, 0x00}; // "[PPVER]\0"

using namespace PP;

unsigned read32u( File& file, HeaderDecrypter& decrypter ){
	auto buf = file.read( 4 );
	decrypter.decrypt( buf.view() );
	return ByteViewReader( buf ).read32u();
}

SubFile::SubFile( File& file, HeaderDecrypter& decrypter ){
	filename = file.read( 260 );
	decrypter.decrypt( filename.view() );
	size   = read32u( file, decrypter );
	offset = read32u( file, decrypter );
	
	metadata = file.read( 20 );
	decrypter.decrypt( metadata.view() );
}
	
Buffer SubFile::getFile( File& file ){
	file.seek( offset, 0 );
	auto data = file.read( size );
	FileDecrypter().decrypt( data.view() );
	return data;
}

PPArchive::PPArchive( File& file ){
	auto header = file.read( 8 );
	//TODO: Check magic
	
	auto temp = HeaderDecrypter();
	auto version = read32u( file, temp );
	
	HeaderDecrypter decrypter;
	auto unknown1 = file.read( 1 );
	//decrypter.decrypt( unknown1.view() );
	
	auto file_amount = read32u( file, decrypter );
	decrypter = HeaderDecrypter();
	//printf( "subfiles: %d\n", file_amount );
	
	//Read all subfile headers
	files.reserve( file_amount );
	for( unsigned i=0; i<file_amount; i++ )
		files.emplace_back( file, decrypter );
	
	temp = HeaderDecrypter();
	auto data_offset = read32u( file, temp );
	//Should be equal to the size of the header
	
	/*
	for( auto& file : files ){	
		printf( "%s\n", file.filename.data() );
		printf( "data: %d - %d\n", file.size, file.offset );
	}//*/
}


