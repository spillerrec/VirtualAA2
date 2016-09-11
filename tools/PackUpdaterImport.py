#!/usr/bin/env python

# This file is part of VirtualAA2, which is free software and is licensed
# under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ )

import itertools
import shutil
import os
import sys

def isRegKey(   str ): return str.startswith( 'Software\illusion\AA2' )
def isFilePath( str ): return str.startswith( '%s/data/' ) # and/or ends with '.pp'
def isFromPP( str ):   return str.startswith( '%s/data/up' )

def isRelevant( str ): return isRegKey(str) or isFilePath(str)

def pathFromGame( str ):
	if( str.endswith( "AA2Edit\\" ) ): return "aa2edit"
	if( str.endswith( "AA2Play\\" ) ): return "aa2main"
	raise Exception( "Unknown game appeared", str )

def splitOnCondition( func, arr ):
	count = 0
	keys = []
	for x in arr:
		if( func(x) ):
			count += 1
		keys.append( count )
	
	grouped = itertools.groupby( zip(arr, keys), lambda x: x[1] )
	return [list(map(lambda x: x[0], group)) for key, group in grouped ]

def main():
	#Get the arguments
	if( len( sys.argv ) != 3 ):
		print( "PackUpdaterImport <dir-of-PackUpdater> <output-dir>" )
		print( "Usage: Pipe the result from strings2.exe to PackUpdaterImport:" )
		print( "strings2.exe <path-of-PackUpdater.exe> | PackUpdaterImport ..." )
		return
	baseDir   = sys.argv[1]
	outputDir = sys.argv[2]
	
	# Get all the strings piped in from "string2.exe"
	allStrings = [ line.decode( sys.stdout.encoding ).rstrip('\r\n') for line in iter(sys.stdin.buffer.readline, b'') ]
	if( len(allStrings) == 0 ): #TODO
		print( "No strings piped to here" )
	
	# Remove all non-relevant strings
	strings = list(filter( isRelevant, allStrings ))
	
	
	games = { files[0]: files[1:] for files in splitOnCondition( isRegKey, strings ) }
	#print( games )
	
	# Start copying stuff
	for game, files in games.items():
		path = outputDir + "\\" + pathFromGame( game )
		os.makedirs( path + "\\data", exist_ok=True )
		
		#Split files
		local = [file for file in files if     isFromPP( file ) ]
		end   = [file for file in files if not isFromPP( file ) ]
		if( len(local) != len(end) ): #TODO
			raise Exception( "Not the same amount of input PP files as output PP files", len(local), len(end) )
		
		# Copy the files with the corresponding filenames
		for copy, to in zip(local, end):
			shutil.copyfile( copy % baseDir, to % path )
		
	
if __name__ == "__main__":
	main()