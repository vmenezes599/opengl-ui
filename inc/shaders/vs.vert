#version 150

const vec2 pos[4] =
    vec2[4](vec2(-1.0, 1.0), vec2(-1.0, -1.0), vec2(1.0, 1.0), vec2(1.0, -1.0));

void main() {
  gl_Position = vec4(pos[gl_VertexID], 0.0, 1.0);
}