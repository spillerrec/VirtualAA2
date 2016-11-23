/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "MergedPPFile.hpp"
#include "../PPFile.hpp"
#include "../../decoders/PPArchive.hpp"
#include "../../decoders/PPCrypto.hpp"
#include "../../utils/debug.hpp"

#include <algorithm>
#include <iostream>

uint64_t MergedPPFile::headerSize() const{
	auto magic_size = 8 + 4 + 1+ + 4; //magic, version, unknown1, file count
	return magic_size + ( 260 + 4 + 4 + 20 ) * files.size() + 4; // ( name, offset, size, meta ) + data offset
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
	
	std::sort( files.begin(), files.end() );
}

uint64_t MergedPPFile::filesize() const{
	//Sum of all sub-files
	auto data_size = std::accumulate( files.begin(), files.end(), 0u
		,	[](uint64_t acc, const PPSubFileReference& file)
				{ return acc + file.filesize(); }
		);
	
	return headerSize() + data_size;
}

MergedPPFile::FileIterator MergedPPFile::firstSubFile( uint64_t offset ) const{
	//The first file were the last byte of it is smaller than offset
	auto comp = []( const PPSubFileReference& file, uint64_t offset )
		{ return file.offset + file.filesize() < offset; };
	return std::lower_bound( files.begin(), files.end(), offset, comp );
}

MergedPPFile::FileIterator MergedPPFile::lastSubFile( uint64_t offset ) const{
	//The last file were the first byte of it is smaller than offset
	auto comp = []( uint64_t offset, const PPSubFileReference& file )
		{ return offset < file.offset; };
	return std::upper_bound( files.begin(), files.end(), offset, comp );
}

struct OffsetView{
	ByteView view;
	uint64_t offset;
	
	OffsetView( ByteView view, uint64_t offset ) : view(view), offset(offset) { }
	
	uint64_t left()  const{ return offset; }
	uint64_t right() const{ return offset + view.size(); }
	
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
			view.copyInto( 0, 0, ConstByteView{ PP::Header::magic, PP::Header::magic_length } );
			
			//TODO: version 4
			view.copyIntoEncrypt( PP::Header::magic_length, 0, UInt32View( 109 ).view(), PP::HeaderDecrypter() ); //TODO:
			//TODO: unknown1 1
			view.copyInto( PP::Header::magic_length, 4, UInt32View( 0x35 ).view() ); //TODO:
			//File count
			view.copyIntoEncrypt( PP::Header::magic_length, 5, UInt32View( pp.subfiles().size() ).view(), PP::HeaderDecrypter() );
			
			return view.leftAt( header_header_size ).view.size();
		}
		uint64_t readHeaderFile( const PPSubFileReference& file, OffsetView view, uint64_t position ){
			if( view.view.size() > 0 ){
				//Make untouched bytes null
				for( auto& val : view.view )
					val = 0;
				
				//Write header
				view.copyInto( position,   0, file.parent->filename );
				view.copyInto( position, 260, UInt32View(file.parent->file->filesize()).view() );
				view.copyInto( position, 264, UInt32View(file.offset                 ).view() );
				view.copyInto( position, 268, file.parent->metadata );
				
				//encrypt
				header_encrypt.setPosition( view.offset - header_header_size );
				header_encrypt.decrypt( view.view );
			}
			return view.view.size();
		}
		uint64_t readHeaderFiles( OffsetView view ){
			auto first_index = (view.left()  - header_header_size) / header_file_size;
			auto  last_index = (view.right() - header_header_size) / header_file_size;
			last_index = std::min( last_index, pp.subfiles().size()-1 ); //TODO: size == 0
			
			uint64_t written = 0;
			for( auto i=first_index; i<=last_index; i++ ){
				auto position = header_header_size + i*header_file_size;
				auto file_view = view.rightAt( position ).leftAt( position + header_file_size );
				written += readHeaderFile( pp.subfiles()[i], file_view, position );
			}
			
			auto data_offset = pp.headerSize();
			view.rightAt( data_offset-4 ).copyIntoEncrypt( data_offset-4, 0, UInt32View( data_offset ).view(), PP::HeaderDecrypter() );
			
			return written;
		}
		
		uint64_t readHeader( OffsetView view ){
			if( view.view.size() == 0 )
				return 0;
			
			return readHeaderHeader( view.leftAt( header_header_size ) )
			   +   readHeaderFiles( view.rightAt( header_header_size ) )
			   ;
		}
		
		
		uint64_t readFile( const PPSubFileReference& file, OffsetView view ){
			if( view.view.size() > 0 ){
				always( (bool)file.parent->file, "PPSubFile did not have a file" );
				auto& handle = open_handle.open( *file.parent->file );
				
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
			if( view.view.size() == 0 )
				return 0;
			
			uint64_t written = 0;
			auto subfile = pp.firstSubFile( view.left()  );
			auto last    = pp. lastSubFile( view.right() );
			for( ; subfile!=last; subfile++ ){
				auto file_view = view.rightAt( subfile->offset ).leftAt( subfile->offset + subfile->parent->file->filesize() );
				written += readFile( *subfile, file_view );
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

bool MergedPPFile::contains( const PPSubFile& file ) const{
	return std::any_of( files.begin(), files.end()
		,	[&]( auto& item ){ return item.parent->filename == file.filename; }
		);
}
	
void MergedPPFile::combine( const FileObject& with ){
	//Combine with MergedPPFile if possible
	if( type() == with.type() ){
		combine( *static_cast<const MergedPPFile*>( &with ) );
		return;
	}
	
	//Combine with PPFile if possible
	if( with.type() == ID::PP_FILE ){
		combine( *static_cast<const PPFile*>( &with ) );
		return;
	}
	
	std::cout << "Merging with: " << with.type() << "\n";
	throw std::runtime_error( "MergedPPFile could not combine with object" );
}

void MergedPPFile::combine( const class PPFile& with ){
	//Check for collisions
	for( auto& sub : with.subfiles() )
		if( contains( sub ) )
			throw std::runtime_error( "Duplicate element in PP file (TODO: support merging)" );
	
	//Add files
	addPP( with.subfiles() );
}

void MergedPPFile::combine( const MergedPPFile& with ){
	//Prepare new subfiles
	std::vector<PPSubFile> new_files;
	new_files.reserve( with.subfiles().size() );
	
	for( auto& sub : with.subfiles() ){
		//Check for collisions
		if( contains( *sub.parent ) )
			throw std::runtime_error( "Duplicate element in PP file (TODO: support merging)" );
		new_files.push_back( *sub.parent );
	}
	
	addPP( new_files );
}