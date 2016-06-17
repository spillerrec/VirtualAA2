/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef PP_ARCHIVE_HPP
#define PP_ARCHIVE_HPP

#include "../utils/Buffer.hpp"

#include <vector>

class File;

namespace PP{
class HeaderDecrypter;

struct SubFile{
	Buffer filename;
	uint32_t size, offset;
	Buffer metadata;
	
	SubFile( Buffer filename, uint32_t size, uint32_t offset, Buffer metadata )
		:	filename(std::move(filename)), size(size), offset(offset), metadata(std::move(metadata)) {}
	
	SubFile( File& file, HeaderDecrypter& decrypter );
	Buffer getFile( File& file );
};

}

class PPArchive{
	public:
		static const uint8_t magic[];
		std::vector<PP::SubFile> files;
		
	public:
		PPArchive( File& file );
};


#endif
