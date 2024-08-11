#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

out vec4 fragment;
  
in vec3 vert_color;

void main() {
    fragment = vec4(vert_color, 1.0);
}