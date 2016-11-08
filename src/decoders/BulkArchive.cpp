/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "BulkArchive.hpp"

#include "../compressors/deflate.hpp"
#include "../compressors/lz4.hpp"
#include "../compressors/lzma.hpp"

#include "../resources/Deduper.hpp"

#include "../utils/Buffer.hpp"
#include "../utils/ByteViewReader.hpp"
#include "../utils/File.hpp"

const uint8_t BULK_MAGIC[4] = { 'B', 'U', 'L', 'K' };


BulkEntry::BulkEntry( File& file ){
	auto start_pos = file.tell();
	auto header = file.read( 28 );
	//TODO: assert header size
	
	//Parse header
	ByteViewReader reader( header );
	auto magic  = reader.read( 4 ); //4 bytes, magic
	compressed  = reader.read32u(); //4 bytes, compressed size
	offset      = reader.read64u(); //8 bytes, offset of the data section (current pos - 8 bytes)
	checksum    = reader.read32u(); //4 bytes, crc32 checksum
	size        = reader.read32u(); //4 bytes, uncompressed size
	compression = reader.read8u();  //1 byte , type of compression: NULL, LZ4, DEFLATE, LZMA
	//                                3 bytes, reserved
	
	//Assert it is correct
	//TODO: check magic
	//TODO: check offset
}

uint64_t BulkEntry::nextHeader()  const { return dataSection() + compressed; }
uint64_t BulkEntry::dataSection() const { return offset + 28; }

Buffer BulkEntry::readData( File& file ) const{
	//Stop if NULL file
	if( compression == 0 )
		return {};
	
	//Read data, and return it if no compression
	file.seek( dataSection(), 0 );
	auto data = file.read( compressed );
	if( compression == 1 )
		return std::move( data );
	
	//Decompress
	Buffer out( size );
	switch( compression ){
		case 2:     lz4::decompress( data.constView(), out.view() );
		case 3: DEFLATE::decompress( data.constView(), out.view() );
		case 4:    lzma::decompress( data.constView(), out.view() );
		default: {};
	}
	return out;
}

static Buffer compressData( ConstByteView data, BulkCompression compression ){
	switch( compression ){
		case BulkCompression::EMPTY  : return Buffer( data.size() );
		case BulkCompression::LZ4    : return     lz4::compress(  data );
		case BulkCompression::DEFLATE: return DEFLATE::compress2( data );
		case BulkCompression::LZMA   : return    lzma::compress(  data );
		default: return {};
	}
}

#include <iostream>
BulkEntry BulkEntry::createHeader( File& output, ConstByteView data, BulkCompression compression ){
	auto out = compressData( data, compression );
	auto start_pos = output.tell();
	
	//Construct header
	BulkEntry entry;
	entry.compressed  = out.size();
	entry.offset      = start_pos;
	entry.checksum    = Deduper::calculateHash( data );
	entry.size        = data.size();
	entry.compression = uint8_t(compression);
	std::cout << "Size: " << entry.size / double( entry.compressed) << "\n";
	
	//Write header
	output.write( Buffer{ BULK_MAGIC, 4 } );
	output.write32u( entry.compressed  );
	output.write64u( entry.offset      );
	output.write32u( entry.checksum    );
	output.write32u( entry.size        );
	output.write8u(  entry.compression );
	output.write8u( 0 );
	output.write8u( 0 );
	output.write8u( 0 );
	
	//Write data
	output.write( out.view() );
	
	return entry;
}

