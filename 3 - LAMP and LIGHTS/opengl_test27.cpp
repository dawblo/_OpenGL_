#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION // dodane do pliku z kursu 
#define GLM_ENABLE_EXPERIMENTAL  // naprawia bledy

#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<glm/gtc/type_ptr.hpp>          //do transformacji, macierze, obracanie 
#include<glm/gtx/rotate_vector.hpp>
#include<glm/gtx/vector_angle.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_m.h>       //z kursu
#include <learnopengl/camera.h>
#include <learnopengl/model.h>


#include"EBO.h"
#include"VBO.h"
#include"VAO.h"

#include <iostream>

//TODO:
//
//Naprawa trybu wolnej kamery
//Problem z generatorem po zdobyciu 25 pkt
//Niebo do naprawienia
//Generowanie budynkow w losowych miejscach (z odstepem od siebie)
//Jakieś lampy? (wykorzystanie świateł)

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void Inputs(GLFWwindow* window); //Funkcja obsługująca klawisze
unsigned int loadTexture(const char* path); //Funkcja wczytująca tekstury
unsigned int loadCubemap(vector<std::string> faces); //Funkcja wczytująca Cubemape
void PrintCoordinates(glm::vec3 modelPos);
void ModelInputs(GLFWwindow* window);
void setDefaults();

// szerokość/wysokość okna
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

//PARAMETRY KAMERY
float FOVdeg = 45.0f;           // dystans renderowania 
float nearPlane = 0.1f;     //odleglosc wczytywania terenu, wglab etc
float farPlane = 100.0f;

//Powoduje ze kamera nie skacze podczas klikania
bool firstClick = true;

//Poprawia szybkość i czułość podczas obracania kamery
float speed = 10.f;
float sensitivity = 100.0f;

//Defaultowe ustawienia
int points = 0; //punkty
int record = 0;
int deaths = 0;
bool endGame = false; //czy przegrana
bool gameplay = false; //tryb rozgrywki/tryb swobodnej kamery
bool wireframeMode = false; //tryb wireframe

glm::vec3 Position = glm::vec3(0.0f, 1.5f, 4.0f); //pozycja kamery     
glm::vec3 RememberedPosition = glm::vec3(0.0f, 0.0f, 0.0f); //pozycja kamery do zapamiętania (używana dopiero po zmianie trybu)
glm::vec3 Orientation = glm::vec3(0.0f, 0.0f, -1.0f); //domyślna orientacja kamery
glm::vec3 Up = glm::vec3(0.0f, 1.0f, 0.0f); //vektor w górę
glm::mat4 cameraMatrix = glm::mat4(1.0f);  //do matrixa kamery
glm::vec3 modelPos = glm::vec3(0.0f, 0.0f, 0.00f); //domyslna pozycja gracza
glm::vec3 carPos = glm::vec3(0.5f, 0.5f, -10.f);  //domyslna pozycja bloku przeszkody
glm::vec3 pointPos = glm::vec3(0.5f, 0.5f, -15.f);

float carsSpeed = 0.05f; //domyslna szybkosc bloku przeszkody
float pointSpeed = 0.005f; //domyslna szybkosc bloku przeszkody
float pointHeightSpeed = 0.005f;
float characterSpeed = 0.005f;
float defaultCharacterSpeed = 0.01f;
float modelRot = 0.0f;
float pointRot = 0.0f;

//domyslne parametry generatora swiata
int grass_width_min = -10;
int grass_width_max = 10;
int grass_height_min = -15;
int grass_height_max = 15;


const char* title = "Gra | Punkty: 0 | Poziom: 0 | Szybkosc postaci: 0 | Szybkosc rywala: 0 | Odleglosc: 0 | Skuchy: 0 | Rekord: 0";

GLfloat grassVertices[] = {
    //             KORDY  /    KOLORY                / TEXTURY      /   NORMALS
    -1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 0.0f,		0.0f, 1.0f, 0.0f,
    -1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		0.0f, 1.0f,		0.0f, 1.0f, 0.0f,
     1.0f, 0.0f, -1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 1.0f,		0.0f, 1.0f, 0.0f,
     1.0f, 0.0f,  1.0f,		0.0f, 0.0f, 0.0f,		1.0f, 0.0f,		0.0f, 1.0f, 0.0f
};
GLuint grassIndices[] = {
    0, 1, 2, // Dolna scianka
    0, 2, 3, // Dolna scianka
};

