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
#include <string>
#include <vector>
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

float drunk_level = 0;

const int n = 16;
Object objectsArray[n];

int animationStep = 0, animated_bottle_id=-1;
float new_y, frames=200;

double fRand(double fMin, double fMax){
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
	glClearColor(1,1,1,1);
	glEnable(GL_DEPTH_TEST);
	glfwSetFramebufferSizeCallback(window,windowResize);

    for(int i=0;i<n;i++){
        objectsArray[i].shaderProgram=new ShaderProgram("vshader.glsl",NULL,"fshader.glsl");
        objectsArray[i].prepareObject(objectsArray[i].shaderProgram);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos){
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

    float sensitivity = 0.1;

    if(drunk_level>0) sensitivity = fRand(-1*drunk_level*0.2,drunk_level*0.2)+0.5*(1+drunk_level);
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

void animate(int id){
    if(animationStep==1){
            new_y = objectsArray[id].y_current + (cameraPos.y-objectsArray[id].y_start)/frames;
            objectsArray[id].setPosition(objectsArray[id].x_current,new_y,objectsArray[id].z_current);
            objectsArray[id].rotateM(objectsArray[id].angle_current+PI/2/frames,vec3(-1,0,0));
            if(new_y>cameraPos.y){
                animationStep++;
                drunk_level++;
            }
        }
    else if(animationStep==2){
        new_y = objectsArray[id].y_current - (cameraPos.y-objectsArray[id].y_start)/frames;
        if(new_y<objectsArray[id].y_start){
            new_y=objectsArray[id].y_start;
            animationStep=0;
        }
        objectsArray[id].setPosition(objectsArray[id].x_current,new_y,objectsArray[id].z_current);
        objectsArray[id].rotateM(objectsArray[id].angle_current-PI/2/frames,vec3(-1,0,0));
    }
}

void drawScene(GLFWwindow* window) {

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);

	glm::mat4 P = glm::perspective(50 * PI / 180, aspect, 1.0f, 50.0f);
    glm::mat4 V = glm::lookAt(cameraPos,cameraPos+cameraFront, cameraUp);

    if(animationStep)
        animate(animated_bottle_id);

    for(int i=0;i<n;i++){
        objectsArray[i].drawObject(objectsArray[i].shaderProgram,P,V,i,cameraPos);
    }
	glfwSwapBuffers(window);
}

int getBottleId(){
    float error=999999,min_error=999999;
    int id=-1;
    for(int i=0;i<n;i++){
        error=abs(objectsArray[i].x_start-cameraPos.x)+abs(objectsArray[i].z_start-cameraPos.z);
        if(error<min_error && objectsArray[i].is_pickable){
            min_error=error;
            id=i;
        }
    }
    return min_error > 5 ? -1 : id;
}

void tryPickUp(){
    int id = getBottleId();
    if(id!=-1){
        animated_bottle_id=id;
        animationStep = 1;
    }
}

void processInput(GLFWwindow *window){
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if(!animationStep){
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
            tryPickUp();

        float cameraSpeed = 15 * deltaTime;
        vec3 temp = cameraPos;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            cameraPos += cameraSpeed * vec3(1.0f,0.0f,1.0f)*cameraFront;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            cameraPos -= cameraSpeed * vec3(1.0f,0.0f,1.0f)*cameraFront;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            cout<<cameraPos.x<<" "<<cameraPos.z<<endl;

        if(abs(cameraPos.x)>16 || abs(cameraPos.z)>16)
            cameraPos = temp;
    }
}

int main(void){
    srand( time( NULL ) );
    vector <string> tex;
    string temp="glassX.png";
    for(int i=0;i<=9;i++){
        temp[5] = i + '0';
        tex.push_back(temp);
    }
    cout<<tex[0]<<endl;
    for(int i=0;i+6<11;i++){
        objectsArray[i+6].Create("bottle.obj", tex[rand()%tex.size()],0+i*3,10,18,0.35,0.35,0.35,true);
    }
    for(int i=0;i+11<n;i++){
        objectsArray[i+11].Create("kieliszek.obj", tex[rand()%tex.size()],-15+3*i,7,18,0.3,0.3,0.3, true);
    }

    objectsArray[0].Create("bar.obj","czarny.png",0,0,18,2,6,4);
    objectsArray[0].rotateM(PI/2,vec3(0,1,0));

    objectsArray[1].Create("wall.obj","ctroj.png",0,0,0,10,10,10);//podloga

    objectsArray[3].Create("wall.obj","pink.png",0,5,-20,10,5,5);//sciana
    objectsArray[3].rotateM(PI/2,vec3(1,0,0));

    objectsArray[2].Create("wall.obj","pink.png",0,5,20,10,5,5);//sciana
    objectsArray[2].rotateM(PI/2,vec3(-1,0,0));

    objectsArray[4].Create("wall.obj","glass7.png",20,5,0,10,5,5);//sciana
    objectsArray[4].rotateM(PI/2,vec3(-1,0,0));
    objectsArray[4].rotateM(PI/2,vec3(0,0,1));

    objectsArray[5].Create("wall.obj","glass7.png",-20,5,0,10,5,5);//sciana
    objectsArray[5].rotateM(PI/2,vec3(-1,0,0));
    objectsArray[5].rotateM(PI/2,vec3(0,0,-1));

    for(int i=0;i<n;i++){
        objectsArray[i].vertexCount = objectsArray[i].v_vertices.size();
    }

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

	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}
	initOpenGLProgram(window);
	glfwSetTime(0);
	while (!glfwWindowShouldClose(window))
	{
	    if(drunk_level>0)drunk_level-=1/frames/10;
        processInput(window);
        deltaTime = glfwGetTime();
		glfwSetTime(0);
		drawScene(window);
		glfwPollEvents();
	}

    for(int i=0;i<n;i++){
        objectsArray[i].freeOpenGLProgram();
    }

	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}
