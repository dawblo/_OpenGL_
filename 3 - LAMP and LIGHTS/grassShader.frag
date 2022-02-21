#version 330 core

// Outputs colors in RGBA
out vec4 FragColor;

// Importuje color z Vertex Shadera
in vec3 color;
//Importuje kordyynaty textur z Vertex Shadera
in vec2 texCoord;
//Importuje normalsy z Vertex Shadera
in vec3 Normal;
//Importuje obecn¹ pozycje z Vertex Shadera
in vec3 crntPos;

//Pobiera texture z g³ównej funkcji
uniform sampler2D tex0;


//Pobiera color swiatla z glownej funkcji
uniform vec4 lightColor;
//Pobiera pozycje swiatla z glownej funkcji
uniform vec3 lightPos;

//Pobiera color swiatla z glownej funkcji
uniform vec4 lightColor2;
//Pobiera pozycje swiatla z glownej funkcji
uniform vec3 lightPos2;
//Pobiera pozycje kamery z glownej funkcji
uniform vec3 camPos;


vec4 direcLight()
{
	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(vec3(1.0f, 1.0f, 0.0f));
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 0.0f), 16);
	float specular = specAmount * specularLight;

	return (texture(tex0, texCoord) * (diffuse + ambient) + texture(tex0, texCoord).r * specular) * lightColor;
}


vec4 spotLight()
{
	// controls how big the area that is lit up is
	float outerCone = 0.90f;
	float innerCone = 0.95f;

	// ambient lighting
	float ambient = 0.20f;

	// diffuse lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPos2 - crntPos);
	float diffuse = max(dot(normal, lightDirection), 0.0f);

	// specular lighting
	float specularLight = 0.50f;
	vec3 viewDirection = normalize(camPos - crntPos);
	vec3 reflectionDirection = reflect(-lightDirection, normal);
	float specAmount = pow(max(dot(viewDirection, reflectionDirection), 2.0f), 16);
	float specular = specAmount * specularLight;

	// calculates the intensity of the crntPos based on its angle to the center of the light cone
	float angle = dot(vec3(0.0f, -1.0f, 0.0f), -lightDirection);
	float inten = clamp((angle - outerCone) / (innerCone - outerCone), 0.0f, 1.0f);

	return (texture(tex0, texCoord) * (diffuse * inten + ambient) + texture(tex0, texCoord).r * specular * inten) * lightColor2;
}




void main()
{

	//wynik
	FragColor = spotLight() + direcLight();
}