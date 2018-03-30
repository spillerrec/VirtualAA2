#include <stdio.h>
#include <stdlib.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
using namespace glm;

#include <decoders/XXModel.hpp>
#include <utils/File.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>

std::string read_shader( const char * file_path ){
	std::ifstream stream( file_path, std::ios::in );
	if(stream.is_open()){
		std::string code;
		std::string line = "";
		while(getline(stream, line))
			code += "\n" + line;
		return code;
	}else{
		printf( "Couldn't read file %s\n", file_path );
		getchar();
		return {};
	}
}

GLuint compile_shader( char const* code_ptr, GLenum shader_type ){
	GLint Result = GL_FALSE;
	int InfoLogLength;
	
	GLuint id = glCreateShader( shader_type );

	// Compile Vertex Shader
	printf("Compiling shader\n");
	glShaderSource( id, 1, &code_ptr , nullptr );
	glCompileShader( id );

	// Check Vertex Shader
	glGetShaderiv( id, GL_COMPILE_STATUS, &Result);
	glGetShaderiv( id, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if ( InfoLogLength > 0 ){
		std::vector<char> error_message(InfoLogLength+1);
		glGetShaderInfoLog( id, InfoLogLength, nullptr, &error_message[0]);
		printf("%s\n", &error_message[0]);
	}
	
	return id;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
	// Read the Vertex and Fragment Shader code from the file
	std::string VertexShaderCode   = read_shader( vertex_file_path   );
	std::string FragmentShaderCode = read_shader( fragment_file_path );

	// Compile Shaders
	GLuint VertexShaderID   = compile_shader( VertexShaderCode  .c_str(), GL_VERTEX_SHADER   );
	GLuint FragmentShaderID = compile_shader( FragmentShaderCode.c_str(), GL_FRAGMENT_SHADER );

	// Link the program
	printf("Linking program\n");
	GLuint ProgramID = glCreateProgram();
	glAttachShader( ProgramID, VertexShaderID   );
	glAttachShader( ProgramID, FragmentShaderID );
	glLinkProgram(  ProgramID );

	// Check the program
	GLint Result = GL_FALSE;
	int InfoLogLength;
	glGetProgramiv( ProgramID, GL_LINK_STATUS, &Result );
	glGetProgramiv( ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength );
	if ( InfoLogLength > 0 ){
		std::vector<char> ProgramErrorMessage( InfoLogLength+1 );
		glGetProgramInfoLog(ProgramID, InfoLogLength, nullptr, &ProgramErrorMessage[0]);
		printf("%s\n", &ProgramErrorMessage[0]);
	}

	
	glDetachShader(ProgramID, VertexShaderID);
	glDetachShader(ProgramID, FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);

	return ProgramID;
}

class Model{
	private:
		struct Vertex{
			GLfloat x, y, z;
			GLfloat u, v;
			//Weights??
			//Bones
			//Normals ??
			uint16_t id;
			
			Vertex( ByteView line );
			
			bool operator<( const Vertex& other ) const{ return id < other.id; };
		};
		struct Face{
			uint16_t a, b, c;
			Face( ByteView line );
		};
		
		std::vector<Vertex> vertices;
		std::vector<Face> faces;
		std::vector<Vertex*> vertex_lookup;
		uint32_t material_index;
		
		Vertex* getVertex( uint16_t id ) const{
			if( id >= vertex_lookup.size() ){
				std::cout << "ID too large: " << id << std::endl;
				return nullptr;
			}
			if( !vertex_lookup[id] ){
				std::cout << "Vertex not defined! " << id << std::endl;
			}
			return vertex_lookup[id];
		}
		
		void addMesh( const XX::Mesh& input );
		void createLookup();
		
	public:
		Model( const XX::Mesh& mesh ){
			addMesh( mesh );
			createLookup();
		}
		Model( const std::vector<XX::Mesh>& meshes ){
			for( auto& mesh : meshes )
				addMesh( mesh );
			createLookup();
		}
		
		void addFaces( std::vector<GLfloat>& points ) const;
};

void Model::addMesh( const XX::Mesh& input ){
	faces.reserve( faces.size() + input.faces_count );
	for( unsigned i=0; i < input.faces_count; i++ )
		faces.emplace_back( input.faces.subView( i*6, 6 ) );
	
	vertices.reserve( vertices.size() + input.vertex_count );
	for( unsigned i=0; i < input.vertex_count; i++ )
		vertices.emplace_back( input.vertexes.subView( i*70, 70 ) );
};

void Model::createLookup(){
	//Construct lookup
	auto max_id = std::max_element( vertices.begin(), vertices.end() )->id;
	vertex_lookup.clear();
	vertex_lookup.resize( max_id+1, nullptr );
	for( auto& vertex : vertices ){
		if( vertex_lookup[vertex.id] )
			std::cout << "Warning, id already defined: " << vertex.id << std::endl;
		vertex_lookup[vertex.id] = &vertex;
	}
}

void Model::addFaces( std::vector<GLfloat>& points ) const{
	points.reserve( points.size() + faces.size() * 3 * 3 );
	for( auto& face : faces ){
		/*
		auto addVertex = [&]( uint16_t id ){
				auto vertex = getVertex( id );
				if( !vertex )
					throw std::runtime_error( "Missing vertex lookup" );
				
				points.push_back( vertex->x );
				points.push_back( vertex->y );
				points.push_back( vertex->z );
			};
		addVertex( face.a );
		addVertex( face.b );
		addVertex( face.b );
		addVertex( face.c );
		addVertex( face.c );
		addVertex( face.a );
		/*/
		auto vertex_a = getVertex( face.a );
		auto vertex_b = getVertex( face.b );
		auto vertex_c = getVertex( face.c );
		if( vertex_a && vertex_b && vertex_c ){
			auto addVertex = [&]( const Vertex& v ){
					points.push_back( v.x );
					points.push_back( v.y );
					points.push_back( v.z );
				};
			addVertex( *vertex_a );
			addVertex( *vertex_b );
			addVertex( *vertex_b );
			addVertex( *vertex_c );
			addVertex( *vertex_c );
			addVertex( *vertex_a );
		}
		//*/
	}
}

Model::Vertex::Vertex( ByteView line ){
	id = *(uint16_t*)(line.begin());
	auto floats = (float*)(line.begin() + 2);
	x = floats[0];
	y = floats[1];
	z = floats[2];
	//Weights3 - 3 * floats
	//BoneIndices - 4 * bytes
	//Normal - 3 floats
	u = floats[10];
	v = floats[10];
	//20 bytes of unknown
}

Model::Face::Face( ByteView line ){
	auto ids = (uint16_t*)(line.begin());
	a = ids[0];
	b = ids[1];
	c = ids[2];
}

void getMeshes( const XX::Frame& frame, std::vector<Model>& models ){
	if( frame.meshes.size() > 0 )	
	//	models.emplace_back( frame.meshes );
		for( auto& mesh : frame.meshes )
			models.emplace_back( mesh );
	
	for( auto& child : frame.children )
		getMeshes( child, models );
}

int main( int argc, char* argv[] ){
	if( argc != 2 )
		return -1;
	XX::Model xx( File( argv[1], "rb" ).readAll() );
	std::vector<Model> models;
	getMeshes( xx.frame, models );
	std::cout << "Amount of models: " << models.size() << std::endl;
	
	// Initialise GLFW
	if( !glfwInit() ){
		fprintf( stderr, "Failed to initialize GLFW\n" );
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL 

	// Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global)
	window = glfwCreateWindow( 1024, 768, "Tutorial 01", NULL, NULL);
	if( window == NULL ){
		fprintf( stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n" );
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window); // Initialize GLEW
	glewExperimental=true; // Needed in core profile
	if( glewInit() != GLEW_OK ){
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}
	
	GLuint programID = LoadShaders( "vertex.glsl", "fragment.glsl" );
	
	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1, &VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	// An array of 3 vectors which represents 3 vertices
	std::vector<GLfloat> vertices;
	
	for( auto& model : models ){
		model.addFaces( vertices );
	}
	for( auto& i : vertices ){
		i /= 25;
	}
	
	// Get a handle for our "MVP" uniform
	GLuint MatrixID = glGetUniformLocation(programID, "MVP");

	// This will identify our vertex buffer
	GLuint vertexbuffer;
	// Generate 1 buffer, put the resulting identifier in vertexbuffer
	glGenBuffers(1, &vertexbuffer);
	// The following commands will talk about our 'vertexbuffer' buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
	// Give our vertices to OpenGL.
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);

	GLfloat i = 0;
	do{
		glm::mat4 MVP{ 1 };
		MVP = glm::rotate( MVP, i, glm::vec3(1,0,0) );
	
		glClearColor(0.0f, 0.0f, 0.4f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		// 1rst attribute buffer : vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
		glVertexAttribPointer(
			0,                  // attribute 0. No particular reason for 0, but must match the layout in the shader.
			3,                  // size
			GL_FLOAT,           // type
			GL_FALSE,           // normalized?
			0,                  // stride
			(void*)0            // array buffer offset
		);
		// Use our shader
		glUseProgram(programID);
		glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
		// Draw the triangle !
		//glDrawArrays(GL_TRIANGLES, 0, vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDrawArrays(GL_LINES, 0, vertices.size()); // Starting from vertex 0; 3 vertices total -> 1 triangle
		glDisableVertexAttribArray(0);

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
		i += 0.02;
	} // Check if the ESC key was pressed or the window was closed
	while( glfwGetKey(window, GLFW_KEY_ESCAPE ) != GLFW_PRESS && glfwWindowShouldClose(window) == 0 );
	
	return 0;
}