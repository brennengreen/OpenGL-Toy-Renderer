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


	// SHADERS ///////////////////////////////////////////////////////////////////////////////////
	Shader meshShader("mesh.vert", "mesh.frag");
	//////////////////////////////////////////////////////////////////////////////////////////////

	// MODELS  ///////////////////////////////////////////////////////////////////////////////////
	Model meshModel((char*)("Models/Planet/planet.obj"));
	Model planeModel((char*)("Models/Plane/plane.obj"));
	//////////////////////////////////////////////////////////////////////////////////////////////

	// Main Loop
	glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);

	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window)) {
		// Important Time Keeping
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		// Process Input
		process_input(window);
		// Render
		glClearColor(0.66f, 0.79f, 0.85f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		model = glm::mat4(1.0f);

		meshShader.use();
		model = glm::mat4(1.0f);

		//model = glm::translate(model, glm::vec3(0, cos(currentFrame) * 0.2, 0));
		model = glm::rotate(model, glm::radians(cos(currentFrame) * 20.0f + 5.0f), glm::vec3(1, 0, 1));

		meshShader.setMat4("model", model);
		meshShader.setVec3("lightColor", lightColor);
		meshShader.setVec3("lightPos", lightPos);
		meshShader.setMat4("projection", projection);
		meshShader.setMat4("view", view);
		meshShader.setVec3("viewPos", camera.Position);
		meshShader.setVec3("objColor", glm::vec3(0.45f, 0.745f, 0.89f));
		meshShader.setFloat("ambientStrength", 0.25);
		meshModel.Draw(meshShader);

		// Plane
		meshShader.setVec3("objColor", glm::vec3(0.65f, 0.65f, 0.89f));
		meshShader.setFloat("ambientStrength", 0.5);
		model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(0, -1.0f, 0));
		model = glm::scale(model, glm::vec3(5.0f, 0.1f, 5.0f));
		meshShader.setMat4("model", model);
		planeModel.Draw(meshShader);

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