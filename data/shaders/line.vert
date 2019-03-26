R""(
#version 130

in vec3 in_vertex;

out vec4 ex_color;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_vertex, 1.0);
    ex_color=gl_Color;
}
)""
