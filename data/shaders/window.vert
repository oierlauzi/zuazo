R""(
#version 130

in vec2     in_vertex;
in vec2     in_uv;

out vec2    ex_uv;

void main(){
    gl_Position = vec4(in_vertex, 0.0, 1.0);
    ex_uv=in_uv;
}
)""
