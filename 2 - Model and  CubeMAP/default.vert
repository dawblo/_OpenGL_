#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTex;
layout (location = 3) in vec3 aNormal;

// Eksportuje do FragmentShadera
out vec3 color;
out vec2 texCoord;
out vec3 Normal;
out vec3 crntPos;

//Importuje matrix kamery z g��wnej funkcji
uniform mat4 camMatrix;
uniform mat4 model;

void main()
{
   //Przelicza obecn� pozycje
   crntPos = vec3(model*vec4(aPos, 1.0f));
   // Przelicza pozycje wszystkich kordynat�w danych vertex�w 
   gl_Position = camMatrix * model * vec4(aPos, 1.0);

   //Dodaje dane do vertex�w
   color = aColor;
   texCoord = aTex;
   Normal = aNormal;
}