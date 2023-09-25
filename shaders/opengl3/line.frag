// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#version 400 core

out vec4 fragment;
  
in vec3 vert_color;

void main() {
    fragment = vec4(vert_color, 1.0);
}