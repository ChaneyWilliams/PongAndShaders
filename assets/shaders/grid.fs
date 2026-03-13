#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec2 TexCoord;
uniform sampler2D MAIN_TEXTURE;
uniform vec4 COLOR;
uniform float TIME;

void main()
{
    vec4 color = vec4(sin(TIME * 2.0), 0.25, cos(TIME *2.0), 1.0);
    vec2 scrollRight = TexCoord - vec2(TIME * 0.5, TIME * 0.5);
    FragColor = texture(MAIN_TEXTURE, scrollRight)  *color ;// * vec4(ourColor, 1.0);
}