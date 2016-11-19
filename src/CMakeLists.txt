cmake_minimum_required(VERSION 3.0)

# Turn on warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

set(SOURCES_DECODERS
	decoders/BulkArchive.cpp
	decoders/PPArchive.cpp
	decoders/PPCrypto.cpp
	decoders/XXModel.cpp
	)

set(SOURCES_ENCODERS
	encoders/BulkArchiveWriter.cpp
	encoders/PPCompactor.cpp
#	encoders/XXCompactor.cpp
	)

set(SOURCES_COMPRESSORS
	compressors/deflate.cpp
	compressors/lz4.cpp
	compressors/lzma.cpp
	)

set(SOURCES_FILESYSTEM
	filesystem/mergers/AMergingObject.cpp
	filesystem/mergers/FakeDir.cpp
	filesystem/mergers/MergedPPFile.cpp
	filesystem/CompressedFile.cpp
	filesystem/DeflateFile.cpp
	filesystem/FileObject.cpp
	filesystem/FileFactory.cpp
	filesystem/FileSystem.cpp
	filesystem/Lz4File.cpp
	filesystem/LzmaFile.cpp
	filesystem/PassthroughDir.cpp
	filesystem/PassthroughFile.cpp
	filesystem/PPFolder.cpp
	filesystem/VirtualDataDir.cpp
	)

set(SOURCES_RESOURCES
	resources/Deduper.cpp
	)

set(SOURCES_UTILS
	utils/ArrayView.cpp
	utils/StringView.cpp
	utils/File.cpp
	)
	
set(SOURCES_USER_FILESYSTEM filesystem-win.cpp)

# add_executable(testing ${SOURCES_DECODERS} ${SOURCES_FILESYSTEM} ${SOURCES_UTILS} main.cpp)

ADD_DEFINITIONS(-DUNICODE)
SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} -municode" )

add_library(VirtualAA2Core ${SOURCES_DECODERS} ${SOURCES_ENCODERS} ${SOURCES_COMPRESSORS} ${SOURCES_FILESYSTEM} ${SOURCES_RESOURCES} ${SOURCES_UTILS} ${SOURCES_USER_FILESYSTEM} )

add_executable(VirtualAA2 main.cpp)
target_link_libraries(VirtualAA2 VirtualAA2Core -ldokan1 -llz4 -lz -llzma)

# Enable C++14 features
set_property(TARGET VirtualAA2     PROPERTY CXX_STANDARD 14)
set_property(TARGET VirtualAA2Core PROPERTY CXX_STANDARD 14)
set_property(TARGET VirtualAA2     PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET VirtualAA2Core PROPERTY CXX_STANDARD_REQUIRED ON)
