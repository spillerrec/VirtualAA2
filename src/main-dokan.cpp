/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

extern "C"{
	#include "dokan/dokan.h"
}

#include <ntstatus.h>
#include <string>
#include <iostream>

#include "utils/debug.hpp"
#include "utils/StringView.hpp"

#include "filesystem/FilePath.hpp"
#include "filesystem/VirtualDataDir.hpp"

static void setFileSize( DWORD& high, DWORD& low, int64_t filesize ){
	ULARGE_INTEGER large;
	large.QuadPart = filesize;
	high = large.HighPart;
	low  = large.LowPart;
}

struct PersistentFileObject{
	public:
		const FileObject* object;
		FILE* handle{ 0 };
	
	private:
		PersistentFileObject( const FileObject* object ) : object(object) { }
	
	public:
		static PersistentFileObject* createReadAccess( const FileObject& dir ){
			auto file = new PersistentFileObject( &dir );
			file->handle = dir.openRead();
			return file;
		}
		
		static PersistentFileObject* createWriteAccess( const FileObject& dir ){
			auto file = new PersistentFileObject( &dir );
			file->handle = dir.openWrite();
			return file;
		}
		
		static PersistentFileObject* createAppendAccess( const FileObject& dir ){
			auto file = new PersistentFileObject( &dir );
			file->handle = dir.openAppend();
			return file;
		}
};

void handleToContext( PDOKAN_FILE_INFO file_info, PersistentFileObject* handle )
	{ file_info->Context = reinterpret_cast<ULONG64>( handle ); }

PersistentFileObject* contextToHandle( PDOKAN_FILE_INFO file_info )
	{ return reinterpret_cast<PersistentFileObject*>( file_info->Context ); }

static void setRoot( _DOKAN_OPTIONS& options, VirtualDataDir& root_dir )
	{ options.GlobalContext = reinterpret_cast<ULONG64>( &root_dir ); }

static VirtualDataDir& getRoot( PDOKAN_FILE_INFO file_info )
	{ return *reinterpret_cast<VirtualDataDir*>( file_info->DokanOptions->GlobalContext ); }

