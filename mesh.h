//
// Created by pluto on 08.06.18.
//

#ifndef PROJECT_MESH_H
#define PROJECT_MESH_H

#include <glm/glm.hpp>
#include <GL/glew.h>


struct Vertex
{
public:
    Vertex(const glm::vec3& pos, const glm::vec2& texCoord, const glm::vec3& normal)
    {
        this->position = position;
        this->texCoord = texCoord;
        this->normal = normal;
    }

    glm::vec3* GetPosition() { return &position; }
    glm::vec2* GetTexCoord() { return &texCoord; }
    glm::vec3* GetNormal() { return &normal; }

private:
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
};

class Mesh {
public:
    Mesh(Vertex* vertices, unsigned int numberVertices);
    void draw();
    virtual ~Mesh();
protected:
private:
    enum{
        POSITION_VERTEX_BUFFER,
        NUMBER_BUFFERS
};
    GLuint mVertexArrayObject;
    GLuint mVertexArrayBuffers[NUMBER_BUFFERS];
    unsigned int mDrawCount;
};


#endif //PROJECT_MESH_H
