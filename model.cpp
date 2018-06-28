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

using namespace std;
using namespace glm;

class Object{
public:
    GLuint vao, bufVertices, bufNormals, bufTexCoords, texture;
    ShaderProgram *shaderProgram;
    vector<glm::vec4> v_vertices, v_normals;
    vector<glm::vec2> v_uvs;
    vector<float> v_colors;
    float *vertices, *normals, *texCoords;
    int vertexCount;
    bool wynik, is_pickable=0;
    char* texturename;
    glm::mat4 M;
    float x_start = 0.0f, y_start = 0.0f, z_start = 0.0f;
    float x_current = 0.0f, y_current =0.0f, z_current =0.0f;
    float angle_start = 0.0f, angle_current = 0.0f;

    Object(){};
    void Create(const char *pathname, char* filename,float x = 0.0f, float y = 0.0f, float z = 0.0f, float scalex=1,float scaley=1,float scalez=1, bool pickable = false){
        M = glm::mat4(1.0f);
        x_start = x;
        y_start = y;
        z_start = z;

        x_current = x_start;
        y_current = y_start;
        z_current = z_start;

        M = glm::translate(M,vec3(x,y,z));

        is_pickable = pickable;

        load(pathname,scalex,scaley,scalez);
        texturename=filename;
    };

    void load(const char* pathname, float scalex, float scaley, float scalez){
        wynik = loadOBJ(pathname,v_vertices,v_uvs,v_normals,scalex,scaley,scalez);
        vertices = static_cast<float*>(glm::value_ptr(v_vertices.front()));
        normals = static_cast<float*>(glm::value_ptr(v_normals.front()));
        texCoords = static_cast<float*>(glm::value_ptr(v_uvs.front()));
    }

    void prepareObject(ShaderProgram *shaderProgram) {
        bufVertices=makeBuffer(vertices, vertexCount, sizeof(float)*4);
        bufNormals=makeBuffer(normals, vertexCount, sizeof(float)*4);
        bufTexCoords=makeBuffer(texCoords, vertexCount, sizeof(float)*2);
        glGenVertexArrays(1,&vao);

        glBindVertexArray(vao);

        assignVBOtoAttribute(shaderProgram,"vertex",bufVertices,4);
        assignVBOtoAttribute(shaderProgram,"normal",bufNormals,4);
        assignVBOtoAttribute(shaderProgram,"texCoord0",bufTexCoords,2);
        glBindVertexArray(0);

        glGenTextures(1,&texture);
        readTexture(texturename, texture);
    }

    void drawObject(ShaderProgram *shaderProgram, mat4 mP, mat4 mV,int n, vec3 cameraPos) {
        shaderProgram->use();

        glUniformMatrix4fv(shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
        glUniformMatrix4fv(shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
        glUniformMatrix4fv(shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(M));
	   // glUniform4fv(shaderProgram->getUniformLocation("lp"),1,glm::value_ptr(vec4(cameraPos.x*0.5,cameraPos.y,cameraPos.z*0.5,-1)));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture);

        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES,0,vertexCount);
        glBindVertexArray(0);
    }

    void setPosition(float x, float y , float z){
        x_current = x;
        y_current = y;
        z_current = z;
        M = glm::mat4(1.0f);
        M = glm::translate(M,vec3(x,y,z));
    }

    void rotateM(float angle, vec3 vec3v){
        angle_current=angle;
        M = glm::rotate(M, angle, vec3v);
    }

    GLuint readTexture(char* filename, GLuint texture) {
        glActiveTexture(GL_TEXTURE0);
        std::vector<unsigned char> image;
        unsigned width, height;
        unsigned error = lodepng::decode(image, width, height, filename);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
        GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
        return texture;
    }

    GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
        GLuint handle;
        glGenBuffers(1,&handle);
        glBindBuffer(GL_ARRAY_BUFFER,handle);
        glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);
        return handle;
    }

    void assignVBOtoAttribute(ShaderProgram *shaderProgram,const char* attributeName, GLuint bufVBO, int vertexSize) {
        GLuint location=shaderProgram->getAttribLocation(attributeName);
        glBindBuffer(GL_ARRAY_BUFFER,bufVBO);
        glEnableVertexAttribArray(location);
        glVertexAttribPointer(location,vertexSize,GL_FLOAT, GL_FALSE, 0, NULL);
    }

    void freeOpenGLProgram() {
        delete shaderProgram;
        glDeleteVertexArrays(1,&vao);
        glDeleteBuffers(1,&bufVertices);
        glDeleteBuffers(1,&bufNormals);
        glDeleteBuffers(1,&bufTexCoords);
    }
};

