/*
Niniejszy program jest wolnym oprogramowaniem; możesz go
rozprowadzać dalej i / lub modyfikować na warunkach Powszechnej
Licencji Publicznej GNU, wydanej przez Fundację Wolnego
Oprogramowania - według wersji 2 tej Licencji lub(według twojego
wyboru) którejś z późniejszych wersji.

Niniejszy program rozpowszechniany jest z nadzieją, iż będzie on
użyteczny - jednak BEZ JAKIEJKOLWIEK GWARANCJI, nawet domyślnej
gwarancji PRZYDATNOŚCI HANDLOWEJ albo PRZYDATNOŚCI DO OKREŚLONYCH
ZASTOSOWAŃ.W celu uzyskania bliższych informacji sięgnij do
Powszechnej Licencji Publicznej GNU.

Z pewnością wraz z niniejszym programem otrzymałeś też egzemplarz
Powszechnej Licencji Publicznej GNU(GNU General Public License);
jeśli nie - napisz do Free Software Foundation, Inc., 59 Temple
Place, Fifth Floor, Boston, MA  02110 - 1301  USA
*/

#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE
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
// settings
const unsigned int SCR_WIDTH = 500;
const unsigned int SCR_HEIGHT = 500;

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

bool firstMouse = true;
float yaw1   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
float pitch1 =  0.0f;
float lastX =  500.0f / 2.0;
float lastY =  500.0 / 2.0;
float fov   =  45.0f;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

const float PI = 3.141592653589793f;

GLuint normalTex; //Zmienna reprezentujaca teksturę
GLuint normalTex2; //Zmienna reprezentujaca teksturę

float speed_x = 0; // [radiany/s]
float speed_y = 0; // [radiany/s]
float eye_x = 0; // [radiany/s]
float eye_y = 0; // [radiany/s]

float aspect=1; //Stosunek szerokości do wysokości okna


//Uchwyty na VAO i bufory wierzchołków
GLuint vao;
GLuint bufVertices; //Uchwyt na bufor VBO przechowujący tablicę współrzędnych wierzchołków
GLuint bufNormals; //Uchwyt na bufor VBO przechowujący tablicę wektorów normalnych
GLuint bufTexCoords; //Uchwyt na bufor VBO przechowujący tablicę współrzędnych teksturowania
GLuint bufC2; //Uchwyt na bufor VBO przechowujący drugą kolumnę moacierzy TBN^-1


GLuint vao2;
GLuint bufVertices2; //Uchwyt na bufor VBO przechowujący tablicę współrzędnych wierzchołków
GLuint bufNormals2; //Uchwyt na bufor VBO przechowujący tablicę wektorów normalnych
GLuint bufTexCoords2; //Uchwyt na bufor VBO przechowujący tablicę współrzędnych teksturowania
GLuint bufC22; //Uchwyt na bufor VBO przechowujący drugą kolumnę moacierzy TBN^-1

//Uchwyty na shadery
ShaderProgram *shaderProgram; //Wskaźnik na obiekt reprezentujący program cieniujący.
ShaderProgram *shaderProgram2; //Wskaźnik na obiekt reprezentujący program cieniujący.

vector<glm::vec4> v_vertices;
vector<glm::vec2> v_uvs;
vector<glm::vec4> v_normals;
vector<float> v_colors;
int vertexCount;

bool wynik = loadOBJ("bottle.obj",v_vertices,v_uvs,v_normals);
float *vertices = static_cast<float*>(glm::value_ptr(v_vertices.front()));
float *normals = static_cast<float*>(glm::value_ptr(v_normals.front()));
float *texCoords = static_cast<float*>(glm::value_ptr(v_uvs.front()));

vector<glm::vec4> v_vertices2;
vector<glm::vec2> v_uvs2;
vector<glm::vec4> v_normals2;
vector<float> v_colors2;
int vertexCount2;

bool wynik2 = loadOBJ("bottle.obj",v_vertices2,v_uvs2,v_normals2);
float *vertices2 = static_cast<float*>(glm::value_ptr(v_vertices2.front()));
float *normals2 = static_cast<float*>(glm::value_ptr(v_normals2.front()));
float *texCoords2 = static_cast<float*>(glm::value_ptr(v_uvs2.front()));
GLuint *textures;


//Procedura obsługi błędów
void error_callback(int error, const char* description) {
	fputs(description, stderr);
}

//Procedura obługi zmiany rozmiaru bufora ramki
void windowResize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height); //Obraz ma być generowany w oknie o tej rozdzielczości
    if (height!=0) {
        aspect=(float)width/(float)height; //Stosunek szerokości do wysokości okna
    } else {
        aspect=1;
    }
}

