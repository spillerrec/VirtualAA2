/*	This file is part of VirtualAA2, which is free software and is licensed
 * under the terms of the GNU GPL v3.0. (see http://www.gnu.org/licenses/ ) */
#ifndef XX_MODEL_HPP
#define XX_MODEL_HPP

#include "ArrayView.hpp"
#include "Buffer.hpp"

#include <vector>

class File;
class BufferReader; //TODO:
class XXFrame;

class XXMesh{
	private:
		ArrayView unknown1;
		int index; //Material index
		ArrayView faces;
		ArrayView vertexes;
		ArrayView unknown2; //Use name at end?
		
	public:
		XXMesh( XXFrame& parent, BufferReader& reader );
};

class XXFrame{
	private:
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
	//TODO:
};

class XXTexture{
	//TODO:
};

class XXModel{
	private:
		Buffer data;
		
	//Data
		uint32_t format;
		uint8_t unknown1;
		ArrayView unknown2; //Some kind of header
		//Frame
		ArrayView unknown3; //Supposedly related to materials
		//Material count
		//[Materials]...
		//Texture count
		//[Textures]...
		ArrayView unknown4; //A footer? Nah... can't be
	
	public:
		XXFrame frame;
		std::vector<XXMaterial> materials;
		std::vector<XXTexture> textures;
		
	public:
		XXModel( Buffer file );
};


#endif
