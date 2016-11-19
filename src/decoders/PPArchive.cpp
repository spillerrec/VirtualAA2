/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPArchive.hpp"
#include "PPCrypto.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/File.hpp"
#include "../utils/debug.hpp"

#include <cstdio>

using namespace PP;

const uint8_t Header::magic[] = {0x5B, 0x50, 0x50, 0x56, 0x45, 0x52, 0x5D, 0x00}; // "[PPVER]\0"

static unsigned read32u( File& file, HeaderDecrypter& decrypter ){
	auto buf = file.read( 4 );
	decrypter.decrypt( buf.view() );
	return ByteViewReader( buf ).read32u();
}

/*
	PP File:
	
		[HEADER]
			8 bytes - string  - Magic
			4 bytes - uint32  - version
			1 byte  - unknown - ??
			4 bytes - uint32  - file count
		Total: 17 bytes
		
		[SUBFILE] repeated 'file count' times
			260 bytes - string  - file name
			  4 bytes - uint32  - data size
			  4 bytes - uint32  - data offset
			 20 bytes - unknown - metadata
		Total: 288 bytes
		
		4 bytes - uint32 - size of the header + subfiles. Also those 4 bytes?
			 
		[DATA]
			at specified offsets in [SUBFILE]'s
*/

Header::Header( const wchar_t* path ){
	File f( path, L"rb" );
	*this = Header( f );
}

Header::Header( File& file ){
	//Check header
	file.seek( 0, 0 );
	auto magic_bytes = file.read( 8 );
	always( magic_bytes.view() == ConstByteView( magic, magic_length ), "Wrong magic bytes for PP file" );
	
	//Seek to file count, and read decode it
	file.seek( 8 + 4 + 1, 0 );
	HeaderDecrypter decrypter;
	file_count = read32u( file, decrypter );
	
	//Read entire header
	file.seek( 0, 0 );
	header = file.read( 17 + 288*file_count + 4 );
	
	//Decrypt everything
	decrypter = {};
	decrypter.decrypt( header.view().subView( 8, 4 ) );
	decrypter = {};
	decrypter.decrypt( header.view().subView( 13, 4 ) );
	decrypter = {};
	decrypter.decrypt( header.view().subView( 17, 288*file_count ) );
	decrypter = {};
	decrypter.decrypt( header.view().right( 4 ) );
	
	//TODO: assert last bit?
}

Header::SubFile Header::operator[]( uint32_t index ) const
	{ return { header.constView(), 17 + 288*index }; }

Header::SubFile Header::begin() { return (*this)[     0    ]; }
Header::SubFile Header::end()   { return (*this)[file_count]; }

Header::SubFile::SubFile( ConstByteView data, uint32_t offset )
	:	file( data.subView( offset, 288 ) ) { }

Header::SubFile& Header::SubFile::operator++(){
	file = { file.end(), 288 };
	return *this;
}


ConstByteView Header::SubFile::filename() const { return file.subView(   0, 260 ); }
ConstByteView Header::SubFile::metadata() const { return file.subView( 268,  20 ); }
//TODO: Fix to avoid const cast
uint32_t Header::SubFile::size()     const
	{ return ByteViewReader( ByteView(const_cast<uint8_t*>(file.subView( 260, 4 ).begin()), 4) ).read32u(); }
uint32_t Header::SubFile::offset()   const
	{ return ByteViewReader( ByteView(const_cast<uint8_t*>(file.subView( 264, 4 ).begin()), 4) ).read32u(); }


Buffer PP::readFile( File& file, uint32_t offset, uint32_t size ){
	file.seek( offset, 0 );
	auto data = file.read( size );
	FileDecrypter().decrypt( data.view() );
	return data;
}