GLuint readTexture0(char* filename, GLuint texture) {
  GLuint tex;
  glActiveTexture(GL_TEXTURE0);

  //Wczytanie do pamięci komputera
  std::vector<unsigned char> image;   //Alokuj wektor do wczytania obrazka
  unsigned width, height;   //Zmienne do których wczytamy wymiary obrazka
  //Wczytaj obrazek
  unsigned error = lodepng::decode(image, width, height, filename);

  //Import do pamięci karty graficznejgl
  glBindTexture(GL_TEXTURE_2D, texture);
  //Wczytaj obrazek do pamięci KG skojarzonej z uchwytem
  glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0,
    GL_RGBA, GL_UNSIGNED_BYTE, (unsigned char*) image.data());

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  return texture;
}

//Tworzy bufor VBO z tablicy
GLuint makeBuffer(void *data, int vertexCount, int vertexSize) {
	GLuint handle;

	glGenBuffers(1,&handle);//Wygeneruj uchwyt na Vertex Buffer Object (VBO), który będzie zawierał tablicę danych
	glBindBuffer(GL_ARRAY_BUFFER,handle);  //Uaktywnij wygenerowany uchwyt VBO
	glBufferData(GL_ARRAY_BUFFER, vertexCount*vertexSize, data, GL_STATIC_DRAW);//Wgraj tablicę do VBO

	return handle;
}

//Przypisuje bufor VBO do atrybutu
void assignVBOtoAttribute(ShaderProgram *shaderProgram,const char* attributeName, GLuint bufVBO, int vertexSize) {
	GLuint location=shaderProgram->getAttribLocation(attributeName); //Pobierz numer slotu dla atrybutu
	glBindBuffer(GL_ARRAY_BUFFER,bufVBO);  //Uaktywnij uchwyt VBO
	glEnableVertexAttribArray(location); //Włącz używanie atrybutu o numerze slotu zapisanym w zmiennej location
	glVertexAttribPointer(location,vertexSize,GL_FLOAT, GL_FALSE, 0, NULL); //Dane do slotu location mają być brane z aktywnego VBO
}

//Przygotowanie do rysowania pojedynczego obiektu
void prepareObject(ShaderProgram *shaderProgram) {
	//Zbuduj VBO z danymi obiektu do narysowania
	bufVertices=makeBuffer(vertices, vertexCount, sizeof(float)*4); //VBO ze współrzędnymi wierzchołków
	bufNormals=makeBuffer(normals, vertexCount, sizeof(float)*4);//VBO z wektorami normalnymi wierzchołków
	bufTexCoords=makeBuffer(texCoords, vertexCount, sizeof(float)*2);//VBO ze współrzędnymi teksturowania
	//bufC2=makeBuffer(c2, vertexCount, sizeof(float)*4);//VBO z drugą kolumną macierzy TBN^-1 dla każdego wierzchoła

	//Zbuduj VAO wiążący atrybuty z konkretnymi VBO
	glGenVertexArrays(1,&vao); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej

	glBindVertexArray(vao); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram,"vertex",bufVertices,4); //"vertex" odnosi się do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram,"normal",bufNormals,4); //"normal" odnosi się do deklaracji "in vec4 normal;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram,"texCoord0",bufTexCoords,2); //"texCoord0" odnosi się do deklaracji "in vec2 texCoord0;" w vertex shaderze
	//assignVBOtoAttribute(shaderProgram,"c2",bufC2,4); //"c2" odnosi się do deklaracji "in vec4 c2;" w vertex shaderze
	glBindVertexArray(0); //Dezaktywuj VAO
}
void prepareObject2(ShaderProgram *shaderProgram) {
	//Zbuduj VBO z danymi obiektu do narysowania
	bufVertices2=makeBuffer(vertices2, vertexCount2, sizeof(float)*4); //VBO ze współrzędnymi wierzchołków
	bufNormals2=makeBuffer(normals2, vertexCount2, sizeof(float)*4);//VBO z wektorami normalnymi wierzchołków
	bufTexCoords2=makeBuffer(texCoords2, vertexCount2, sizeof(float)*2);//VBO ze współrzędnymi teksturowania
	//bufC2=makeBuffer(c2, vertexCount, sizeof(float)*4);//VBO z drugą kolumną macierzy TBN^-1 dla każdego wierzchoła

	//Zbuduj VAO wiążący atrybuty z konkretnymi VBO
	glGenVertexArrays(1,&vao2); //Wygeneruj uchwyt na VAO i zapisz go do zmiennej globalnej

	glBindVertexArray(vao2); //Uaktywnij nowo utworzony VAO

	assignVBOtoAttribute(shaderProgram2,"vertex",bufVertices2,4); //"vertex" odnosi się do deklaracji "in vec4 vertex;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram2,"normal",bufNormals2,4); //"normal" odnosi się do deklaracji "in vec4 normal;" w vertex shaderze
	assignVBOtoAttribute(shaderProgram2,"texCoord0",bufTexCoords2,2); //"texCoord0" odnosi się do deklaracji "in vec2 texCoord0;" w vertex shaderze
	//assignVBOtoAttribute(shaderProgram,"c2",bufC2,4); //"c2" odnosi się do deklaracji "in vec4 c2;" w vertex shaderze
	glBindVertexArray(0); //Dezaktywuj VAO
}


