#version 420

subroutine void postProcessEffectType(vec2 pos);
subroutine uniform postProcessEffectType postProcessEffect;

uniform sampler2D ColorMap;
uniform vec2 size;

layout(location = 0) out vec4 FragColor;

//basic texture only version
subroutine( postProcessEffectType )
void drawTextureOnly(vec2 pos)
{
	FragColor = texture2D(ColorMap, pos);
}

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

//predator vision post effect adapted from: http://www.geeks3d.com/20101123/shader-library-predators-thermal-vision-post-processing-filter-glsl/
subroutine( postProcessEffectType )
void predatorVision(vec2 pos)
{
    vec4 texColor = texture2D( ColorMap, pos);
	vec3 tc = vec3(1.0, 1.0, 1.0);

	vec3 c = texColor.xyz;
	vec3 colors[3];
	colors[0] = vec3(0.0, 0.0, 1.0);
	colors[1] = vec3(1.0, 1.0, 0.0);
	colors[2] = vec3(1.0, 0.0, 0.0);
	float lum = dot(vec3(0.30, 0.59, 0.11), c.rgb); //(c.r + c.g + c.b) / 3.0;
	int ix = (lum < 0.5) ? 0:1;
	tc = mix(colors[ix], colors[ix + 1], (lum - float(ix) * 0.5) / 0.5);

    FragColor = vec4(tc, 1.0);
}

//dream vision post effect [http://www.geeks3d.com/20091112/shader-library-dream-vision-post-processing-filter-glsl/]
subroutine( postProcessEffectType )
void dreamVision(vec2 pos)
{

    vec4 texColor = texture2D( ColorMap, pos);

	texColor += texture(ColorMap, pos + 0.001);
	texColor += texture(ColorMap, pos + 0.003);
	texColor += texture(ColorMap, pos + 0.005);
	texColor += texture(ColorMap, pos + 0.007);
	texColor += texture(ColorMap, pos + 0.009);
	texColor += texture(ColorMap, pos + 0.011);

	texColor += texture(ColorMap, pos - 0.001);
	texColor += texture(ColorMap, pos - 0.003);
	texColor += texture(ColorMap, pos - 0.005);
	texColor += texture(ColorMap, pos - 0.007);
	texColor += texture(ColorMap, pos - 0.009);
	texColor += texture(ColorMap, pos - 0.011);

	texColor.rgb = vec3((texColor.r + texColor.g + texColor.b) / 3.0);
	texColor = texColor / 9.5;
	
    FragColor = texColor;
}

//scan lines and faked chromatic aberration 
//inpiration/methods used from: http://blog.jahfer.com/2012/04/02/experimenting-shaders-openframeworks/, 
//http://cpansearch.perl.org/src/CORION/App-VideoMixer-0.02/filters/scanlines.glsl
subroutine( postProcessEffectType )
void scanLinesChromatic(vec2 pos)
{
    float global_pos = (pos.y + 0.2) * 300.0;
    float wave_pos = cos((fract( global_pos ) - 0.5)*3.14);
    vec4 texColor = texture2D(ColorMap, pos );

	texColor.r = texture2D(ColorMap, pos).r;
    texColor.g = texture2D(ColorMap, pos + 0.004).g;
    texColor.b = texture2D(ColorMap, pos - 0.004).b;

    FragColor = mix(vec4(0,0,0,0), texColor, wave_pos);
}

//basic noise effect
subroutine( postProcessEffectType )
void basicNoise(vec2 pos)
{
    vec4 texColor = texture( ColorMap, pos);
	texColor = texColor * fract(sin(dot(pos.xy, vec2(12.9898,78.233))) * 43758.5453) * 4;
    FragColor = texColor;
}

void main()
{
	vec2 pos = vec2(gl_FragCoord) / size;
	postProcessEffect(pos);
}