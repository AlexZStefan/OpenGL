#include "Renderer.h"
#define ASSERT(x) if(!(x)) __debugbreak();
#define GLCALL(x) GLClearError();\
	x;\
	ASSERT(GLLogCall())

namespace GE {

	Vertex triangleVertecies[3] = {
		Vertex(-1.0f, .0f, .0f),
		Vertex(1.0f, .0f, .0f),
		Vertex(.0f, 1.0f, .0f),
	};

	GLfloat vertexData[] = {
		-1.f, .0f,
		1.f, .0f,
		.0f, 1.f,
	};

	// clear errors
	static void GLClearError() {
		while (glGetError() != GL_NO_ERROR);
	}
	// show all errors
	static bool GLLogCall() {
		while (GLenum err = glGetError()) {
			std::cerr << "OpenGl Error : " << err << std::endl;
			return false;
		}
		return true;
	}

	void displayShaderCompilerError(GLuint shaderId) {
		GLint MsgLen = 0;

		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &MsgLen);

		if (MsgLen > 1) {
			//GLchar* Msg = new GLchar[MsgLen + 1];

			GLchar* Msg = (GLchar*)_malloca(MsgLen * sizeof(GLchar));

			glGetShaderInfoLog(shaderId, MsgLen, NULL, Msg);

			std::cerr << "Error compiling shader\n" << Msg << std::endl;

			//delete[] Msg;
			glDeleteShader(shaderId);
		}
	}

	Renderer::Renderer()
	{
		pos_x = pos_y = pos_z = 0.0f;
		rot_x = rot_y = rot_z = 0.0f;
		scale_x = 1.0f;
		scale_y = 1.0f;
		scale_z = 1.0f;
	}

	Renderer::~Renderer()
	{
	}

	void Renderer::init()
	{
		ShaderSource source =  ParseShader("Basic.shader");
		std::cerr << source.vertexSource << std::endl;
		std::cerr << source.fragmentSource<< std::endl;

		// create vertex shader
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		const GLchar* V_ShaderCode[] = {
			"#version 140\n"
			"in vec2 vertexPos2D;\n"
			"uniform mat4 transform;\n"
			"uniform mat4 view;\n"
			"uniform mat4 projection;\n"
			"void main()\n"
			"{\n"
			"vec4 v = vec4(vertexPos2D.x, vertexPos2D.y, 0, 1);\n"
			"v = projection * view * transform * v;\n"
			"gl_Position = vec4(vertexPos2D.x, vertexPos2D.y, 0,1);\n"
			//"gl_Position = v;\n"
			"}\n"
		};

		// Copy the source to gl, ready for compilation
		glShaderSource(vertexShader, 1, V_ShaderCode, NULL);
		// compile the code
		GLCALL(glCompileShader(vertexShader));
		// check for errors - presume shader did not compile
		GLint isFShaderCompiledOK = GL_FALSE;
		// get  compilation status from openGL
		GLCALL(glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isFShaderCompiledOK));

		if (isFShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile vertex shader" << std::endl;
			displayShaderCompilerError(vertexShader);
			return;
		}

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		const GLchar* F_ShaderCode[] = {
			"#version 140\n"
			"out vec4 fragmentColour;\n"
			"void main()\n"
			"{\n"
			"fragmentColour = vec4(1.0, 0.0, 0.0, 1.0);\n"
			"}\n"
		};

		GLCALL(glShaderSource(fragmentShader, 1, F_ShaderCode, NULL));
		GLCALL(glCompileShader(fragmentShader));
		
		GLCALL(GLint isShaderCompiledOK = GL_FALSE);
		
		GLCALL(glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isShaderCompiledOK));

		if (isShaderCompiledOK != GL_TRUE) {
			std::cerr << "Unable to compile the fragment shader" << std::endl;

			displayShaderCompilerError(fragmentShader);
			return;
		}

		// create program and store it`s ID 
		programID = glCreateProgram();

		// attach shader to program obj 
		GLCALL(glAttachShader(programID, vertexShader));
		GLCALL(glAttachShader(programID, fragmentShader));

		// link program to create executable program and use to render objects
		// this will exists in graphics memory 
		GLCALL(glLinkProgram(programID));

		GLint isProgramLinked = GL_FALSE;

		// shader error check 
		GLCALL(glGetProgramiv(programID, GL_LINK_STATUS, &isProgramLinked));
		if (isProgramLinked != GL_TRUE) {
			std::cerr << "Failed to link program" << std::endl;
		}

		vertexPos2DLocation = glGetAttribLocation(programID, "vertexPos2D");

		if (vertexPos2DLocation == -1) {
			std::cerr << "Problem getting vertexPos2D" << std::endl;
		}

		transformUniformId = glGetUniformLocation(programID, "transform");
		projectionUniformId = glGetUniformLocation(programID, "projection");
		viewUniformId = glGetUniformLocation(programID, "view");
	
		// create vertex buffer object
		GLCALL(glGenBuffers(1, &vboTraiangle));
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vboTraiangle));
		
		// transfer vertecies to graphic memory 
		GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW));
	}

	void Renderer::update()
	{
	}
	
	void Renderer::draw(Camera* cam)
	{
	
		GLCALL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW));
				
		if(vertexData[0] <= 0) 
			vertexData[0] += 0.01;
		if (vertexData[5] >= -1)
			vertexData[5] -= 0.01;		

		glm::mat4 transformationMat = glm::mat4(1.0f);

		transformationMat = glm::translate(transformationMat, glm::vec3(pos_x, pos_y, pos_z));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_x), glm::vec3(1.0f, 0.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_y), glm::vec3(0.0f, 1.0f, 0.0f));
		transformationMat = glm::rotate(transformationMat, glm::radians(rot_z), glm::vec3(0.0f, 0.0f, 1.0f));
		transformationMat = glm::scale(transformationMat, glm::vec3(scale_x, scale_y, scale_z));

		glm::mat4 viewMat = cam->getViewMatrix();
		glm::mat4 projectionMat = cam->getProjectionMatrix();

		GLCALL(glUseProgram(programID));
		
		GLCALL(glBindBuffer(GL_ARRAY_BUFFER, vboTraiangle));
		
		// functions transfer ram to graphics ram 
		GLCALL(glUniformMatrix4fv(transformUniformId, 1, GL_FALSE, glm::value_ptr(transformationMat)));
		GLCALL(glUniformMatrix4fv(viewUniformId, 1, GL_FALSE, glm::value_ptr(viewMat)));
		
		GLCALL(glUniformMatrix4fv(projectionUniformId, 1, GL_FALSE, glm::value_ptr(projectionMat)));
		
		GLCALL(glEnableVertexAttribArray(vertexPos2DLocation));
		
		GLCALL(glVertexAttribPointer(vertexPos2DLocation, 2, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 2, nullptr));

		glDrawArrays(GL_TRIANGLES, 0, 3);

		GLCALL(glDisableVertexAttribArray(vertexPos2DLocation));

		//glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLCALL(glUseProgram(0));
	}

	void Renderer::destroy()
	{
		// Release object allocated for program and vertex buffer obj 
		GLCALL(glDeleteProgram(programID));
		GLCALL(glDeleteBuffers(1, &vboTraiangle));
	}

}