float buildingVertices[] = {

    //       KORDY  /    KOLORY                / TEXTURY      /   NORMALS
    -2.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,   0.0f,	0.0f,      0.0f,  0.0f, -1.0f, //Scianka tylnia
     2.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	0.0f,      0.0f,  0.0f, -1.0f,
     2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	1.0f,      0.0f,  0.0f, -1.0f,
     2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	1.0f,      0.0f,  0.0f, -1.0f,
    -2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    0.0f,	1.0f,      0.0f,  0.0f, -1.0f,
    -2.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,    0.0f,	0.0f,      0.0f,  0.0f, -1.0f,

    -2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f,     0.0f,	0.0f,	 0.0f,	0.0f,	1.0f,  //Scianka przednia
    2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,    0.0f,	0.0f,	1.0f,
    2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    -2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    -2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,    0.0f,	0.0f,	1.0f,

    -2.5f,  2.f,  0.5f, 0.0f, 0.0f, 0.0f,     1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,   //LEWA SCIANKA
    -2.5f,  2.f, -0.5f, 0.0f, 0.0f, 0.0f,    1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f,  2.f,  0.5f, 0.0f, 0.0f, 0.0f,     1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,

    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   1.0f,	0.0f,	0.0f,  //PRAWA SCIANKA
    2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	0.5f,  0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   1.0f,	0.0f,	0.0f,


    -2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   0.0f,	1.0f,	0.0f, //GORNA
    2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,   0.0f,	1.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    -2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    -2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   0.0f,	1.0f,	0.0f,
};

float lampVertices[] = {

    //       KORDY  /    KOLORY                / TEXTURY      /   NORMALS
    -2.5f, -0.5f, -0.5f,   0.0f, 0.0f, 0.0f,   0.0f,	0.0f,      0.0f,  0.0f, -1.0f, //Scianka tylnia
     2.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	0.0f,      0.0f,  0.0f, -1.0f,
     2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	1.0f,      0.0f,  0.0f, -1.0f,
     2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    1.0f,	1.0f,      0.0f,  0.0f, -1.0f,
    -2.5f,  2.f, -0.5f,  0.0f, 0.0f, 0.0f,    0.0f,	1.0f,      0.0f,  0.0f, -1.0f,
    -2.5f, -0.5f, -0.5f,  0.0f, 0.0f, 0.0f,    0.0f,	0.0f,      0.0f,  0.0f, -1.0f,

    -2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f,     0.0f,	0.0f,	 0.0f,	0.0f,	1.0f,  //Scianka przednia
    2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,    0.0f,	0.0f,	1.0f,
    2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    -2.5f,	2.f,	0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,    0.0f,	0.0f,	1.0f,
    -2.5f,	-0.5f,	0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,    0.0f,	0.0f,	1.0f,

    -2.5f,  2.f,  0.5f, 0.0f, 0.0f, 0.0f,     1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,   //LEWA SCIANKA
    -2.5f,  2.f, -0.5f, 0.0f, 0.0f, 0.0f,    1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f, -0.5f,  0.5f, 0.0f, 0.0f, 0.0f,    0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
    -2.5f,  2.f,  0.5f, 0.0f, 0.0f, 0.0f,     1.0f, 0.0f,    -1.0f,  0.0f,  0.0f,

    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   1.0f,	0.0f,	0.0f,  //PRAWA SCIANKA
    2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	-0.5f,	0.5f,  0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,   1.0f,	0.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   1.0f,	0.0f,	0.0f,


    -2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   0.0f,	1.0f,	0.0f, //GORNA
    2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	1.0f,	1.0f,   0.0f,	1.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	1.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    -2.5f,	2.f,	0.5f,  0.0f, 0.0f, 0.0f, 	0.0f,	0.0f,   0.0f,	1.0f,	0.0f,
    -2.5f,	2.f,	-0.5f, 0.0f, 0.0f, 0.0f, 	0.0f,	1.0f,   0.0f,	1.0f,	0.0f,
};

GLfloat lightVertices[] = { //  KORDY
    -0.1f, -0.1f, 0.1f, 
    -0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, -0.1f,
    0.1f, -0.1f, 0.1f,

    -0.1f, 0.1f, 0.1f,
    -0.1f, 0.1f, -0.1f,
    0.1f, 0.1f, -0.1f,
    0.1f, 0.1f, 0.1f,
};
GLuint lightIndices[] = {
    0,1,2,
    0,3,2,//
    0,4,7,
    0,7,3,//
    3,7,6,
    3,6,2,//
    2,6,5,
    2,1,5,//
    1,5,4,
    1,0,4,//
    4,7,6,
    4,5,6,//
};

