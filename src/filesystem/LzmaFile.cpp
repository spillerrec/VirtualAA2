/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "LzmaFile.hpp"
#include "BufferHandle.hpp"
#include "../compressors/lzma.hpp"


class LzmaFileHandle : public BufferReadHandle{
	public:
		LzmaFileHandle( ConstByteView compressed, uint64_t filesize ) : BufferReadHandle( filesize )
			{ lzma::decompress( compressed, data.view() ); }
};

std::unique_ptr<FileHandle> LzmaFile::openRead() const
	{ return std::make_unique<LzmaFileHandle>( compressedData().constView(), filesize() ); }

bool LzmaFile::compressFile( std::wstring filepath, ConstByteView data ){
	Buffer out( lzma::maxSize( data.size() ) );
	return writeCompressed( filepath.c_str(), data.size(), lzma::compress( data, out.view() ) );
}