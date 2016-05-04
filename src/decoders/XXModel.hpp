/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef XX_MODEL_HPP
#define XX_MODEL_HPP

#include "../utils/ArrayView.hpp"
#include "../utils/Buffer.hpp"

#include <algorithm>
#include <vector>

class File;
class BufferReader; //TODO:
class XXFrame;

template<typename T>
auto sumXXSize( const std::vector<T>& arr ){
	return std::accumulate( arr.begin(), arr.end(), 0u
		,	[](auto acc, auto t){ return t.size() + acc; } );
}

class XXMesh{
	private:
		ByteView unknown1;
		int index; //Material index
		ByteView faces;
		ByteView vertexes;
		ByteView unknown2; //Use name at end?
		
	public:
		XXMesh( BufferReader& reader, int format, int vector2count );
};

class XXFrame{
	private:
		//Name length
		NotByteView name;
		//Frames/children count
		ByteView header1;
		//Submeshes count
		ByteView header2;
		//[Submeshes]...
		ByteView vertice_dupes; //Donno
		ByteView bones;
		//[XXFrame]...
	
	public:
		std::vector<XXMesh> meshes;
		std::vector<XXFrame> children;
		
	public:
		XXFrame() { } //TODO: Avoid?
		XXFrame( BufferReader& reader, int format );
};

class XXMaterial{
	private:
		struct TextureRef{
			//Name length
			NotByteView name;
			ByteView unknown;
			
			auto size() const{ return 4 + name.size() + unknown.size(); }
		};
		
		//Name length
		NotByteView name;
		ByteView colors;
		std::vector<TextureRef> textures;
		ByteView unknown;
		
	public:
		XXMaterial( BufferReader& reader );
		
		auto size() const
			{ return 4 + name.size() + colors.size() + sumXXSize( textures ) + unknown.size(); }
};

class XXTexture{
	public:
		//Name length
		NotByteView name;
		ByteView header;
		//Data length
		ByteView data;
		
	public:
		XXTexture( BufferReader& reader );
		
		auto size() const{ return 4 + name.size() + header.size() + 4 + data.size(); }
};

class XXModel{
	private:
		Buffer data;
	
	public:
		ByteView data_header;
		XXFrame frame;
		ByteView unknown3; //Supposedly related to materials
		//Material count
		std::vector<XXMaterial> materials;
		//Texture count
		std::vector<XXTexture> textures;
		ByteView unknown4; //A footer? Nah... can't be
		
	public:
		XXModel( Buffer file );
};


#endif
