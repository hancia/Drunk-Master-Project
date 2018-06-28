#ifndef SHADERPROGRAM_H
#define SHADERPROGRAM_H

#include "GL/glew.h"
#include "stdio.h"

class ShaderProgram {
private:
	GLuint shaderProgram; //Uchwyt reprezentuj¹cy program cieniujacy
	GLuint vertexShader; //Uchwyt reprezentuj¹cy vertex shader
	GLuint geometryShader; //Uchwyt reprezentuj¹cy geometry shader
	GLuint fragmentShader; //Uchwyt reprezentuj¹cy fragment shader
	char* readFile(const char* fileName); //metoda wczytuj¹ca plik tekstowy do tablicy znaków
	GLuint loadShader(GLenum shaderType,const char* fileName); //Metoda wczytuje i kompiluje shader, a nastêpnie zwraca jego uchwyt
public:
	ShaderProgram(const char* vertexShaderFile,const char* geometryShaderFile,const char* fragmentShaderFile);
	~ShaderProgram();
	void use(); //W³¹cza wykorzystywanie programu cieniuj¹cego
	GLuint getUniformLocation(const char* variableName); //Pobiera numer slotu zwi¹zanego z dan¹ zmienn¹ jednorodn¹
	GLuint getAttribLocation(const char* variableName); //Pobiera numer slotu zwi¹zanego z danym atrybutem
};



#endif
