#version 420

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 Texture;

struct LightInfo {
  vec4 Position;  // Light position in eye coords.
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
uniform sampler2D TextureID;

layout(location = 0) out vec4 FragColor;

vec3 phongModel( vec3 pos, vec3 norm ) {
    vec3 light_sum;
    for(int i = 0; i < 8; i++) {
        vec3 s = normalize(vec3(Light[i].Position) - pos);
        vec3 v = normalize(-pos.xyz);
        vec3 r = reflect( -s, norm );
        vec3 ambient = Light[i].Intensity * Material.Ka;
        float sDotN = max( dot(s,norm), 0.0 );
        vec3 diffuse = Light[i].Intensity * Material.Kd * sDotN;
        vec3 spec = vec3(0.0);
        if( sDotN > 0.0 )
            spec = Light[i].Intensity * Material.Ks *
                   pow( max( dot(r,v), 0.0 ), Material.Shininess );
        light_sum += ambient + diffuse + spec;
    }
    return light_sum;
}
    	
void main(void) {
    vec3 phong = phongModel(Position, Normal);
//    vec4 t_color = texture2D(TextureID, vec2(Texture));
//    gl_FragColor = vec4(phong, 1.0) * t_color;
    FragColor = vec4(1.0) * vec4(phong, 1.0);
}