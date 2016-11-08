/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef BULK_ARCHIVE_HPP
#define BULK_ARCHIVE_HPP

#include "../utils/ArrayView.hpp"
#include <cstdint>

class File;
class Buffer;

enum class BulkCompression{
		EMPTY   = 0
	,	NONE    = 1
	,	LZ4     = 2
	,	DEFLATE = 3
	,	LZMA    = 4
};

class BulkEntry{
	private:
		uint64_t offset;
		uint32_t size;
		uint32_t compressed;
		uint32_t checksum;
		uint8_t compression;
		uint8_t state;
		BulkEntry() {}
		
	public:
		BulkEntry( File& file );
		
		uint64_t nextHeader() const;
		uint64_t dataSection() const;
		
		Buffer readData( File& file ) const;
		
		static BulkEntry createHeader( File& output, ConstByteView data, BulkCompression compression );
		
	public:
		auto getChecksum() const{ return checksum; }
		auto getSize()     const{ return size; }
};

#endif
