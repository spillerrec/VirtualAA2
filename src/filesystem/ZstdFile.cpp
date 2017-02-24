/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "ZstdFile.hpp"
#include "BufferHandle.hpp"
#include "../compressors/zstd.hpp"


class ZstdFileHandle : public BufferReadHandle{
	public:
		ZstdFileHandle( ConstByteView compressed, uint64_t filesize ) : BufferReadHandle( filesize )
			{ zstd::decompress( compressed, data.view() ); }
};

std::unique_ptr<FileHandle> ZstdFile::openRead() const
	{ return std::make_unique<ZstdFileHandle>( compressedData().constView(), filesize() ); }

bool ZstdFile::compressFile( std::wstring filepath, ConstByteView data ){
	Buffer out( zstd::maxSize( data.size() ) );
	return writeCompressed( filepath.c_str(), data.size(), zstd::compress( data, out.view() ) );
}