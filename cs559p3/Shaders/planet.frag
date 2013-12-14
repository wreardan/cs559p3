#version 400

in vec3 position; 
in vec3 normal;
flat in vec3 color;
in vec2 UV;
in vec3 positionWorld;


struct LightInfo {
  vec3 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light[8];

out vec4 FragColor;

uniform sampler2D ColorMap;

uniform bool isStar;


const float shininess = 1.0f;

//const vec3 lightIntensity = vec3(1.0f, 1.0f, 1.0f);
//const vec3 lightPosition = vec3(0.0f, 0.0f, 0.0f); //light position in world space


//ambient color
vec3 Ka =  vec3(texture(ColorMap, UV)) * 0.01; //This give a nice reddish "dark side"

//diffuse
vec3 Kd = vec3(texture(ColorMap, UV)) *0.9;


//Phong ADS model
vec3 ads() {
	
	vec3 n = normalize( vec3(normal) ); 
	vec3 s = normalize( Light[0].Position - positionWorld ); //find vector in terms of world space, equivilent to view space
	vec3 v = normalize( vec3(-position) );
	vec3 r = reflect( -s, n );


	vec3 ambient = Light[0].Intensity * Ka;

	vec3 diffuse = Kd * max( dot(s, n), 0.0f);


	vec3 specular = vec3(0.0f, 0.0f, 0.0f);
	if( dot(v, n) > 0.0f) {
		//specular = pow( max( dot(r, v), 0.0f), shininess) * Ks;
	}


	return clamp(ambient + diffuse + specular, 0.0, 1.0);

}


void main() {

	if(isStar) {
		FragColor = texture(ColorMap, UV);
	}else {
		FragColor = vec4(ads(), 1.0f);
	}

	//FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//FragColor = texture(marsSampler, UV);
}