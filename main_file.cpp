#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
#include <cstdlib>
#include <ctime>
#include <vector>
#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <stdlib.h>
#include <stdio.h>
#include "lodepng.h"
#include "shaderprogram.h"
#include "objLoader.h"
#include "model.cpp"

using namespace std;
using namespace glm;

const unsigned int SCR_WIDTH = 500;
const unsigned int SCR_HEIGHT = 500;

const float PI = 3.141592653589793f;

glm::vec3 cameraPos   = glm::vec3(0.0f, 15.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

float bottle_y = 1.0f,new_y=15.0f,frames=600,current_y=bottle_y;
float goal_angle = PI/1.5f, start_angle = 0, current_angle = start_angle;

bool firstMouse = true;
float yaw1   = -90.0f;
float pitch1 =  0.0f;
float lastX =  500.0f / 2.0;
float lastY =  500.0 / 2.0;
float fov   =  45.0f;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float speed_x = 0;
float speed_y = 0;
float eye_x = 0;
float eye_y = 0;

float aspect=1;


Object butelka1("bottle.obj", "glass.png");
Object butelka2("bottle.obj", "bricks.png");

double fRand(double fMin, double fMax)
{
    double f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

void windowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if (height!=0) {
        aspect=(float)width/(float)height;
    } else {
        aspect=1;
    }
}

void initOpenGLProgram(GLFWwindow* window) {
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window,windowResize);

	butelka1.shaderProgram=new ShaderProgram("vshader.glsl",NULL,"fshader.glsl");
	butelka2.shaderProgram=new ShaderProgram("vshader.glsl",NULL,"fshader.glsl");

	butelka1.prepareObject(butelka1.shaderProgram);
	butelka2.prepareObject(butelka2.shaderProgram);

}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
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

    float sensitivity = 0.1*fRand(-1,3);
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw1 += xoffset;
    pitch1 += yoffset;

    if (pitch1 > 89.0f)
        pitch1 = 89.0f;
    if (pitch1 < -89.0f)
        pitch1 = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    front.y = sin(glm::radians(pitch1));
    front.z = sin(glm::radians(yaw1)) * cos(glm::radians(pitch1));
    cameraFront = glm::normalize(front);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 15 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * vec3(1.0f,0.0f,1.0f)*cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * vec3(1.0f,0.0f,1.0f)*cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void drawScene(GLFWwindow* window) {

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glm::mat4 P = glm::perspective(50 * PI / 180, aspect, 1.0f, 50.0f);
	//cout<<cameraPos.x<<" "<<cameraPos.y<<" "<<cameraPos.y<<" "<<cameraFront.x<<" "<<cameraFront.y<<" "<<cameraFront.z<<" "<<cameraUp.x<<" "<<cameraUp.y<<" "<<cameraUp.z<<" "<<endl;
    glm::mat4 V = glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUp);


	butelka1.drawObject(butelka1.shaderProgram,P,V,0);

	butelka2.drawObject(butelka2.shaderProgram,P,V,1);
    if(current_y<new_y)current_y+=(new_y-bottle_y)/frames;
    if(current_angle<goal_angle)current_angle+=(goal_angle-start_angle)/frames;

	glfwSwapBuffers(window);

}

int main(void)
{
    srand( time( NULL ) );

    butelka1.vertexCount = butelka1.v_vertices.size();
    butelka2.vertexCount = butelka2.v_vertices.size();

	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(1024, 768, "OpenGL", NULL, NULL);

	if (!window)
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window);
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window))
	{
        processInput(window);
        deltaTime = glfwGetTime();
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}

	butelka1.freeOpenGLProgram();
    butelka2.freeOpenGLProgram();

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
