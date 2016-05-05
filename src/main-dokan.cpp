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

#include "filesystem/VirtualDataDir.hpp"

std::unique_ptr<VirtualDataDir> data_dir;

static void print( WStringView str ){
	for( auto wc : str )
		std::wcout << wc;
}

static void setFileSize( DWORD& high, DWORD& low, int64_t filesize ){
	LARGE_INTEGER large;
	large.QuadPart = filesize;
	high = large.HighPart;
	low  = large.LowPart;
}

struct FilePath{
	std::vector<WStringView> path;
	
	FilePath( WStringView filepath ) : path( split<const wchar_t>( filepath, L'\\' ) ) { }
	FilePath( LPCWSTR filepath ) : FilePath( makeView( filepath ) ) { }
	
	bool hasRoot() const{ return path.size() > 0 && path[0].size() == 0; }
	bool isRoot() const{ return hasRoot() && path.size() == 1; }
};

namespace VirtualAA2{

NTSTATUS DOKAN_CALLBACK CreateFile( LPCWSTR filename, PDOKAN_IO_SECURITY_CONTEXT, ACCESS_MASK DesiredAccess, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PDOKAN_FILE_INFO file_info ){
	auto dir = data_dir->getFromPath( { filename } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
	if( ! (DesiredAccess & (FILE_READ_DATA | FILE_READ_ATTRIBUTES ) ) )
		return STATUS_ACCESS_VIOLATION;
	
	file_info->IsDirectory = dir->isDir();
	
	return STATUS_SUCCESS;
}


NTSTATUS DOKAN_CALLBACK ReadFile( LPCWSTR filename, LPVOID buffer, DWORD bytes_to_read, LPDWORD bytes_read, LONGLONG offset, PDOKAN_FILE_INFO ){
//	std::cout << "ReadFile\n";
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DOKAN_CALLBACK GetFileInformation( LPCWSTR filename, LPBY_HANDLE_FILE_INFORMATION info, PDOKAN_FILE_INFO file_info ){
	auto dir = data_dir->getFromPath( { filename } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
//	std::wcout << L"GetFileInformation: " << filename << std::endl;
	
	info->dwFileAttributes = dir->isDir() ? FILE_ATTRIBUTE_DIRECTORY : 0;
	
	info->ftCreationTime   = { 0 };
	info->ftLastAccessTime = { 0 };
	info->ftLastWriteTime  = { 0 };
	
	setFileSize( info->nFileSizeHigh, info->nFileSizeLow, dir->filesize() );
	
	//For files: FILE_ATTRIBUTE_READONLY
	
	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK FindFiles( LPCWSTR path, PFillFindData insert, PDOKAN_FILE_INFO file_info ){
	auto dir = data_dir->getFromPath( { path } );
	if( !dir )
		return STATUS_OBJECT_NAME_NOT_FOUND;
	
	for( uint64_t i=0; i<dir->children(); i++ ){
		auto& child = (*dir)[i];
		
		WIN32_FIND_DATA file = { 0 };
		file.dwFileAttributes = child.isDir() ? FILE_ATTRIBUTE_DIRECTORY : 0;
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
	
	data_dir = std::make_unique<VirtualDataDir>( data_dir_path );
	
	_DOKAN_OPERATIONS func = { 0 };
	
	using namespace VirtualAA2;
	func.ZwCreateFile = CreateFile;
	func.ReadFile = ReadFile;
	func.GetFileInformation = GetFileInformation;
	func.FindFiles = FindFiles;
	
	auto result = DokanMain( &options, &func );
	if( result != 0 ){
		std::cout << "Something went wrong";
	}
	
	return result;
}
