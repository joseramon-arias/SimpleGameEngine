#pragma once

#include <GL\glew.h>
#include <string>
#include "CustomMath.h"

namespace Engine
{
	class Shader
	{
	public:
		Shader();
		~Shader();

		// Load the vertex\fragment shaders with the given names
		bool Load(const std::string& vertName, const std::string& fragName);
		void Unload();
		// Set this as the active shader program
		void SetActive();
	private:
		//Tries to compile the specified shader
		bool CompileShader(const std::string& fileName,
						   GLenum shaderType,
						   GLuint& outShader);

		// Tests whether shader compiled successfully
		bool IsCompiled(GLuint shader);
		// Tests whether vertex/fragment programs link
		bool IsValidProgram();

		// Store the shader object IDs
		GLuint m_VertexShader;
		GLuint m_FragShader;
		GLuint m_ShaderProgram;
	};
}