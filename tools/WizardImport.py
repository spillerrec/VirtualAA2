
import shutil
import os
import sys
import py7zlib

def replaceStartIf( str, prefix, replacement ):
	if str.startswith( prefix ):
		return str.replace( prefix, replacement, 1 )
	return str

# Extract a 7z file to path
def extractFile( path, file ):
	os.makedirs( os.path.dirname(path), exist_ok=True )
	fp = open( path, 'wb' );
	fp.write( file.read() )
	fp.close()

def main():
	#Get the arguments
	if( len( sys.argv ) != 3 ):
		print( "WizardImport <path of 7z file> <Mod name>" )
		return
	wizard_file = sys.argv[1]
	mod_name    = sys.argv[2]
	mod_dir_path = "[MOD] " + mod_name + "/";
	
	def renamePath( rename ):
		rename = replaceStartIf( rename, "AA2_MAKE", "aa2edit/data" );
		rename = replaceStartIf( rename, "AA2_PLAY", "aa2main/data" );
		return rename
	
	try:
		fp = open(wizard_file, 'rb')
		archive = py7zlib.Archive7z(fp)
		for name, file in zip(archive.getnames(), archive.getmembers()):
			outputPath = mod_dir_path + renamePath(name)
			print( outputPath, "\t- ", file.size, " bytes" )
			extractFile( outputPath, file )
		
		#TODO: rename PP folders to "[PP] <NAME>.pp" from "<NAME>"
		#TODO: Add fake "__HEADER" file
		
	finally:
		if fp:
			fp.close()
	
if __name__ == "__main__":
	main()
