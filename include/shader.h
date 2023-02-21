#ifndef SHADER_H
#define SHADER_H

#include "libs.h"


class Shader {
public:
	unsigned int id;
	
	Shader(const char* vertexShaderPath, const char* fragmentShaderPath);
	void activate();

	std::string loadShaderSrc(const char* filepath);
	GLuint compileShader(const char* filepath, GLenum type);

	void setObjectType(int type);
	void setPosition(int position);
};

class Texture {
private:
	unsigned int id;
	std::string filePath;
	unsigned char* localBuffer;
	int width, height, bpp;
public:
	Texture(const std::string& path);
	~Texture();

	void bind(unsigned int slot = 0);
	void unbind();

	inline int GetWidth() const { return width; }
	inline int Getheight() const { return height; }
};
#endif