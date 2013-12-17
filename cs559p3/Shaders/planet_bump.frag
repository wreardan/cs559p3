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
uniform sampler2D NormalMap;

uniform bool isStar;


const float shininess = 1.0f;

//const vec3 lightIntensity = vec3(1.0f, 1.0f, 1.0f);
//const vec3 lightPosition = vec3(0.0f, 0.0f, 0.0f); //light position in world space


//ambient color
vec3 Ka =  vec3(texture(ColorMap, UV)) * 0.01; //This give a nice reddish "dark side"

//diffuse
vec3 Kd = vec3(texture(ColorMap, UV)) *0.9;

//Normal Mapping without Tangents
//http://www.thetenthplanet.de/archives/1180
mat3 cotangent_frame( vec3 N, vec3 p, vec2 uv )
{
    // get edge vectors of the pixel triangle
    vec3 dp1 = dFdx( p );
    vec3 dp2 = dFdy( p );
    vec2 duv1 = dFdx( uv );
    vec2 duv2 = dFdy( uv );
 
    // solve the linear system
    vec3 dp2perp = cross( dp2, N );
    vec3 dp1perp = cross( N, dp1 );
    vec3 T = dp2perp * duv1.x + dp1perp * duv2.x;
    vec3 B = dp2perp * duv1.y + dp1perp * duv2.y;
 
    // construct a scale-invariant frame 
    float invmax = inversesqrt( max( dot(T,T), dot(B,B) ) );
    return mat3( T * invmax, B * invmax, N );
}

vec3 perturb_normal( vec3 N, vec3 V, vec2 texcoord )
{
    // assume N, the interpolated vertex normal and 
    // V, the view vector (vertex to eye)
    vec3 map = texture2D( NormalMap, texcoord ).xyz;
#ifdef WITH_NORMALMAP_UNSIGNED
    map = map * 255./127. - 128./127.;
#endif
#ifdef WITH_NORMALMAP_2CHANNEL
    map.z = sqrt( 1. - dot( map.xy, map.xy ) );
#endif
#ifdef WITH_NORMALMAP_GREEN_UP
    map.y = -map.y;
#endif
    mat3 TBN = cotangent_frame( N, -V, texcoord );
    return normalize( TBN * map );
}


//Phong ADS model
vec3 ads(vec3 pos, vec3 norm) {
	
	vec3 n = normalize( vec3(norm) ); 
	vec3 s = normalize( Light[0].Position - positionWorld ); //find vector in terms of world space, equivilent to view space
	vec3 v = normalize( vec3(-pos) );
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
	vec3 norm = normal;
	norm = perturb_normal(norm, position, UV);

	if(isStar) {
		FragColor = texture(ColorMap, UV);
	}else {
		FragColor = vec4(ads(position, norm), 1.0f);
	}

	//FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
	//FragColor = texture(marsSampler, UV);
}