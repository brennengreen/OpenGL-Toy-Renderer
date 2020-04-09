#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "Shader.h"

class Geometry {
public:
	std::vector<float> mesh;
	std::vector<GLuint> indices;
	GLuint VAO;
	Shader shader;

	Geometry(std::vector<float> vertices, std::vector<GLuint> indices);
	void Draw();
	void setShader(const char* vs, const char* fs);

private:
	GLuint VBO, EBO;

	void setup();

};


#endif // !GEOMETRY_H
