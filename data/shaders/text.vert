#version 130

in vec4     in_vertex;

out vec2    ex_uv;

void main(){
    gl_Position =gl_ModelViewProjectionMatrix * vec4(in_vertex.x, in_vertex.y, 0.0, 1.0);
    ex_uv=vec2(in_vertex.z, in_vertex.w);
}
