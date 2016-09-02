/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "MergedPPFile.hpp"
#include "../../decoders/PPArchive.hpp"
#include "../../decoders/PPCrypto.hpp"
#include "../../utils/debug.hpp"

#include <iostream>

uint64_t MergedPPFile::headerSize() const{
	auto magic_size = 8 + 4 + 1; //magic, version, unknown1
	return magic_size + ( 260 + 4 + 4 + 20 ) * files.size(); // ( name, offset, size, meta )
}

void MergedPPFile::addPP( const std::vector<PPSubFile>& subfiles ){
	//Add new files
	for( auto& subfile : subfiles )
		files.emplace_back( subfile ); //TODO: check for collisions
	
	//Update offsets
	auto offset = headerSize();
	for( auto& file : files ){
		file.offset = offset;
		offset += file.filesize();
	}
}

uint64_t MergedPPFile::filesize() const{
	//Sum of all sub-files
	auto data_size = std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFileReference& file)
				{ return acc + file.filesize(); }
		);
	
	return headerSize() + data_size;
}

struct OffsetView{
	ByteView view;
	uint64_t offset;
	
	OffsetView( ByteView view, uint64_t offset ) : view(view), offset(offset) { }
	
	uint64_t splitPos( uint64_t position ) const{
		auto relative = int64_t(position) - int64_t(offset);
		//Truncate to be within range of view
		return std::min( uint64_t(std::max( relative, int64_t(0) )), view.size() );
	}
		
	OffsetView leftAt( uint64_t position )
		{ return { view.left( splitPos(position) ), offset }; }
	
	OffsetView rightAt( uint64_t position ){
		auto bytes = view.size() - splitPos(position);
		return { view.right( bytes ), offset + view.size() - bytes };
	}
	
	template<typename T>
	void copyFrom( ArrayView<T> data, uint64_t offset=0 ){
		if( offset < data.size() )
			for( unsigned i=0; i<std::min(view.size(), data.size()-offset); i++ )
				view[i] = data[i+offset];
	}
	
	template<typename T>
	auto copyInto( uint64_t position, uint64_t from, ArrayView<T> data ){
		auto to = rightAt( position+from ).leftAt( position+from+data.size() );
		to.copyFrom( data, to.offset - (position+from) );
		return to;
	}
	
	template<typename T, typename Decrypter>
	void copyIntoEncrypt( uint64_t position, uint64_t from, ArrayView<T> data, Decrypter d ){
		auto to = copyInto( position, from, data );
		if( to.view.size() > 0 ){
			d.setPosition( to.offset - (position+from) );
			d.decrypt( to.view );
		}
	}
	
	OffsetView debug( const char* name ) const{
	//	std::cout << name << ", offset: " << offset << " with size: " << view.size() << "\n";
		return *this;
	}
};

class UInt32View{
	private:
		uint8_t data[4];
	public:
		UInt32View( uint32_t value ) {
			data[0] = (value >>  0) & 0xFF;
			data[1] = (value >>  8) & 0xFF;
			data[2] = (value >> 16) & 0xFF;
			data[3] = (value >> 24) & 0xFF;
		}
		ConstByteView view() { return { data, 4 }; }
};

class PPFileHandle : public FileHandle{
	private:
		static const uint64_t header_header_size = { 8 + 4 + 1 + 4 };
		static const uint64_t header_file_size = { 260 + 4 + 4 + 20 }; //288
		
	private:
		const MergedPPFile& pp;
		PP::HeaderDecrypter header_encrypt;
		
		class CachedHandle{
			private:
				FileObject* file_object{ nullptr };
				std::unique_ptr<FileHandle> handle;
				
			public:
				void clear(){ *this = {}; }
				FileHandle& open( FileObject& file ){
					if( file_object != &file ){
						file_object = &file;
						handle = file.openRead();
					}
					always( (bool)handle, "Could not open file" );
					return *handle;
				}
		} open_handle;
		
