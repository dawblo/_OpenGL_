#version 330 core

layout (location = 0) in vec3 aPos;

// Eksportuje do FragmentShadera
out vec3 crntPos;


uniform mat4 model;
uniform mat4 camMatrix;



void main()
{
	crntPos = vec3(model*vec4(aPos, 1.0f));
	gl_Position = camMatrix * model * vec4(aPos, 1.0f);
}
