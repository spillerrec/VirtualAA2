/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef BULK_ARCHIVE_WRITER_HPP
#define BULK_ARCHIVE_WRITER_HPP

#include "../decoders/BulkArchive.hpp"
#include "../utils/File.hpp"

#include <vector>


class BulkArchiveWriter{
	private:
		std::vector<BulkEntry> entries;
		File file;
	
	public:
		BulkArchiveWriter( const wchar_t* filepath )
			:	file( filepath, L"wb+" ) { }
		
		BulkEntry write( ConstByteView data );
};

#endif
