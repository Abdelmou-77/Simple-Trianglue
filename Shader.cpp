// Shader.cpp
//
// See header file for details

#include "Shader.h"

#include <cstdio>
#include <cstdlib>
#include <SDL.h>
#include <SDL_opengles2.h>

#ifdef _MSC_VER
#pragma warning(disable:4996) // Allows us to use the portable fopen() function
without warnings
#endif

/** Gets the file's length.
*
* @param file the file
*
* @return size_t the file's length in bytes
*/
static size_t fileGetLength(FILE *file) {
	size_t length;
	size_t currPos = ftell(file);
	fseek(file, 0, SEEK_END);
	length = ftell(file);
	
	// Return the file to its previous position
	fseek(file, currPos, SEEK_SET);
	
	return length;
}

/** Loads and compiles a shader from a file.
*
* This will print any errors to the console.
*
* @param filename the shader's filename
* @param shaderType the shader type (e.g., GL_VERTEX_SHADER)
*
* @return GLuint the shader's ID, or 0 if failed
*/
static GLuint shaderLoad(const char *filename, GLenum shaderType) {
	FILE *file = fopen(filename, "r");
	if (!file) {
		SDL_Log("Can't open file: %s\n", filename);
		return 0;
	}
	size_t length = fileGetLength(file);
	
	// Alloc space for the file (plus '\0' termination)
	GLchar *shaderSrc = (GLchar*)calloc(length + 1, 1);
	if (!shaderSrc) {
		SDL_Log("Out of memory when reading file: %s\n", filename);
		fclose(file);
		file = NULL;
		return 0;
	}
	fread(shaderSrc, 1, length, file);
	
	// Done with the file
	fclose(file);
	file = NULL;
	
	// Create the shader
	GLuint shader = glCreateShader(shaderType);
	glShaderSource(shader, 1, (const GLchar**)&shaderSrc, NULL);
	free(shaderSrc);
	shaderSrc = NULL;
	
	// Compile it
	glCompileShader(shader);
	GLint compileSucceeded = GL_FALSE;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileSucceeded);
	if (!compileSucceeded) {
		// Compilation failed. Print error info
		SDL_Log("Compilation of shader %s failed:\n", filename);
		GLint logLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
		GLchar *errLog = (GLchar*)malloc(logLength);
		if (errLog) {
			glGetShaderInfoLog(shader, logLength, &logLength, errLog);
			SDL_Log("%s\n", errLog);
			free(errLog);
		}
		else {
			SDL_Log("Couldn't get shader log; out of memory\n");
		}
		glDeleteShader(shader);
		shader = 0;
	}
	
	return shader;
}