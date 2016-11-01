/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "DeflateFile.hpp"
#include "BufferHandle.hpp"
#include "../compressors/deflate.hpp"


class DeflateFileHandle : public BufferReadHandle{
	public:
		DeflateFileHandle( ConstByteView compressed, uint64_t filesize ) : BufferReadHandle( filesize )
			{ DEFLATE::decompress( compressed, data.view() ); }
};

std::unique_ptr<FileHandle> DeflateFile::openRead() const
	{ return std::make_unique<DeflateFileHandle>( compressedData().constView(), filesize() ); }

bool DeflateFile::compressFile( std::wstring filepath, ConstByteView data ){
	Buffer out( DEFLATE::maxSize( data.size() ) );
	return writeCompressed( filepath.c_str(), data.size(), DEFLATE::compress2( data, out.view() ) );
}
