#ifndef VAO_CLASS_H
#define VAO_CLASS_H

#include<glad/glad.h>
#include"VBO.h"

class VAO
{
public: 
	GLuint ID; //ID VAO 
	VAO(); //Konstruktor kt�ry generuje ID VAO  odpowiednik init w py

	void LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset); // Pod��cza VBO do VAO u�ywaj�c wybranego layouta
	void Bind(); //Podpina VAO
	void Unbind(); //Odpina VAO
	void Delete(); //Usuwa VAO
};

#endif
