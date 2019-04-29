R""(
#version 300 es

in vec3     in_vertex;
in vec2     in_uv;

out vec2    ex_uv;

uniform model{
    mat4 modelMat;
};

uniform view{
    mat4 viewMat;
};

uniform projection{
    mat4 projectionMat;
};

void main(){
    gl_Position = projectionMat * viewMat * modelMat * vec4(in_vertex, 1.0);
    ex_uv=in_uv;
}
)""
