/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPArchive.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/File.hpp"
#include "../utils/debug.hpp"

#include <cstdio>

const uint8_t magic[] = {0x5B, 0x50, 0x50, 0x56, 0x45, 0x52, 0x5D, 0x00}; // "[PPVER]\0"

const uint8_t offset[] = {0x3A, 0xE3, 0x87, 0xC2, 0xBD, 0x1E, 0xA6, 0xFE};
class HeaderDecrypter{
	private:
		uint8_t mask[8] = {0xFA, 0x49, 0x7B, 0x1C, 0xF9, 0x4D, 0x83, 0x0A};
		uint32_t index = { 0 };
	
	public:
		void decrypt( Buffer& buffer ){
			for( auto& val : buffer ){
				auto var28 = index % 8;
				mask[var28] += offset[var28];
				val ^= mask[var28];
				index++;
			}
		}
};

unsigned read32u( File& file, HeaderDecrypter& decrypter ){
	auto buf = file.read( 4 );
	decrypter.decrypt( buf );
	return ByteViewReader( buf ).read32u();
}

const uint8_t mask[] = {
	0x4D, 0x2D, 0xBF, 0x6A, 0x5B, 0x4A, 0xCE, 0x9D,
	0xF4, 0xA5, 0x16, 0x87, 0x92, 0x9B, 0x13, 0x03,
	0x8F, 0x92, 0x3C, 0xF0, 0x98, 0x81, 0xDB, 0x8E,
	0x5F, 0xB4, 0x1D, 0x2B, 0x90, 0xC9, 0x65, 0x00
};

class FileDecrypter{
	private:
		uint32_t index = { 0 };
	
	public:
		void decrypt( Buffer& buffer ){
			for( auto& val : buffer ){
				val ^= mask[index % 32];
				index++;
			}
		}
};

SubFile::SubFile( File& file, HeaderDecrypter& decrypter ){
	filename = file.read( 260 );
	decrypter.decrypt( filename );
	size   = read32u( file, decrypter );
	offset = read32u( file, decrypter );
	
	metadata = file.read( 20 );
	decrypter.decrypt( metadata );
}
	
Buffer SubFile::getFile( File& file ){
	file.seek( offset, 0 );
	auto data = file.read( size );
	FileDecrypter().decrypt( data );
	return data;
}

PPArchive::PPArchive( File& file ){
	auto header = file.read( 8 );
	//TODO: Check magic
	
	auto temp = HeaderDecrypter();
	auto version = read32u( file, temp );
	
	HeaderDecrypter decrypter;
	auto unknown1 = file.read( 1 );
	//decrypter.decrypt( unknown1 );
	
	auto file_amount = read32u( file, decrypter );
	decrypter = HeaderDecrypter();
	//printf( "subfiles: %d\n", file_amount );
	
	//Read all subfile headers
	files.reserve( file_amount );
	for( unsigned i=0; i<file_amount; i++ )
		files.emplace_back( file, decrypter );
	
	/*
	for( auto& file : files ){	
		printf( "%s\n", file.filename.data() );
		printf( "data: %d - %d\n", file.size, file.offset );
	}*/
}


