/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef XX_MODEL_HPP
#define XX_MODEL_HPP

#include "../utils/ArrayView.hpp"
#include "../utils/Buffer.hpp"

#include <algorithm>
#include <vector>

class File;
class ByteViewReader;

namespace XX{
class Frame;

template<typename T>
auto sumXXSize( const std::vector<T>& arr ){
	return std::accumulate( arr.begin(), arr.end(), 0u
		,	[](auto acc, auto t){ return t.size() + acc; } );
}

class Mesh{
	private:
		ByteView unknown1;
		int index; //Material index
		ByteView faces;
		ByteView vertexes;
		ByteView unknown2; //Use name at end?
		
	public:
		Mesh( ByteViewReader& reader, int format, int vector2count );
};

class Frame{
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
		//[Frame]...
	
	public:
		std::vector<Mesh> meshes;
		std::vector<Frame> children;
		
	public:
		Frame() { } //TODO: Avoid?
		Frame( ByteViewReader& reader, int format );
};

class Material{
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
		Material( ByteViewReader& reader );
		
		auto size() const
			{ return 4 + name.size() + colors.size() + sumXXSize( textures ) + unknown.size(); }
};

class Texture{
	public:
		//Name length
		NotByteView name;
		ByteView header;
		//Data length
		ByteView data;
		
	public:
		Texture( ByteViewReader& reader );
		
		auto size() const{ return 4 + name.size() + header.size() + 4 + data.size(); }
};

class Model{
	private:
		Buffer data;
	
	public:
		ByteView data_header;
		Frame frame;
		ByteView unknown3; //Supposedly related to materials
		//Material count
		std::vector<Material> materials;
		//Texture count
		std::vector<Texture> textures;
		ByteView unknown4; //A footer? Nah... can't be
		
	public:
		Model( Buffer file );
};

}

#endif
