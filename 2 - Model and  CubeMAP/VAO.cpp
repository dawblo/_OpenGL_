#include "VAO.h"


VAO::VAO() {
	glGenVertexArrays(1, &ID); // Generuje 1 VAO
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	//Konfiguruj Vertex Attribute �eby OpenGL wiedzia� jak czyta� vBO
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	// W��cz Vertex Attribute �eby openGL wiedzia� jak go u�ywa�
	glEnableVertexAttribArray(layout);

	VBO.Unbind();
}

void VAO::Bind()
{
	glBindVertexArray(ID); //we� VAO jako obecny Vertex Array Object przypisuj�c go
}

void VAO::Unbind()
{
	glBindVertexArray(0);
}

void VAO::Delete()
{
	glDeleteVertexArrays(1, &ID);
}