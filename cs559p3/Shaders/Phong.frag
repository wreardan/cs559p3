#version 420

in vec3 Position;
in vec3 Normal;
in vec2 TextureCoord;
in vec3 WorldPosition;

struct LightInfo {
  vec3 Position;  // Light position in eye coords.
  vec3 Intensity; // A,D,S intensity
};
uniform LightInfo Light[8];

struct MaterialInfo {
  vec3 Ka;            // Ambient reflectivity
  vec3 Kd;            // Diffuse reflectivity
  vec3 Ks;            // Specular reflectivity
  float Shininess;    // Specular shininess factor
};
uniform MaterialInfo Material;
uniform sampler2D ColorMap;
uniform sampler2D NormalMap;
uniform sampler2D SpecularMap;

layout(location = 0) out vec4 FragColor;

vec3 phongModel( vec3 pos, vec3 norm ) {
    vec3 light_sum = vec3(0,0,0);
	vec3 texSpec = texture2D(SpecularMap, TextureCoord).rgb;
	//vec3 texSpec = Material.Ks;
    for(int i = 0; i < 8; i++) {
        vec3 s = normalize(Light[i].Position - pos);
        //vec3 v = normalize(-pos.xyz);
		vec3 v = normalize( -Position );
        vec3 r = reflect( -s, norm );
        vec3 ambient = Light[i].Intensity * Material.Ka;
        float sDotN = max( dot(s,norm), 0.0 );
        vec3 diffuse = Light[i].Intensity * Material.Kd * sDotN;
        vec3 spec = vec3(0.0);
        if( sDotN > 0.0 )
            spec = Light[i].Intensity * texSpec *
                   pow( max( dot(r,v), 0.0 ), Material.Shininess );
        light_sum += ambient + diffuse + spec;
    }
    return light_sum;
}
    	
void main(void) {

    vec3 phong = phongModel(WorldPosition, Normal);
    vec4 t_color = texture2D(ColorMap, TextureCoord);
    FragColor = vec4(phong, 1.0) * t_color;
}