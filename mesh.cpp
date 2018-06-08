//
// Created by pluto on 08.06.18.
//

#include "mesh.h"

Mesh::Mesh(Vertex* vertices, unsigned int numberVertices) {
    mDrawCount = numberVertices;
    glGenVertexArrays(1, &mVertexArrayObject);
}
Mesh::~Mesh(){
    glDeleteVertexArrays(1, &mVertexArrayObject);
};