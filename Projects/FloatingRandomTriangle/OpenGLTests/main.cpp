#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <cmath>

#include <chrono>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


using namespace std;

//window dimensions
const GLint WIDTH = 800, HEIGHT = 600;

GLuint VAO, VBO, shader, uniformModel;

float xPos = 0;
float yPos = 0;
float rot = 0;
float scale = 0.5;
float xSpeed = 0;
float ySpeed = 0;
float rotSpeed = 0;
float scaleChange = 0;



// Vertex Shader
static const char* vShader = "          \n\
#version 330													\n\
\n\
uniform mat4 model;\n\
layout (location = 0) in vec3 pos;\n\
out vec4 vColor;\n\
void main(){\n\
	gl_Position = model * vec4(pos.x, pos.y, pos.z, 1.0);\n\
	vec3 tempPos = vec3(gl_Position.x, gl_Position.y, gl_Position.z);\n\
	vColor = vec4(clamp(tempPos, 0.0f, 1.0f), 1.0);\n\
}\n\
";

//Fragment Shader
static const char* fShader = "\n\
#version 330													\n\
\n\
in vec4 vColor;\n\
out vec4 color;\n\
void main(){\n\
	color = vColor;\n\
}\n\
";


void CreateTriangle() {
	GLfloat vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		0.0f, 1.0f, 0.0f
	};

	//Generate and bind Vertex Array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate and bind Vetex Buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Put vertices into buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	
	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	//
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType) {
	GLuint theShader = glCreateShader(shaderType);
	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar elog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result) {
		glGetShaderInfoLog(theShader, sizeof(elog), NULL, elog);
		printf("Error compiling the %d shader: '%s'\n", shaderType, elog);

	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders() {
	shader = glCreateProgram();
	if (!shader) {
		printf("Error Initialising GLFW");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar elog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
		printf("Error linking program: '%s'\n", elog);

	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result) {
		glGetProgramInfoLog(shader, sizeof(elog), NULL, elog);
		printf("Error validating program: '%s'\n", elog);

	}

	uniformModel = glGetUniformLocation(shader, "model");
}

int main() {

	if (!glfwInit()) {
		printf("GLFW initialization failed!");
			glfwTerminate();
			return 1;
	}

	//Setup GLFW window properties
	//OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Core profile = No Backward Compatibility
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Allow Forward Compability
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* mainWindow = glfwCreateWindow(WIDTH, HEIGHT, "Test window", NULL, NULL);
	if (!mainWindow) {
		printf("GLFW window creation failed!");
		glfwTerminate();
		return 1;
	}

	//Get Buffer Size Information
	int bufferWidth, bufferHeight;
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	//Set Context for glew to use
	glfwMakeContextCurrent(mainWindow);

	// Allow modern extension futures
	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK) {
		printf("GLEW initialisation failed!");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();
		return 1;
	}

	//Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	CreateTriangle();
	CompileShaders();

	// initial time
	auto initial_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
	auto final_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();

	// initialize movement
	srand(time(NULL));
	xSpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
	ySpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
	rotSpeed = ((float(rand()) / float((RAND_MAX))) * 2 - 1) * M_PI;
	scaleChange = 0.1f * ((float(rand()) / float((RAND_MAX))) * 2 - 1);
	
	// Loop until window closed
	while (!glfwWindowShouldClose(mainWindow)) {
		//Get and handle user input events
		glfwPollEvents();

		//Time computation
		final_time = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now().time_since_epoch()).count();
		auto timePassed = final_time - initial_time;
		initial_time = final_time;

		//New Pos Calculation
		xPos += xSpeed * timePassed / float(1000);
		yPos += ySpeed * timePassed / float(1000);
		rot += rotSpeed * timePassed / float(1000);
		scale += scaleChange * timePassed / float(1000);

		//Check where triangle collided
		if (xPos > 1 ) {
			xSpeed = float(rand()) / float((RAND_MAX)) - 1;
			ySpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
			rotSpeed = ((float(rand()) / float((RAND_MAX))) * 2 - 1) * M_PI;
			scaleChange = 0.1f * ((float(rand()) / float((RAND_MAX))) * 2 - 1);
		}
		else if (xPos < -1) {
			xSpeed = float(rand()) / float((RAND_MAX));
			ySpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
			rotSpeed = ((float(rand()) / float((RAND_MAX))) * 2 - 1) * M_PI;
			scaleChange = 0.1f * ((float(rand()) / float((RAND_MAX))) * 2 - 1);
		}
		else if (yPos > 1) {
			xSpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
			ySpeed = float(rand()) / float((RAND_MAX)) - 1;
			rotSpeed = ((float(rand()) / float((RAND_MAX))) * 2 - 1) * M_PI;
			scaleChange = 0.1f * ((float(rand()) / float((RAND_MAX))) * 2 - 1);
		}
		else if (yPos < -1) {
			xSpeed = (float(rand()) / float((RAND_MAX))) * 2 - 1;
			ySpeed = float(rand()) / float((RAND_MAX));
			rotSpeed = ((float(rand()) / float((RAND_MAX))) * 2 - 1) * M_PI;
			scaleChange = 0.1f * ((float(rand()) / float((RAND_MAX))) * 2 - 1);
		}
		else if (scale < 0.0f) {
			scaleChange = 0.1f * float(rand()) / float(RAND_MAX);
		}
		else if (scale > 1.0f) {
			scaleChange = -0.1f * float(rand()) / float(RAND_MAX);
		}

		//Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader);

		glm::mat4 model(1.0f);

		
		model = glm::translate(model, glm::vec3((1-scale) * xPos, (1 - scale) * yPos, 0));
		model = glm::rotate(model, rot, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));

		glBindVertexArray(VAO);

		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindVertexArray(0);

		glUseProgram(0);

		glfwSwapBuffers(mainWindow);
	}


	return 0;
}