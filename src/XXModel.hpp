/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef XX_MODEL_HPP
#define XX_MODEL_HPP

#include "ArrayView.hpp"
#include "Buffer.hpp"

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
		ArrayView unknown1;
		int index; //Material index
		ArrayView faces;
		ArrayView vertexes;
		ArrayView unknown2; //Use name at end?
		
	public:
		XXMesh( BufferReader& reader, int format, int vector2count );
};

class XXFrame{
	private:
		//Name length
		NotArrayView name;
		//Frames/children count
		ArrayView header1;
		//Submeshes count
		ArrayView header2;
		//[Submeshes]...
		ArrayView vertice_dupes; //Donno
		ArrayView bones;
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
			NotArrayView name;
			ArrayView unknown;
			
			auto size() const{ return 4 + name.size() + unknown.size(); }
		};
		
		//Name length
		NotArrayView name;
		ArrayView colors;
		std::vector<TextureRef> textures;
		ArrayView unknown;
		
	public:
		XXMaterial( BufferReader& reader );
		
		auto size() const
			{ return 4 + name.size() + colors.size() + sumXXSize( textures ) + unknown.size(); }
};

class XXTexture{
	private:
		//Name length
		NotArrayView name;
		ArrayView header;
		//Data length
		ArrayView data;
		
	public:
		XXTexture( BufferReader& reader );
		
		auto size() const{ return 4 + name.size() + header.size() + 4 + data.size(); }
};

class XXModel{
	private:
		Buffer data;
	
	public:
		ArrayView data_header;
		XXFrame frame;
		ArrayView unknown3; //Supposedly related to materials
		//Material count
		std::vector<XXMaterial> materials;
		//Texture count
		std::vector<XXTexture> textures;
		ArrayView unknown4; //A footer? Nah... can't be
		
	public:
		XXModel( Buffer file );
};


#endif