GLfloat carVertices[] = { //  KORDY
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,
};
GLuint carIndices[] = {
    0,1,2,
    0,3,2,//
    0,4,7,
    0,7,3,//
    3,7,6,
    3,6,2,//
    2,6,5,
    2,1,5,//
    1,5,4,
    1,0,4,//
    4,7,6,
    4,5,6,//
};

GLfloat houseVertices[] = {
    -1.0f, -1.0f, 1.0f,
    -1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, -1.0f,
    1.0f, -1.0f, 1.0f,

    -1.0f, 1.0f, 1.0f,
    -1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, -1.0f,
    1.0f, 1.0f, 1.0f,

};
GLuint houseIndices[] ={
    0,1,2,
    0,3,2,//
    0,4,7,
    0,7,3,//
    3,7,6,
    3,6,2,//
    2,6,5,
    2,1,5,//
    1,5,4,
    1,0,4,//
    4,7,6,
    4,5,6,//
};

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

struct Character {
    unsigned int TextureID;  // ID handle of the glyph texture
    glm::ivec2   Size;       // Size of glyph
    glm::ivec2   Bearing;    // Offset from baseline to left/top of glyph
    unsigned int Advance;    // Offset to advance to next glyph
};

int main()
{
    // Inicjalizacja i konfiguracja środowiska
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);          //wersja opengl
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);      //profil funkcji w opengl


    // Tworzenie okna
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Błąd podczas wczytywanie okienka GLFW" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window); //ustawia obecne okno
    gladLoadGL(); //Ładowanie glada
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);        //generuje szerokosc ekranu od lewo do prawo

    // Błąd podczas wczytywania glada
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Błąd podczas ładowania GLADA" << std::endl;
        return -1;
    }

    //GENEROWANIE OBJEKTÓW TRAWY=====================================================================================
    Shader grassShader("grassShader.vert", "grassShader.frag");

    VAO grassVAO;
    grassVAO.Bind();  //obiektowka do vao.cpp

    VBO grassVBO(grassVertices, sizeof(grassVertices)); // Generuje VBO
    EBO grassEBO(grassIndices, sizeof(grassIndices)); // Generuje EBO

    //Przypisuje VBO takie jak kordyynaty czy kolory do VAO
    grassVAO.LinkAttrib(grassVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);                    //vertex
    grassVAO.LinkAttrib(grassVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));  //kolory
    grassVAO.LinkAttrib(grassVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));  //textury
    grassVAO.LinkAttrib(grassVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));  //normalsy

    //Odbindowanie
    grassVAO.Unbind();
    grassVBO.Unbind();
    grassEBO.Unbind();


    //GENEROWANIE OBJEKTÓW SKYBOXA=====================================================================================
    Shader skyboxShader("skyboxShader.vert", "skyboxShader.frag");
    VAO skyboxVAO;
    skyboxVAO.Bind();

    VBO skyboxVBO(skyboxVertices, sizeof(skyboxVertices));
    skyboxVAO.LinkAttrib(skyboxVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        "right2.jpg",
        "left.jpg",
        "top.jpg",
        "bottom.jpg",
        "front.jpg",
        "back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);
    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);

    //Odbindowanie
    skyboxVAO.Unbind();
    skyboxVBO.Unbind();

    //GENEROWANIE OBJEKTÓW BUDYNKÓW=====================================================================================
    Shader buildingShader("buildShader.vert", "buildShader.frag");
    VAO buildingVAO;
    buildingVAO.Bind();

    VBO buildingVBO(buildingVertices, sizeof(buildingVertices));

    buildingVAO.LinkAttrib(buildingVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);                    //vertex
    buildingVAO.LinkAttrib(buildingVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));  //kolory
    buildingVAO.LinkAttrib(buildingVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));  //textury
    buildingVAO.LinkAttrib(buildingVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));  //normalsy


    buildingVAO.Unbind();
    buildingVBO.Unbind();


    //GENEROWANIE LAMP ŚWIETLNYCH=====================================================================================
    Shader lampShader("lampShader.vert", "lampShader.frag");
    VAO lampVAO;
    lampVAO.Bind();

    VBO lampVBO(lampVertices, sizeof(lampVertices));

    lampVAO.LinkAttrib(lampVBO, 0, 3, GL_FLOAT, 11 * sizeof(float), (void*)0);                    //vertex
    lampVAO.LinkAttrib(lampVBO, 1, 3, GL_FLOAT, 11 * sizeof(float), (void*)(3 * sizeof(float)));  //kolory
    lampVAO.LinkAttrib(lampVBO, 2, 2, GL_FLOAT, 11 * sizeof(float), (void*)(6 * sizeof(float)));  //textury
    lampVAO.LinkAttrib(lampVBO, 3, 3, GL_FLOAT, 11 * sizeof(float), (void*)(8 * sizeof(float)));  //normalsy


    lampVAO.Unbind();
    lampVBO.Unbind();

    //GENEROWANIE OBJEKTÓW PRZESZKÓD=====================================================================================
    Shader carShader("carShader.vert", "carShader.frag");
    VAO carVAO;
    carVAO.Bind();

    VBO carVBO(carVertices, sizeof(carVertices));
    EBO carEBO(carIndices, sizeof(carIndices));

    carVAO.LinkAttrib(carVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    carVAO.Unbind();
    carVBO.Unbind();
    carEBO.Unbind();

    //GENEROWANIE OBJEKTÓW LAMP/SWIATLA=====================================================================================

    //Kwadrat obsługujący światło
    Shader lightShader("lightShader.vert", "lightShader.frag");

    //Generuje VAO i przypisuje
    VAO lightVAO;
    lightVAO.Bind();

    //Generuje VBO i przypisuje do vertexow
    VBO lightVBO(lightVertices, sizeof(lightVertices));
    //Generuje EBO i rzypisuje do indices
    EBO lightEBO(lightIndices, sizeof(lightIndices));

    //Przypisuje atrybuty VBO takie jak kordyynaty czy kolory
    lightVAO.LinkAttrib(lightVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    //Unbinduje zebysmy przypadkowo ich nie zmodyfikowali
    lightVAO.Unbind();
    lightVBO.Unbind();
    lightEBO.Unbind();

    //GENEROWANIE OBJEKTÓW DOMKU (do usuniecia) =====================================================================================

    Shader houseShader("houseShader.vert", "houseShader.frag");

    VAO houseVAO;
    houseVAO.Bind();

    VBO houseVBO(houseVertices, sizeof(houseVertices));
    EBO houseEBO(houseIndices, sizeof(houseIndices));

    houseVAO.LinkAttrib(houseVBO, 0, 3, GL_FLOAT, 3 * sizeof(float), (void*)0);

    houseVAO.Unbind();
    houseVBO.Unbind();
    houseEBO.Unbind();
    
    //GENEROWANIE OBJEKTÓW GRACZA  =====================================================================================
    Shader modelShader("modelShader.vert", "modelShader.frag");

    // Wczytywanie modeli
    Model ourModel("../resources/objects/nanosuit/nanosuit.obj");

    //DOMYSLNE USTAWIENIA =====================================================================================
                                
    glm::vec3 lightPos = glm::vec3(27.0f, 37.0f, 100.0f);
    glm::vec3 lightPos2 = glm::vec3(0.0f, 1.45f, 0.005f);
    glm::vec4 lightColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //defaultowe pozycje modeli rgb+przezroczystosc
    glm::vec4 lightColor2 = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); //defaultowe pozycje modeli rgb+przezroczystosc
    glm::mat4 lightModel = glm::mat4(1.0f);
    lightModel = glm::translate(lightModel, lightPos);   
   

    glm::mat4 carModel = glm::mat4(1.0f);
    carModel = glm::translate(carModel, carPos);

    glm::vec3 buildingPos = glm::vec3(0.5f, 0.5f, -20.22f);
    glm::mat4 buildingModel = glm::mat4(1.0f);
    buildingModel = glm::rotate(buildingModel, glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
    buildingModel = glm::translate(buildingModel, buildingPos);

    glm::vec3 lampPos = glm::vec3(0.5f, 0.5f, -20.22f);
    glm::mat4 lampModel = glm::mat4(1.0f);
    lampModel = glm::rotate(lampModel, glm::radians(90.f), glm::vec3(0.0f, 1.0f, 0.0f));
    lampModel = glm::translate(lampModel, lampPos);

    glm::vec3 grassPos = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::mat4 grassModel = glm::mat4(1.0f);
    grassModel = glm::scale(grassModel, glm::vec3(1.0f, 1.0f, 1.0f));
    grassModel = glm::translate(grassModel, grassPos);

    glm::vec3 housePos = glm::vec3(3.0f, 0.0f, 3.0f);
    glm::mat4 houseModel = glm::mat4(1.0f);
    houseModel = glm::translate(houseModel, housePos);

    glm::vec3 calculatedModelPos = modelPos;
 
    lightShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel)); //wysylaja do shadera vertexow, pozwala na transformacje obiektow
    glUniform4f(glGetUniformLocation(lightShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    carShader.use();
    glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(carModel)); //wysylaja do shadera vertexow, pozwala na transformacje obiektow
    glUniform4f(glGetUniformLocation(carShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    grassShader.use();
    glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(grassModel));
    glUniform4f(glGetUniformLocation(grassShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(grassShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    glUniform3f(glGetUniformLocation(grassShader.ID, "lightPos2"), lightPos2.x, lightPos2.y, lightPos2.z);
    glUniform4f(glGetUniformLocation(grassShader.ID, "lightColor2"), lightColor2.x, lightColor2.y, lightColor2.z, lightColor2.w);

    buildingShader.use();
    glUniformMatrix4fv(glGetUniformLocation(buildingShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(buildingModel));
    glUniform4f(glGetUniformLocation(buildingShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(buildingShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    lampShader.use();
    glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
    glUniform4f(glGetUniformLocation(lampShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(lampShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
    houseShader.use();
    glUniformMatrix4fv(glGetUniformLocation(houseShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(houseModel));
    glUniform4f(glGetUniformLocation(houseShader.ID, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
    glUniform3f(glGetUniformLocation(houseShader.ID, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

    //TEKSTURY =====================================================================================

    //TEXTURA1-POCZĄTEK
    unsigned int texture = loadTexture("grass.png");

    //TEXTURA2-POCZĄTEK
    unsigned int texture2 = loadTexture("bulding.png");

    //TEXTURA2-POCZĄTEK
    unsigned int texture3 = loadTexture("lamp.png");
    


    glEnable(GL_DEPTH_TEST);   //glebia zeby nie nachodzily na siebie

    // Tryb wireframe
    double prevTime = glfwGetTime();

    // Główna pętla
    while (!glfwWindowShouldClose(window))
    {
        string newTitle = "Gra | Punkty: " + std::to_string(points) +" | Szybkosc postaci: " + std::to_string(int(characterSpeed * 100)) + " | Szybkosc rywala: " + std::to_string(int(carsSpeed * 100)) + " | Odleglosc: " + std::to_string(int(-modelPos.z)) + " | Skuchy: " + std::to_string(deaths) + " | Rekord: " + std::to_string(record) + "";
        title = newTitle.c_str();
        glfwSetWindowTitle(window,title);
        if (endGame == false) {
            glClearColor(0.05f, 0.05f, 0.05f, 1.0f);  //kolor tla
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //oczyszcza bufor
            double crntTime = glfwGetTime();
            if (crntTime - prevTime >= 1 / 60) {
                pointRot += 1.f;
                prevTime = crntTime;
            }
            if (wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            if (!wireframeMode) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

            //Obsluga klawiszów kamery
            if (!gameplay) Inputs(window);

            //Tryb swobodnej kamery
            if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
                RememberedPosition = Position;
                gameplay = false;
            }
            if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
                Position = RememberedPosition;
                Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
                gameplay = true;
            }

            //Tryb poligonów
            if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
                wireframeMode = true;
            }
            if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
                wireframeMode = false;
            }

            //Aktualizuje i eksportuje martix kamery do VertexShadera
            glm::mat4 view = glm::mat4(1.0f);
            glm::mat4 projection = glm::mat4(1.0f);


            //Powoduje ze kamera patrzy sie we wlasciwym kierunku
            if (!gameplay) view = glm::lookAt(Position, Position + Orientation, Up);
            if (gameplay) view = glm::lookAt(Position, modelPos + Orientation, Up);

            //Dodaje perspektywe do sceny
            projection = glm::perspective(glm::radians(FOVdeg), (float)(SCR_WIDTH / SCR_HEIGHT), nearPlane, farPlane);
            //Eksportuje kamere do vertex shadera
            cameraMatrix = projection * view;

            //TRAWA============================================================================================================================================

            //Generowanie trawy
            if (modelPos.z <= (grass_height_min * 10) + 50) {
                cout << "Generowanie terenu..." << endl;
                grass_height_min -= 15;
                grass_height_max -= 15;
            }

            for (int i = grass_width_min; i < grass_width_max; i++) {
                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 grassPos = glm::vec3(0.0f + (i * 2), 0.0f, 0.0f + (j * 2));
                    glm::mat4 grassModel = glm::mat4(1.0f);
                    grassModel = glm::scale(grassModel, glm::vec3(5.0f, 5.0f, 5.0f));
                    grassModel = glm::translate(grassModel, grassPos);

                    glBindTexture(GL_TEXTURE_2D, texture);
                    grassShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(grassModel));
                    glUniform3f(glGetUniformLocation(grassShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));



                    //planksSpec.Bind();
                    grassVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawElements(GL_TRIANGLES, sizeof(grassIndices) / sizeof(int), GL_UNSIGNED_INT, 0); //Narysuj trójk¹t u¿ywaj¹c GL_TRIANGLE
                }

            }
            //BUDYNKI============================================================================================================================================

                //Generowanie budynków
                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 buildingPos = glm::vec3(5.0f, 0.0f, 0.0f + (j * 5));
                    glm::mat4 buildingModel = glm::mat4(1.0f);
                    buildingModel = glm::scale(buildingModel, glm::vec3(5.0f, 5.0f, 5.0f));
                    buildingModel = glm::translate(buildingModel, buildingPos);

                    glBindTexture(GL_TEXTURE_2D, texture2);
                    buildingShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(buildingShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(buildingModel));
                    glUniform3f(glGetUniformLocation(buildingShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(buildingShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));



                    //planksSpec.Bind();
                    buildingVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 buildingPos = glm::vec3(-5.0f, 0.0f, 0.0f + (j * 5));
                    glm::mat4 buildingModel = glm::mat4(1.0f);
                    buildingModel = glm::scale(buildingModel, glm::vec3(5.0f, 5.0f, 5.0f));
                    buildingModel = glm::translate(buildingModel, buildingPos);

                    glBindTexture(GL_TEXTURE_2D, texture2);
                    buildingShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(buildingShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(buildingModel));
                    glUniform3f(glGetUniformLocation(buildingShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(buildingShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));



                    //planksSpec.Bind();
                    buildingVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                //LAMPY============================================================================================================================================

                //Generowanie lamp
                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 lampPos = glm::vec3(250.0f, 0.0f, 0.0f + (j * 150));
                    glm::mat4 lampModel = glm::mat4(1.0f);
                    lampModel = glm::scale(lampModel, glm::vec3(0.04f, 1.8f, 0.09f));
                    lampModel = glm::translate(lampModel, lampPos);

                    glBindTexture(GL_TEXTURE_2D, texture3);
                    lampShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
                    glUniform3f(glGetUniformLocation(lampShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));



                    //planksSpec.Bind();
                    lampVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }


                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 lampPos = glm::vec3(-250.0f, 0.0f, 0.0f + (j * 150));
                    glm::mat4 lampModel = glm::mat4(1.0f);
                    lampModel = glm::scale(lampModel, glm::vec3(0.04f, 1.8f, 0.09f));
                    lampModel = glm::translate(lampModel, lampPos);

                    glBindTexture(GL_TEXTURE_2D, texture3);
                    lampShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
                    glUniform3f(glGetUniformLocation(lampShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(lampShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));



                    //planksSpec.Bind();
                    lampVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }
                //LAMPY - Światła============================================================================================================================================

                //Generowanie lamp - światła
                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 lampPos = glm::vec3(245.0f, 715.0f, 0.0f + (j * 150));
                    glm::mat4 lampModel = glm::mat4(1.0f);
                    lampModel = glm::scale(lampModel, glm::vec3(0.04f, 0.005f, 0.09f));
                    lampModel = glm::translate(lampModel, lampPos);

                    glBindTexture(GL_TEXTURE_2D, texture3);
                    lightShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
                    glUniform3f(glGetUniformLocation(lightShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
                    glUniform3f(glGetUniformLocation(grassShader.ID, "lampPos"), lampPos.x, lampPos.y, lampPos.z);


                    //planksSpec.Bind();
                    lampVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }


                for (int j = grass_height_min; j < grass_height_max; j++)
                {
                    glm::vec3 lampPos = glm::vec3(-245.0f, 715.0f, 0.0f + (j * 150));
                    glm::mat4 lampModel = glm::mat4(1.0f);
                    lampModel = glm::scale(lampModel, glm::vec3(0.04f, 0.005f, 0.09f));
                    lampModel = glm::translate(lampModel, lampPos);

                    glBindTexture(GL_TEXTURE_2D, texture3);
                    lightShader.use();
                    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lampModel));
                    glUniform3f(glGetUniformLocation(lightShader.ID, "camPos"), Position.x, Position.y, Position.z);  //wysylamy pozycje kamery do shadera
                    glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
                    


                    //planksSpec.Bind();
                    lampVAO.Bind(); //Przypisz VAO ¿eby OpenGL go u¿ywa³
                    glDrawArrays(GL_TRIANGLES, 0, 36);
                }

                

            //BUDYNEK============================================================================================================================================

                houseShader.use();

                //cout << "car X:" + std::to_string(carPos.x) << endl;
                pointPos.x -= pointSpeed;
                pointPos.y -= pointHeightSpeed;
                if (modelPos.x <= pointPos.x + 0.30f && modelPos.x >= pointPos.x - 0.30f && modelPos.z <= pointPos.z + 0.30f && modelPos.z >= pointPos.z - 0.30)
                {
                    pointPos.z -= 20;
                    defaultCharacterSpeed += 0.01f;

                }
                if (modelPos.z < pointPos.z - 2) {
                    pointPos.z -= 20;
                    points += 1;
                    if (points > record) record = points;
                    cout << "OMINALES PUNKT, GENERUJE NOWY:" << endl;
                }

                if (pointPos.x >= 5.f || pointPos.x <= -5.f) {
                    pointSpeed = -pointSpeed;
                }
                if (pointPos.y >= 1.2f || pointPos.y <= 0.0f) {
                    pointHeightSpeed = -pointHeightSpeed;
                }
                houseModel = glm::mat4(1.0f);
                houseModel = glm::translate(houseModel, pointPos);
                houseModel = glm::scale(houseModel, glm::vec3(0.2f, 0.2f, 0.2f));
                houseModel = glm::rotate(houseModel, glm::radians(pointRot), glm::vec3(0.0f, 1.0f, 0.0f));   //rotacja


                glUniform3f(glGetUniformLocation(grassShader.ID, "lightPos2"), pointPos.x, pointPos.y, pointPos.z);
                glUniformMatrix4fv(glGetUniformLocation(houseShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(houseModel));

                glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
                houseVAO.Bind();

                glDrawElements(GL_TRIANGLES, sizeof(houseIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

            //ŚWIATŁO============================================================================================================================================

                lightShader.use();
                glUniformMatrix4fv(glGetUniformLocation(lightShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(lightModel));
                glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
                lightVAO.Bind();
                glDrawElements(GL_TRIANGLES, sizeof(lightIndices) / sizeof(int), GL_UNSIGNED_INT, 0);

            //SAMOCHÓD===========================================================================================================================================

            carShader.use();

            //cout << "car X:" + std::to_string(carPos.x) << endl;
            carPos.x -= carsSpeed;
            if (modelPos.x <= carPos.x + 1 && modelPos.x >= carPos.x - 1 && modelPos.z <= carPos.z + 1 && modelPos.z >= carPos.z - 1)
            {
                if (endGame == false) cout << "KONIEC GRY, WYNIK KONCOWY:" + std::to_string(points) << endl;
                points = 0;
                deaths += 1;
                endGame = true;
            }
            if (modelPos.z < carPos.z - 2) {
                carPos.z -= 20;
                points += 1;
                if (points > record) record = points;
                if (carsSpeed < 0) {
                    carsSpeed -= 0.01f;
                }
                else {
                    carsSpeed += 0.01f;
                }

                cout << "OMINALES PRZESZKODE, PUNKTY:" + std::to_string(points) << endl;
                //cout << "SZYBKOSC PRZESZKOD ZWIEKSZONA - :" + std::to_string(carsSpeed) << endl;
            }

            if (carPos.x >= 15.f || carPos.x <= -15.f) {
                carsSpeed = -carsSpeed;
            }
            carModel = glm::mat4(1.0f);
            carModel = glm::translate(carModel, carPos);


            glUniformMatrix4fv(glGetUniformLocation(carShader.ID, "model"), 1, GL_FALSE, glm::value_ptr(carModel));

            glUniformMatrix4fv(glGetUniformLocation(grassShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
            carVAO.Bind();

            glDrawElements(GL_TRIANGLES, sizeof(carIndices) / sizeof(int), GL_UNSIGNED_INT, 0);


            //MODEL============================================================================================================================================
            modelShader.use();

            // Wyślij cameraMatrix do modelShadera
            glUniformMatrix4fv(glGetUniformLocation(modelShader.ID, "camMatrix"), 1, GL_FALSE, glm::value_ptr(cameraMatrix));

            modelRot = 0;
            if (gameplay) ModelInputs(window);

            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
                characterSpeed = defaultCharacterSpeed + 0.01f;
                if (gameplay) modelPos.z -= characterSpeed;
                if (gameplay) Position.z -= characterSpeed;
                //PrintCoordinates(modelPos);
            }
            else {
                characterSpeed = defaultCharacterSpeed;
                if (gameplay) modelPos.z -= characterSpeed;
                if (gameplay) Position.z -= characterSpeed;
            }

            //KOLEJNOŚĆ MA ZNACZENIE!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, modelPos);
            model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f));   //rotacja
            model = glm::rotate(model, glm::radians(modelRot), glm::vec3(0.0f, 1.0f, 0.0f));   //rotacja
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));

            modelShader.setMat4("model", model);
            ourModel.Draw(modelShader);


            //SKYBOX============================================================================================================================================
            glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
            skyboxShader.use();
            if (!gameplay) view = glm::mat4(glm::mat3(glm::lookAt(Position, Position + Orientation, Up)));
            if (gameplay) view = glm::mat4(glm::mat3(glm::lookAt(Position, modelPos + Orientation, Up)));

            skyboxShader.setMat4("view", view);
            skyboxShader.setMat4("projection", projection);
            // skybox cube
            skyboxVAO.Bind();
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            glBindVertexArray(0);
            glDepthFunc(GL_LESS); // set depth function back to default



            // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
            glfwSwapBuffers(window);
            glfwPollEvents();
        }
        else {
            setDefaults();
        }

    }

    glfwTerminate();
    return 0;
}



unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Błąd podczas wczytywania tekstury: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void setDefaults() {
    Position = glm::vec3(0.0f, 1.5f, 4.0f);
    RememberedPosition = glm::vec3(0.0f, 0.0f, 0.0f);
    Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraMatrix = glm::mat4(1.0f);  //do shadera vert
    modelPos = glm::vec3(0.0f, 0.0f, 0.00f);
    pointPos = glm::vec3(0.5f, 0.5f, -15.f);
    carPos = glm::vec3(0.5f, 0.5f, -10.f);

    grass_width_min = -10;
    grass_width_max = 10;
    grass_height_min = -15;
    grass_height_max = 15;

    defaultCharacterSpeed = 0.01f;
    carsSpeed = 0.05f;
    pointSpeed = 0.01f;
    points = 0;


    endGame = false;
}

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Błąd podczas wczytywania Cubemapy: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void Inputs(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        Position += speed * Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        Position += speed * -Orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        Position += speed * -glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        Position += speed * Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        Position += speed * -Up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        speed += 0.00001f;
    }
    else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) { //release przywraca domyslna wartosc
        speed = 0.001f;
    }

    //Obsługa myszy   
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {
        //Ukrywa kursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

        //Powoduje że kamera nie skacze gdy klikamy
        if (firstClick)
        {
            glfwSetCursorPos(window, (SCR_WIDTH / 2), (SCR_HEIGHT / 2));
            firstClick = false;
        }

        //Przetrzymuje kordyynaty myszki
        double mouseX;
        double mouseY;
        //Przechwytuje kordyynaty myszki
        glfwGetCursorPos(window, &mouseX, &mouseY);

        //Normalizuje kordynaty tak żeby zaczynały się na środku ekranu
        // wtedy transformuje je na stopnie(degrees)
        float rotx = sensitivity * (float)(mouseY - (SCR_HEIGHT / 2)) / SCR_HEIGHT;
        float roty = sensitivity * (float)(mouseX - (SCR_HEIGHT / 2)) / SCR_HEIGHT;

        //Kalkuluje nadchodzącą zmiane wertykalną w Orientation
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotx), glm::normalize(glm::cross(Orientation, Up)));

        //Decyzja czy zmiana jest dozwolona czy też nie
        if (!((glm::angle(newOrientation, Up) <= glm::radians(5.0f)) or (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))) {
            Orientation = newOrientation;
        }

        //Obraca w prawo i lewo
        Orientation = glm::rotate(Orientation, glm::radians(-roty), Up);

        //Ustawia kursor na środku ekranu
        glfwSetCursorPos(window, (SCR_WIDTH / 2), (SCR_HEIGHT / 2));

    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        //Przywraca kursor
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        //Powoduje że jest pewność że kamera nie będzie skakała przy kolejnym obracaniu
        firstClick = true;
    }
}

void ModelInputs(GLFWwindow* window) {
    //if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        //modelPos.z -= 0.01f;
        //Position.z -= 0.01f;
        //PrintCoordinates(modelPos);
    //}
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        modelRot = 45.f;
        if (modelPos.x > -3.f) {
            modelPos.x -= 0.03f;
            
        }
        //PrintCoordinates(modelPos);
    }
    else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        modelRot = 315.f;
        if (modelPos.x < 3.f) {
            modelPos.x += 0.03f;
            
        }
        //PrintCoordinates(modelPos);
    }


    //if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    //   modelPos.z += 0.01f;
    //   PrintCoordinates(modelPos);
    //}
}

void PrintCoordinates(glm::vec3 modelPos) {
    cout << "X:" + std::to_string(modelPos.x);
    cout << "Y:" + std::to_string(modelPos.y);
    cout << "Z:" + std::to_string(modelPos.z) << endl;
}


