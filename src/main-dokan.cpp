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

#include "filesystem/PassthroughDir.hpp"

static void print( WStringView str ){
	for( auto wc : str )
		std::wcout << wc;
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
	std::wcout << "CreateFile: " << filename << std::endl;
	
	FilePath path( filename );
	require( path.hasRoot() );
	
/*	for( auto part : path.path ){
		std::cout << "\t";
		print( part );
		std::cout << "\n";
	}
	*/
	if( ! (DesiredAccess & (FILE_READ_DATA | FILE_READ_ATTRIBUTES ) ) )
		return STATUS_ACCESS_VIOLATION;
//	std::cout << "OK\n";
	
	file_info->IsDirectory = true;
	
	return STATUS_SUCCESS;
}


NTSTATUS DOKAN_CALLBACK ReadFile( LPCWSTR filename, LPVOID buffer, DWORD bytes_to_read, LPDWORD bytes_read, LONGLONG offset, PDOKAN_FILE_INFO ){
//	std::cout << "ReadFile\n";
	return STATUS_NOT_IMPLEMENTED;
}

NTSTATUS DOKAN_CALLBACK GetFileInformation( LPCWSTR filename, LPBY_HANDLE_FILE_INFORMATION info, PDOKAN_FILE_INFO file_info ){
//	std::wcout << L"GetFileInformation: " << filename << std::endl;
	
	info->dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
	
	info->ftCreationTime   = { 0 };
	info->ftLastAccessTime = { 0 };
	info->ftLastWriteTime  = { 0 };
	
	info->nFileSizeHigh = 0;
	info->nFileSizeLow = 0;
	
	//For files: FILE_ATTRIBUTE_READONLY
	
	return STATUS_SUCCESS;
}

NTSTATUS DOKAN_CALLBACK FindFiles( LPCWSTR path, PFillFindData insert, PDOKAN_FILE_INFO file_info ){
	std::cout << "FindFiles\n";
	
	FilePath filepath( path );
	std::wstring sub = filepath.isRoot() ? L"" : std::wstring(path);
	PassthroughDir dir( std::wstring(L"aa2dir\\data") + sub );
	
	std::cout << "Children: " << dir.children() << "\n";
	for( uint64_t i=0; i<dir.children(); i++ ){
		auto& child = dir[i];
		auto name = child.name();
		std::wcout << "Adding child: ";
		//print( name );
		std::wcout << "\n";
		require( name.size() < MAX_PATH );
		
		WIN32_FIND_DATA file = { 0 };
		file.dwFileAttributes = child.isDir() ? FILE_ATTRIBUTE_DIRECTORY : 0;
		//file.nFileSizeLow = 400;
		
		for( unsigned j=0; j<name.size(); j++ ){
			file.cFileName[j] = name[j];
		}
		
		auto result = insert( &file, file_info );
		//if( result != 0 ) //Some error
	}
	
	return STATUS_SUCCESS;
}
}

int wmain( int argc, wchar_t* argv[] ){
	_DOKAN_OPTIONS options = { 0 };
	options.Version = 100;
	options.ThreadCount = 1; //TODO: for now
	//options.GlobalContext = 0;
	options.MountPoint = L"v";
	
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