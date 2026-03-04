#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;

void main()
{
   FragColor = texture(MAIN_TEXTURE, TexCoord) *COLOR ;// * vec4(ourColor, 1.0);
}