namespace VirtualAA2{

NTSTATUS DOKAN_CALLBACK CreateFile( LPCWSTR filename, PDOKAN_IO_SECURITY_CONTEXT, ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO file_info ){
	bool read_only = false;
	auto dir = getRoot( file_info ).getFromPath( { filename } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
//A long range of access checks
	/*
	if( DesiredAccess & (FILE_READ_EA | FILE_EXECUTE) ){
		std::wcout << "Unknown access wanted for: " << filename << "\n";
		return STATUS_NOT_IMPLEMENTED;
	}
	*/
	
	if( DesiredAccess & (FILE_WRITE_DATA | FILE_WRITE_ATTRIBUTES) && (!dir->canWrite() || read_only) ){
		std::wcout << "Write not allowed for: " << filename << "\n";
		return STATUS_ACCESS_VIOLATION;
	}
	
	/*
	if( DesiredAccess & (FILE_WRITE_EA | FILE_WRITE_ATTRIBUTES) ){
		std::wcout << "Attribute changes not implemented: " << filename;
		return STATUS_NOT_IMPLEMENTED;
	}
	*/
	
	file_info->IsDirectory = dir->isDir();
	
	if( !dir->isDir() ){
		const char* debug = "";
		if( DesiredAccess & FILE_READ_DATA ){
			debug = "Creating read handle";
			handleToContext( file_info, PersistentFileObject::createReadAccess( *dir ) );
		}
		else if( DesiredAccess & FILE_WRITE_DATA ){
			debug = "Creating write handle";
			handleToContext( file_info, PersistentFileObject::createWriteAccess( *dir ) );
		}
		else if( DesiredAccess & FILE_APPEND_DATA ){
			debug = "Creating append handle";
			handleToContext( file_info, PersistentFileObject::createAppendAccess( *dir ) );
		}
		else if( DesiredAccess & FILE_READ_ATTRIBUTES )
			debug = "Not understood read attr";
		else if( DesiredAccess & FILE_WRITE_ATTRIBUTES )
			debug = "Not understood write attr";
		else
			debug = "Unknown access request";
		
	//	std::wcout << debug << " for: " << filename << "\n";
	}
	
	return STATUS_SUCCESS;
}


NTSTATUS DOKAN_CALLBACK ReadFile( LPCWSTR filename, LPVOID buffer, DWORD bytes_to_read, LPDWORD bytes_read, LONGLONG offset, PDOKAN_FILE_INFO file_info ){
	auto file = contextToHandle( file_info );
	if( !file ){
		std::cout << "Read handle was missing, re-creating it\n";
		auto dir = getRoot( file_info ).getFromPath( { filename } );
		if( !dir )
			return STATUS_OBJECT_NAME_NOT_FOUND;
		file = PersistentFileObject::createReadAccess( *dir );
	}
	
	ByteView read_buffer( static_cast<uint8_t*>(buffer), bytes_to_read );
	*bytes_read = file->object->read( file->handle, read_buffer, offset );
//	std::wcout << "ReadFile: " << filename << " - " << bytes_to_read << "\n";
	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK WriteFile( LPCWSTR filename, LPCVOID buffer, DWORD bytes_to_write, LPDWORD bytes_written, LONGLONG offset, PDOKAN_FILE_INFO file_info ){
	PersistentFileObject* file = contextToHandle( file_info );
	if( !file ){
		std::cout << "Write handle was missing, re-creating it\n";
		auto dir = getRoot( file_info ).getFromPath( { filename } );
		if( !dir )
			return STATUS_OBJECT_NAME_NOT_FOUND;
		file = PersistentFileObject::createWriteAccess( *dir );
	}
	
	ConstByteView write_buffer( static_cast<const uint8_t*>(buffer), bytes_to_write );
	*bytes_written = file->object->write( file->handle, write_buffer, offset );
	std::wcout << "WriteFile: " << filename << " - " << *bytes_written << "\n";
	return STATUS_SUCCESS;
}


NTSTATUS DOKAN_CALLBACK GetFileInformation( LPCWSTR filename, LPBY_HANDLE_FILE_INFORMATION info, PDOKAN_FILE_INFO file_info ){
	auto dir = getRoot( file_info ).getFromPath( { filename } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
	info->dwFileAttributes = dir->isDir() ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
	
	info->ftCreationTime   = { 0 };
	info->ftLastAccessTime = { 0 };
	info->ftLastWriteTime  = { 0 };
	
	setFileSize( info->nFileSizeHigh, info->nFileSizeLow, dir->filesize() );
	
	//For files: FILE_ATTRIBUTE_READONLY
	
	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK FindFiles( LPCWSTR path, PFillFindData insert, PDOKAN_FILE_INFO file_info ){
	auto dir = getRoot( file_info ).getFromPath( { path } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
	for( uint64_t i=0; i<dir->children(); i++ ){
		auto& child = (*dir)[i];
		
		WIN32_FIND_DATA file = { 0 };
		file.dwFileAttributes = child.isDir() ? FILE_ATTRIBUTE_DIRECTORY : FILE_ATTRIBUTE_NORMAL;
		setFileSize( file.nFileSizeHigh, file.nFileSizeLow, child.filesize() );
		
		auto name = child.name();
		require( name.size() < MAX_PATH );
		for( unsigned j=0; j<name.size(); j++ )
			file.cFileName[j] = name[j];
		
		if( insert( &file, file_info ) != 0 )
			return STATUS_INTERNAL_ERROR;
	}
	
	return STATUS_SUCCESS;
}

void DOKAN_CALLBACK Cleanup( LPCWSTR filename, PDOKAN_FILE_INFO file_info ){
	PersistentFileObject* file = contextToHandle( file_info );
	
	if( file ){
//		std::wcout << "Removing handle for: " << filename << "\n";
		file->object->close( file->handle );
		delete file;
		file_info->Context = 0;
	}
}



void DOKAN_CALLBACK CloseFile( LPCWSTR filename, PDOKAN_FILE_INFO file_info ){
	Cleanup( filename, file_info );
}

}

int wmain( int argc, wchar_t* argv[] ){
	if( argc < 2 ){
		std::cout << "VirtualAA2 <path-to-aa2-data-dir> [mount-point]\n";
		return -1;
	}
	auto data_dir_path = argv[1];
	
	_DOKAN_OPTIONS options = { 0 };
	options.Version = 100;
	options.ThreadCount = 1; //TODO: for now
	//options.GlobalContext = 0;
	options.MountPoint = argc < 3 ? L"v" : argv[2];
	
	VirtualDataDir data_dir( data_dir_path );
	setRoot( options, data_dir );
	
	_DOKAN_OPERATIONS func = { 0 };
	
	using namespace VirtualAA2;
	func.ZwCreateFile = CreateFile;
	func.ReadFile = ReadFile;
	func.WriteFile = WriteFile;
	func.GetFileInformation = GetFileInformation;
	func.FindFiles = FindFiles;
	func.Cleanup = Cleanup;
	func.CloseFile = CloseFile;
	
	auto result = DokanMain( &options, &func );
	if( result != 0 ){
		std::cout << "Something went wrong";
	}
	
	return result;
}
