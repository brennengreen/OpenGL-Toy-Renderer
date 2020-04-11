#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <filesystem>
#include <vector>

#include "Shader.h"
#include "Geometry.h"
#include "Camera.h"
#include "Model.h"
#include "stb_image.h" 


void framebuffer_size_callback(GLFWwindow * window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void process_input(GLFWwindow* window);

// CONFIGS
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
const float CAM_SPEED = 7.5f;
const float CAM_SENSITIVITY = 0.15f;
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

glm::vec3 lightPos(1.2f, 1.0f, 5.0f);
glm::vec3 lightColor(0.90f, 0.90f, 1.0f);

int main() {
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Shaders", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// CAMERA CONFIG /////////////////////////////////////////////////////////////////////////////
	camera.MovementSpeed = CAM_SPEED;
	camera.MouseSensitivity = CAM_SENSITIVITY;
	//////////////////////////////////////////////////////////////////////////////////////////////

	// PLANE /////////////////////////////////////////////////////////////////////////////////////
	std::vector<float> vertices = { 
		// f
		-.5f,-.5f,.5f, // lbf
		 .5f,-.5f,.5f, // rbf
		-.5f, .5f,.5f, // ltf
		 .5f, .5f,.5f, // rtf

		// n
		-.5f,-.5f,-.5f, // lbn
		 .5f,-.5f,-.5f, // rbn
		-.5f, .5f,-.5f, // ltn
		 .5f, .5f,-.5f, // rtn

		// l
		-.5f,-.5f,-.5f, // lbn
		-.5f,-.5f, .5f, // lbf
		-.5f, .5f,-.5f, // ltn
		-.5f, .5f, .5f, // ltf

		// r
		 .5f,-.5f,-.5f, // rbn
		 .5f,-.5f, .5f, // rbf
		 .5f, .5f,-.5f, // rtn
		 .5f, .5f, .5f, // rtf

		// t
		-.5f, .5f,-.5f, // ltn
		-.5f, .5f, .5f, // ltf
		 .5f, .5f,-.5f, // rtn
		 .5f, .5f, .5f, // rtf

		// b
		-.5f,-.5f,-.5f, // lbn
		-.5f,-.5f, .5f, // lbf
		 .5f,-.5f,-.5f, // rbn
		 .5f,-.5f, .5f, // rbf
	};
	std::vector<GLuint> indices = { 
		1, 2, 0, 1, 3, 2, // f
		6, 5, 4, 6, 7, 5, // n
		9, 10, 8, 11, 10, 9, // l
		14, 13, 12, 14, 15, 13, // r
		17, 18, 16, 17, 19, 18, // t
		22, 21, 20, 22, 23, 21  // b
	};
	Geometry plane(vertices, indices);
	plane.setShader("mesh.vert", "mesh.frag");
	//////////////////////////////////////////////////////////////////////////////////////////////

	// SHADERS ///////////////////////////////////////////////////////////////////////////////////
	Shader meshShader("mesh.vert", "mesh.frag");
	Shader depthShader("depthShader.vert", "depthShader.frag");
	//////////////////////////////////////////////////////////////////////////////////////////////

	Model meshModel((char*)("Models/Planet/planet.obj"));

	// Main Loop
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);


	GLuint depthMapFBO;
	glGenFramebuffers(1, &depthMapFBO);

	const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;

	unsigned int depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		process_input(window);

		glClearColor(0.66f, 0.79f, 0.85f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Render depth of scene to depth map
		float near_plane = 1.0f, far_plane = 7.5f;
		glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		glm::mat4 lightSpaceMatrix = lightProjection * lightView;
		depthShader.use();
		depthShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);

		glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
		glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
		glClear(GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		
		// Reset Viewport
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		glm::mat4 model = glm::mat4(1.0f);

		meshShader.use();
		model = glm::mat4(1.0f);

		//model = glm::translate(model, glm::vec3(0, cos(currentFrame) * 0.2, 0));
		model = glm::rotate(model, glm::radians(cos(currentFrame) * 20.0f + 5.0f), glm::vec3(1, 0, 1));

		meshShader.setMat4("model", model);
		meshShader.setVec3("lightColor", lightColor);
		meshShader.setVec3("lightPos", lightPos);
		meshShader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(meshShader.ID, "shadowMap"), depthMap);
		meshShader.setMat4("projection", projection);
		meshShader.setMat4("view", view);
		meshShader.setVec3("viewPos", camera.Position);
		meshShader.setVec3("objColor", glm::vec3(0.45f, 0.745f, 0.89f));
		meshShader.setFloat("ambientStrength", 0.25);
		meshModel.Draw(meshShader);

		plane.shader.use();
		plane.shader.setMat4("view", view);
		plane.shader.setMat4("projection", projection);
		plane.shader.setVec3("lightColor", lightColor);
		plane.shader.setVec3("lightPos", lightPos);
		plane.shader.setMat4("lightSpaceMatrix", lightSpaceMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depthMap);
		glUniform1i(glGetUniformLocation(plane.shader.ID, "shadowMap"), depthMap);
		plane.shader.setVec3("viewPos", camera.Position);
		plane.shader.setVec3("objColor", glm::vec3(0.65f, 0.65f, 0.89f));
		plane.shader.setFloat("ambientStrength", 0.5);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -1.0f, 0));
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 5.0f));
		plane.shader.setMat4("model", model);
		plane.Draw();



		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwTerminate();
}

void renderScene() {

}


void framebuffer_size_callback(GLFWwindow * window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow * window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow * window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}


void process_input(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camera.MovementSpeed = CAM_SPEED * 0.2;
	else
		camera.MovementSpeed = CAM_SPEED;

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
}