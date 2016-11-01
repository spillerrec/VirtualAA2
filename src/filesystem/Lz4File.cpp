/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "Lz4File.hpp"
#include "BufferHandle.hpp"
#include "../compressors/lz4.hpp"


class Lz4FileHandle : public BufferReadHandle{
	public:
		Lz4FileHandle( ConstByteView compressed, uint64_t filesize ) : BufferReadHandle( filesize )
			{ lz4::decompress( compressed, data.view() ); }
};

std::unique_ptr<FileHandle> Lz4File::openRead() const
	{ return std::make_unique<Lz4FileHandle>( compressedData().constView(), filesize() ); }

bool Lz4File::compressFile( std::wstring filepath, ConstByteView data ){
	Buffer out( lz4::maxSize( data.size() ) );
	return writeCompressed( filepath.c_str(), data.size(), lz4::compress( data, out.view() ) );
}