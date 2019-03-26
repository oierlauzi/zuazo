R""(
#version 130

in vec3     in_vertex;
in vec2     in_uv;

out vec2    ex_uv;
out vec4    ex_color;

void main(){
    gl_Position = gl_ModelViewProjectionMatrix * vec4(in_vertex, 1.0);
    ex_uv=in_uv;
    ex_color=gl_Color;
}
)""
