#include "../include/shader.h"

Shader::Shader(const char* vertexShaderPath, const char* fragmentShaderPath) {
	int success;
	char infoLog[512];
	GLuint vertexShader = compileShader(vertexShaderPath, GL_VERTEX_SHADER);
	GLuint fragShader = compileShader(fragmentShaderPath, GL_FRAGMENT_SHADER);

	id = glCreateProgram();
	glAttachShader(id, vertexShader);
	glAttachShader(id, fragShader);
	glLinkProgram(id);

	//catch error
	glGetProgramiv(id, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(id, 512, NULL, infoLog);
		std::cout << "Linking error:" << std::endl << infoLog << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

void Shader::activate() {
	glUseProgram(id);
}


GLuint Shader::compileShader(const char* filepath, GLenum type) {
	int success;
	char infoLog[512];
	GLuint ret = glCreateShader(type);
	std::string shaderSrc = loadShaderSrc(filepath);
	const GLchar* shader = shaderSrc.c_str();
	glShaderSource(ret, 1, &shader, NULL);
	glCompileShader(ret);
	glGetShaderiv(ret, GL_COMPILE_STATUS, &success);

	// catch error
	if (!success) {
		glGetShaderInfoLog(ret, 512, NULL, infoLog);
		std::cout << "Error frag shader comp.:" << std::endl << infoLog << std::endl;
	}

	return ret;
}

std::string Shader::loadShaderSrc(const char* filename) {
	std::ifstream file;
	std::stringstream buf;

	std::string ret = "";

	file.open(filename);
	if (file.is_open()) {
		buf << file.rdbuf();
		ret = buf.str();
	}
	else {
		std::cout << "Could not open " << filename << std::endl;
	}

	file.close();

	return ret;
}

void Shader::setObjectType(int type) {
	glUniform1i(glGetUniformLocation(id, "OBJECT"), type);
}

void Shader::setPosition(int position) {
	glUniform1f(glGetUniformLocation(id, "POSITION"), position);
}

Texture::Texture(const std::string& path)
	: filePath(path), localBuffer(nullptr), width(0), height(0), bpp(0) 
{
	stbi_set_flip_vertically_on_load(1);
	localBuffer = stbi_load(path.c_str(), &width, &height, &bpp, 4);

	glGenTextures(1, &id);
	glBindTexture(GL_TEXTURE_2D, id);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, localBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (localBuffer)
		stbi_image_free(localBuffer);
}
Texture::~Texture() {
	glDeleteTextures(1, &id);
}

void Texture::bind(unsigned int slot) {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, id);
	}
void Texture::unbind() {
	glBindTexture(GL_TEXTURE_2D, 0);
	}