//Procedura inicjująca
void initOpenGLProgram(GLFWwindow* window) {

	//************Tutaj umieszczaj kod, który należy wykonać raz, na początku programu************
	glClearColor(0, 0, 0, 1); //Czyść ekran na czarno
	glEnable(GL_DEPTH_TEST); //Włącz używanie Z-Bufora
	//glfwSetKeyCallback(window, key_callback); //Zarejestruj procedurę obsługi klawiatury
	glfwSetFramebufferSizeCallback(window,windowResize); //Zarejestruj procedurę obsługi zmiany rozmiaru bufora ramki

	shaderProgram=new ShaderProgram("vshader.glsl",NULL,"fshader.glsl"); //Wczytaj program cieniujący
	shaderProgram2=new ShaderProgram("vshader.glsl",NULL,"fshader.glsl"); //Wczytaj program cieniujący

	prepareObject(shaderProgram);
	prepareObject2(shaderProgram2);

    textures = new GLuint[2];
    glGenTextures(2,textures);
	readTexture0("bricks.png", textures[0]);
	readTexture0("glass.png", textures[1]);
	cout<<textures[0]<<" "<<textures[1]<<endl;
}

//Zwolnienie zasobów zajętych przez program
void freeOpenGLProgram() {
	delete shaderProgram; //Usunięcie programu cieniującego

	glDeleteVertexArrays(1,&vao); //Wykasuj VAO

	//Wykasuj bufory  VBO
	glDeleteBuffers(1,&bufVertices);
	glDeleteBuffers(1,&bufNormals);
    glDeleteBuffers(1,&bufTexCoords);
    //glDeleteBuffers(1,&bufC2);

    //Wykasuj tekstury
	glDeleteTextures(1,&normalTex);

}

void drawObject(ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM) {
	//Włączenie programu cieniującego, który ma zostać użyty do rysowania
	//W tym programie wystarczyłoby wywołać to raz, w setupShaders, ale chodzi o pokazanie,
	//że mozna zmieniać program cieniujący podczas rysowania jednej sceny
	shaderProgram->use();

	//Przekaż do shadera macierze P,V i M.
	//W linijkach poniżej, polecenie:
	//  shaderProgram->getUniformLocation("P")
	//pobiera numer przypisany zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powyższym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze,
	//a mP w glm::value_ptr(mP) odpowiada argumentowi  "mat4 mP;" TYM pliku.
	//Cała poniższa linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane z argumentu mP niniejszej funkcji
	//Pozostałe polecenia działają podobnie.
	//Poniższe polecenia są z grubsza odpowiednikami glLoadMatrixf ze starego opengla
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(mM));

	//Powiąż zmienne typu sampler2D z jednostkami teksturującymi
	//glUniform1i(shaderProgram->getUniformLocation("normalMap"),1);


	//Przypisz tekstury do jednostek teksturujących
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[0]);

    //Aktywuj VAO  (powiązania pomiędzy atrybutami i VBO)
    glBindVertexArray(vao);

	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES,0,vertexCount);


	//Dezaktywuj VAO
	glBindVertexArray(0);
}
void drawObject2(ShaderProgram *shaderProgram, mat4 mP, mat4 mV, mat4 mM) {
	//Włączenie programu cieniującego, który ma zostać użyty do rysowania
	//W tym programie wystarczyłoby wywołać to raz, w setupShaders, ale chodzi o pokazanie,
	//że mozna zmieniać program cieniujący podczas rysowania jednej sceny
	shaderProgram->use();

	//Przekaż do shadera macierze P,V i M.
	//W linijkach poniżej, polecenie:
	//  shaderProgram->getUniformLocation("P")
	//pobiera numer przypisany zmiennej jednorodnej o podanej nazwie
	//UWAGA! "P" w powyższym poleceniu odpowiada deklaracji "uniform mat4 P;" w vertex shaderze,
	//a mP w glm::value_ptr(mP) odpowiada argumentowi  "mat4 mP;" TYM pliku.
	//Cała poniższa linijka przekazuje do zmiennej jednorodnej P w vertex shaderze dane z argumentu mP niniejszej funkcji
	//Pozostałe polecenia działają podobnie.
	//Poniższe polecenia są z grubsza odpowiednikami glLoadMatrixf ze starego opengla
	glUniformMatrix4fv(shaderProgram->getUniformLocation("P"),1, false, glm::value_ptr(mP));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("V"),1, false, glm::value_ptr(mV));
	glUniformMatrix4fv(shaderProgram->getUniformLocation("M"),1, false, glm::value_ptr(mM));

	//Powiąż zmienne typu sampler2D z jednostkami teksturującymi
	//glUniform1i(shaderProgram->getUniformLocation("normalMap"),1);


	//Przypisz tekstury do jednostek teksturujących
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,textures[1]);

    //Aktywuj VAO  (powiązania pomiędzy atrybutami i VBO)
    glBindVertexArray(vao2);

	//Narysowanie obiektu
	glDrawArrays(GL_TRIANGLES,0,vertexCount2);


	//Dezaktywuj VAO
	glBindVertexArray(0);
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
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // change this value to your liking
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw1 += xoffset;
    pitch1 += yoffset;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
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

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    if (fov >= 1.0f && fov <= 45.0f)
        fov -= yoffset;
    if (fov <= 1.0f)
        fov = 1.0f;
    if (fov >= 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    float cameraSpeed = 15 * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

//Procedura rysująca zawartość sceny
void drawScene(GLFWwindow* window, float angle_x, float angle_y, float eye_angle_x, float eye_angle_y) {
	//************Tutaj umieszczaj kod rysujący obraz******************l

	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT); //Wykonaj czyszczenie bufora kolorów i głębokości

	glm::mat4 P = glm::perspective(50 * PI / 180, aspect, 1.0f, 50.0f); //Wylicz macierz rzutowania
    glm::mat4 V = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
	glm::mat4 M = glm::mat4(1.0f);

	//M = glm::rotate(M, angle_x, glm::vec3(1, 0, 0));
	//M = glm::rotate(M, angle_y, glm::vec3(0, 1, 0));

    M = glm::translate(M,vec3(0.0f,0.0f,8.0f));
	//Narysuj obiekt
	drawObject(shaderProgram,P,V,M);

    M = glm::translate(M,vec3(8.0f,0.0f,0.0f));
	drawObject2(shaderProgram2,P,V,M);

	//Przerzuć tylny bufor na przedni
	glfwSwapBuffers(window);

}

