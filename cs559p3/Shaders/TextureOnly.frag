#version 420

in vec2 TextureCoord;

uniform sampler2D ColorMap;

layout(location = 0) out vec4 FragColor;

void main()
{
	//FragColor = vec4(1.0);

	FragColor = texture2D(ColorMap, TextureCoord);
}