		//TODO: assert in all of those, expected end is the same as bytes read?
		uint64_t readHeaderHeader( OffsetView view ){
			view.copyInto( 0, 0, ConstByteView{ PPArchive::magic, PPArchive::magic_length } );
			
			//TODO: version 4
			view.copyIntoEncrypt( PPArchive::magic_length, 0, UInt32View( 109 ).view(), PP::HeaderDecrypter() ); //TODO:
			//TODO: unknown1 1
			view.copyInto( PPArchive::magic_length, 4, UInt32View( 0x35 ).view() ); //TODO:
			//File count
			view.copyIntoEncrypt( PPArchive::magic_length, 5, UInt32View( pp.subfiles().size() ).view(), PP::HeaderDecrypter() );
			
			return view.leftAt( header_header_size ).view.size();
		}
		uint64_t readHeaderFile( const PPSubFileReference& file, OffsetView view, uint64_t position ){
			if( view.view.size() > 0 ){
				//Make untouched bytes null
				for( auto& val : view.view )
					val = 0;
				
				//Write header
				view.copyInto( position,   0, file.parent.filename );
				view.copyInto( position, 260, UInt32View(file.parent.file->filesize()).view() );
				view.copyInto( position, 264, UInt32View(file.offset                 ).view() );
				view.copyInto( position, 268, file.parent.metadata );
				
				//encrypt
				header_encrypt.setPosition( view.offset - header_header_size );
				header_encrypt.decrypt( view.view );
			}
			return view.view.size();
		}
		uint64_t readHeaderFiles( OffsetView view ){
			auto position = header_header_size;
			uint64_t written = 0;
			//TODO: Skip files known not to be in view
			for( auto& subfile : pp.subfiles() ){
				auto new_position = position + header_file_size;
				auto file_view = view.rightAt( position ).leftAt( new_position );
				written += readHeaderFile( subfile, file_view, position );
				position = new_position;
			}
			
			return written;
		}
		
		uint64_t readHeader( OffsetView view ){
			return readHeaderHeader( view.leftAt( header_header_size ) )
			   +   readHeaderFiles( view.rightAt( header_header_size ) )
			   ;
		}
		
		
		uint64_t readFile( const PPSubFileReference& file, OffsetView view ){
			if( view.view.size() > 0 ){
				always( (bool)file.parent.file, "PPSubFile did not have a file" );
				auto& handle = open_handle.open( *file.parent.file );
				
				//Read contents
				auto offset = view.offset - file.offset;
				auto read = handle.read( view.view, offset );
				
				//encrypt
				PP::FileDecrypter encrypter;
				encrypter.setPosition( offset );
				encrypter.decrypt( view.view );
				
				return read;
			}
			
			return 0;
		}
		
		uint64_t readData( OffsetView view ){
			//TODO: more efficient
			uint64_t written = 0;
			for( auto& subfile : pp.subfiles() ){
				auto file_view = view.rightAt( subfile.offset ).leftAt( subfile.offset + subfile.parent.file->filesize() );
				written += readFile( subfile, file_view );
			}
			return written;
		}
		
	public:
		PPFileHandle( const MergedPPFile& pp )
			:	pp( pp ) { }
		
		uint64_t read( ByteView to_read, uint64_t offset ) override{
			OffsetView view( to_read, offset );
			view.debug( "read" );
			auto header_size = pp.headerSize();
			return readHeader( view.leftAt( header_size ) )
			   +   readData( view.rightAt( header_size ) )
			   ;
		}

		uint64_t write( ConstByteView to_write, uint64_t offset ) override
			{ throw std::logic_error( "Trying to write on a no-write PPFileHandle" ); }
};

std::unique_ptr<FileHandle> MergedPPFile::openRead() const
	{ return std::make_unique<PPFileHandle>( *this ); }

std::unique_ptr<AMergingObject> MergedPPFile::createMerger() const
	{ return std::make_unique<MergedPPFile>( *this ); }
