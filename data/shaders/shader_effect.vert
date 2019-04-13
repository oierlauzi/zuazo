R""(
#version 130

in vec2     in_vertices;
in vec2     in_texCoords;

out vec2    ex_texCoords;

void main(){
    gl_Position = vec4(in_vertices, 0.0, 1.0);
    ex_texCoords=in_texCoords;
}
)""
