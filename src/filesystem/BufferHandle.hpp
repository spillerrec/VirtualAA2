/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef BUFFER_HANDLE_HPP
#define BUFFER_HANDLE_HPP

#include "FileObject.hpp"
#include "../utils/Buffer.hpp"
#include "../utils/ByteViewReader.hpp"


class BufferReadHandle : public FileHandle{
	protected:
		Buffer data;
		ByteViewReader reader;
		
		BufferReadHandle( uint64_t size ) : data( size ), reader( data ) { }
		
	public:
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			reader.seek( offset );
			auto data = reader.read( std::min(to_read.size(), reader.left()) );
			data.copyTo( to_read );
			return data.size();
		}

		uint64_t write( ConstByteView, uint64_t ) override { return 0; }
};

//TODO: BufferWriteHandle for writing, needs a resizable Buffer


#endif