int main(void)
{

    vertexCount = v_vertices.size();
    vertexCount2 = v_vertices2.size();

	GLFWwindow* window; //Wskaźnik na obiekt reprezentujący okno

	glfwSetErrorCallback(error_callback);//Zarejestruj procedurę obsługi błędów

	if (!glfwInit()) { //Zainicjuj bibliotekę GLFW
		fprintf(stderr, "Nie można zainicjować GLFW.\n");
		exit(EXIT_FAILURE);
	}

	window = glfwCreateWindow(500, 500, "OpenGL", NULL, NULL);  //Utwórz okno 500x500 o tytule "OpenGL" i kontekst OpenGL.

	if (!window) //Jeżeli okna nie udało się utworzyć, to zamknij program
	{
		fprintf(stderr, "Nie można utworzyć okna.\n");
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glfwMakeContextCurrent(window); //Od tego momentu kontekst okna staje się aktywny i polecenia OpenGL będą dotyczyć właśnie jego.
	glfwSwapInterval(1); //Czekaj na 1 powrót plamki przed pokazaniem ukrytego bufora

	if (glewInit() != GLEW_OK) { //Zainicjuj bibliotekę GLEW
		fprintf(stderr, "Nie można zainicjować GLEW.\n");
		exit(EXIT_FAILURE);
	}

	initOpenGLProgram(window); //Operacje inicjujące

	float angle_x = 0; //Kąt obrotu obiektu
	float angle_y = 0; //Kąt obrotu obiektu
    float eye_angle_x = 0; //Kąt obrotu obiektu
	float eye_angle_y = 0; //Kąt obrotu obiektu

	glfwSetTime(0); //Wyzeruj licznik czasu

	//Główna pętla
	while (!glfwWindowShouldClose(window)) //Tak długo jak okno nie powinno zostać zamknięte
	{

        processInput(window);
        deltaTime = glfwGetTime();
		glfwSetTime(0); //Wyzeruj licznik czasu
		drawScene(window,angle_x,angle_y,eye_angle_x,eye_angle_y); //Wykonaj procedurę rysującą
		glfwPollEvents(); //Wykonaj procedury callback w zalezności od zdarzeń jakie zaszły.
	}

	freeOpenGLProgram();

	glfwDestroyWindow(window); //Usuń kontekst OpenGL i okno
	glfwTerminate(); //Zwolnij zasoby zajęte przez GLFW
	exit(EXIT_SUCCESS);
}
