#version 420

subroutine void postProcessEffectType(vec2 pos);
subroutine uniform postProcessEffectType postProcessEffect;

uniform sampler2D ColorMap;
uniform vec2 size;

layout(location = 0) out vec4 FragColor;

//toony, posterize post effect adapted from:http://www.geeks3d.com/20091027/shader-library-posterization-post-processing-effect-glsl/
subroutine( postProcessEffectType )
void drawPosterized(vec2 pos) {
    vec4 texColor = texture2D( ColorMap, pos);
	vec3 c = texColor.xyz;
	c = pow(c, vec3(0.6, 0.6, 0.6));
	c = c * 8;
	c = floor(c);
	c = c / 8;
	c = pow(c, vec3(1.0/0.6));
	texColor = vec4(c.x, c.y, c.z, 1.0);
    FragColor = texColor;
}

//basic texture only version
subroutine( postProcessEffectType )
void drawTextureOnly(vec2 pos)
{
	FragColor = texture2D(ColorMap, pos);
}

void main()
{
	vec2 pos = vec2(gl_FragCoord) / size;
	postProcessEffect(pos);
}