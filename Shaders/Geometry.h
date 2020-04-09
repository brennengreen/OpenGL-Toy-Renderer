#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <glad/glad.h>
#include <iostream>
#include <vector>

#include "Shader.h"

struct Vertex {
	float x;
	float y;
	float z;
	glm::vec3 normal;
};
struct Triangle {
	Vertex vertices[3];
	glm::vec3 normal;
};
struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
};

class Geometry {
public:
	std::vector<float> mesh;
	std::vector<GLuint> indices;
	std::vector<glm::vec3> normals;
	GLuint VAO;
	Shader shader;

	Geometry(std::vector<float> vertices, std::vector<GLuint> indices);
	void Draw();
	void setShader(const char* vs, const char* fs);

private:
	GLuint VBO, EBO;


	void setup();
	void getNormals();

};


#endif // !GEOMETRY_H
