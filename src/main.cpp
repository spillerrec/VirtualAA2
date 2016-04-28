/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */

#include "PPArchive.hpp"
#include "File.hpp"


int main( int argc, char* argv[] ){
	if( argc < 2 )
		return -1;
	
	File file( argv[1] );
	PPArchive archive( file );
	
	for( auto& f : archive.files ){
		auto output = f.getFile( file );
		auto fout = fopen( (char*)f.filename.data(), "wb" );
		fwrite( output.data(), 1, output.size(), fout );
		fclose( fout );
	}
	
	return 0;
}

