#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <cmath>

#include <chrono>
#include <time.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Mesh.h"
#include "Shader.h"
#include "Window.h"


using namespace std;

//window dimensions
const GLint WIDTH = 800, HEIGHT = 800;

Window mainWindow;
vector<Mesh*> meshList;
vector<Shader*> shaderList;

float xPos = 0;
float yPos = 0;
float rot = 0;
float scale = 0.5;
float xSpeed = 0;
float ySpeed = 0;
float rotSpeed = 0;
float scaleChange = 0;



// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateSquare(unsigned int* indices, unsigned int * pointer, int index) {
	pointer[index * 6] = indices[0];
	pointer[index * 6 + 1] = indices[1];
	pointer[index * 6 + 2] = indices[2];
	pointer[index * 6 + 3] = indices[1];
	pointer[index * 6 + 4] = indices[2];
	pointer[index * 6 + 5] = indices[3];
}

void CreateCube() {
	
	GLfloat vertices[] = {
		0.5f, 0.5f, 0.5f,
		0.5f, 0.5f, -0.5f,
		0.5f, -0.5f, 0.5f,
		0.5f, -0.5f, -0.5f,
		-0.5f, 0.5f, 0.5f,
		-0.5f, 0.5f, -0.5f,
		-0.5f, -0.5f, 0.5f,
		-0.5f, -0.5f, -0.5f,
	};

	unsigned int squareIndices[] = {
		0, 1, 2, 3,
		4, 5, 6, 7,
		0, 1, 4, 5,
		2, 3, 6, 7,
		0, 2, 4, 6,
		1, 3, 5, 7
	};

	unsigned int indices[36]{};

	for (int i = 0; i < 6; i++) {
		CreateSquare(&squareIndices[4*i], indices, i);
	}

	
	Mesh* cube1 = new Mesh();
	cube1->CreateMesh(vertices, indices, sizeof(vertices)/ sizeof(vertices[0]), sizeof(indices) / sizeof(indices[0]));
	meshList.push_back(cube1);


}

void CreateShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(shader1);
}


int main() {

	mainWindow = Window(WIDTH, HEIGHT);
	mainWindow.Initialise();

	

	CreateCube();
	CreateShader();

	GLuint uniformProjection = 0, uniformModel = 0;

	glm::mat4 projection = glm::perspective(90.0f, (GLfloat)mainWindow.GetBufferWidth() / (GLfloat)mainWindow.GetBufferHeight(), 0.1f, 1000.0f);

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
	while (!mainWindow.GetShouldClose()) {
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
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();

		glm::mat4 model(1.0f);

		
		model = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.5f));
		model = glm::rotate(model, rot, glm::vec3(1.0f, 1.0f, 0.0f));
		//model = glm::scale(model, glm::vec3(scale, scale, 1.0f));
		
		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		for (int i = 0; i < meshList.size(); i++) {
			meshList[i]->RenderMesh();
		}

		glUseProgram(0);

		mainWindow.SwapBuffers();
	}


	return 0;
}