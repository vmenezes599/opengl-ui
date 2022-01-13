#version 150

uniform sampler2D testTexture;

out vec4 color;

void main()
{
  color = texelFetch(testTexture, ivec2(gl_FragCoord.xy), 0);
}