#define _USE_MATH_DEFINES

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <vector>

#include <cmath>

#include <chrono>
#include <time.h>

#include <GL/glew.h>
#include <GL/GLU.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Points.h"
#include "Shader.h"
#include "Window.h"

#include "PlaneSweepTriangulation.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


using namespace std;

//window dimensions
const GLint WIDTH = 800, HEIGHT = 800;

const int PLANE_SWEEP_TRIANGULATION = 2;
const int RANDOMIZED_INCREMENTAL_TRIANGULATION = 1;

Window mainWindow;
vector<Shader*> shaderList;




// Vertex Shader
static const char* vShader = "Shaders/shader.vert";

//Fragment Shader
static const char* fShader = "Shaders/shader.frag";

void CreateShader() {
	Shader* shader1 = new Shader();
	shader1->CreateFromFiles(vShader, fShader);
	shaderList.push_back(shader1);
}

double xToLocalPosition(double x) {
	return (x / WIDTH * 2 - 1);
}

double yToLocalPosition(double y) {
	return -(y / HEIGHT * 2 - 1);
}

double scroll = 0;

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	scroll += yoffset;
}

int main() {

	mainWindow = Window(WIDTH, HEIGHT);
	mainWindow.Initialise();



	CreateShader();
	glEnable(GL_PROGRAM_POINT_SIZE);
	glfwSetInputMode(mainWindow.GetGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	glfwSetScrollCallback(mainWindow.GetGLFWwindow(), scroll_callback);

	GLuint uniformProjection = 0, uniformModel = 0, uniformMyColor = 0;

	//glm::mat4 projection = 
	glm::mat4 projection = glm::perspective(90.0f, (GLfloat)mainWindow.GetBufferWidth() / (GLfloat)mainWindow.GetBufferHeight(), 0.1f, 1000.0f);

	int numberOfPoints = 8;
	Points points(numberOfPoints);

	//Create ImGUI components
	ImGui::CreateContext();
	ImGui_ImplGlfw_InitForOpenGL(mainWindow.GetGLFWwindow(), true);
	ImGui_ImplOpenGL3_Init("#version 150");
	ImGui::StyleColorsDark();

	glm::vec3 translate = glm::vec3(0.0f, 0.0f, -1.0f);
	float rot = 0.0f;
	float scale = 1.0f;

	//cursor positions
	double xpos = 0;
	double ypos = 0;
	double prevscroll = scroll;

	//cout << translate.x << " " << translate.y << endl;


	//triangulation variables

	int curTriangulation = 0;

	points.ChangePoint(0, -0.5f, -0.5f);
	points.ChangePoint(1, 0.5f, -0.5f);
	points.ChangePoint(2, 0.0f, 0.0f);
	points.ChangePoint(3, -0.5f, 0.5f);
	points.ChangePoint(4, 0.5f, 0.5f);
	points.ChangePoint(6, -0.3f, 0.8f);
	points.ChangePoint(5, 0.3f, 0.8f);
	points.ChangePoint(7, -0.9f, 0.9f);
	PlaneSweepTriangulation triangulation(&points);

	// Loop until window closed
	while (!mainWindow.GetShouldClose()) {
		//Get and handle user input events
		glfwPollEvents();

		//Clear window
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Render Imgui
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		shaderList[0]->UseShader();
		uniformModel = shaderList[0]->GetModelLocation();
		uniformProjection = shaderList[0]->GetProjectionLocation();
		uniformMyColor = shaderList[0]->GetMyColorLocation();

		glm::mat4 model(1.0f);

		//Triangulation States


		//Handle Mouse Inputs
		int leftstate = glfwGetMouseButton(mainWindow.GetGLFWwindow(), GLFW_MOUSE_BUTTON_LEFT);
		int rightstate = glfwGetMouseButton(mainWindow.GetGLFWwindow(), GLFW_MOUSE_BUTTON_RIGHT);

		//Apply Transformations
		if (leftstate == GLFW_PRESS) {
			double curxpos;
			double curypos;
			glfwGetCursorPos(mainWindow.GetGLFWwindow(), &curxpos, &curypos);
			translate.x += (curxpos - xpos) / (WIDTH / 2);
			translate.y -= (curypos - ypos) / (HEIGHT / 2);

		}
		if (rightstate == GLFW_PRESS) {
			double curxpos;
			double curypos;
			glfwGetCursorPos(mainWindow.GetGLFWwindow(), &curxpos, &curypos);
			double prevangle = atan2(yToLocalPosition(ypos), xToLocalPosition(xpos));
			double curangle = atan2(yToLocalPosition(curypos), xToLocalPosition(curxpos));
			rot += (curangle - prevangle);
		}
		if ((scroll - prevscroll) > 0) {
			scale *= 11.0f / 10.0f;
		}
		else if ((scroll - prevscroll) < 0) {
			scale *= 9.0f / 10.0f;
		}
		prevscroll = scroll;

		glfwGetCursorPos(mainWindow.GetGLFWwindow(), &xpos, &ypos);

		model = glm::translate(model, translate);
		model = glm::rotate(model, rot, glm::vec3(0.0f, 0.0f, 1.0f));
		model = glm::scale(model, glm::vec3(scale, scale, 1.0f));

		glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
		//glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));

		{//Temp code



			ImGui::Begin("Delaunay Triangulation Interface");
			ImGui::Text("Number Of Points");
			ImGui::InputInt(" ", &numberOfPoints);
			if (numberOfPoints != points.GetPointsSize()) {
				points.CreateRandomPoints(numberOfPoints);
			}
			if (ImGui::Button("Start Randomized Incremental Triangulation")) {
				//Reset Previous Triangulation

				//Generate New Points

				//Initialize Start Triangulation
				curTriangulation = RANDOMIZED_INCREMENTAL_TRIANGULATION;
			}
			if (ImGui::Button("Start Plane Sweep Triangulation")) {
				//Reset Previous Triangulation

				//Generate New Points

				//Initialize Start Triangulation
				curTriangulation = PLANE_SWEEP_TRIANGULATION;
			}
			if (curTriangulation == RANDOMIZED_INCREMENTAL_TRIANGULATION) {
				if (ImGui::Button("Advance")) {
					//Advance in Triangulation
				}
				if (ImGui::Button("Finish")) {
					//Finish Triangulation
				}
			}
			else if (curTriangulation == PLANE_SWEEP_TRIANGULATION) {
				if (ImGui::Button("Advance")) {
					//Advance in Triangulation
				}
				if (ImGui::Button("Finish")) {
					//Finish Triangulation
				}
			}

			ImGui::End();
		}

		// Rendering
		if (curTriangulation == RANDOMIZED_INCREMENTAL_TRIANGULATION) {
			//Render Randomized Incremental Data Structure
		}
		else if (curTriangulation == PLANE_SWEEP_TRIANGULATION) {
			//Render Plane Sweep Data Structure
		}

		triangulation.Render(uniformMyColor);

		glUseProgram(0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		mainWindow.SwapBuffers();
	}

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();


	return 0;
}