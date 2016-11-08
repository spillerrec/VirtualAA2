/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "BulkArchiveWriter.hpp"

#include "../resources/Deduper.hpp"

BulkEntry BulkArchiveWriter::write( ConstByteView data ){
/*	auto crc32 = Deduper::calculateHash( data );
	for( auto entry : entries )
		if( entry.getChecksum() == crc32 )
			return entry;//TODO: check data
	*/
	return BulkEntry::createHeader( file, data, BulkCompression::LZMA );
}
