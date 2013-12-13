#version 420

uniform sampler2D ColorMap;
uniform vec2 size;

layout(location = 0) out vec4 FragColor;

void main()
{
	FragColor = texture2D(ColorMap, vec2(gl_FragCoord) / size);
}