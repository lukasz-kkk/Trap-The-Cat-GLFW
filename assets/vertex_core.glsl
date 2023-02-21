#version 330 core

layout (location = 0) in vec2 v_pos;
layout (location = 1) in vec2 texCoord;

uniform float POSITION;
uniform int OBJECT;
uniform mat4 transform;

out vec2 v_TexCoord;

#define GRID_COUNT_X 11
#define GRID_COUNT_Y 11

void main(){
	v_TexCoord = texCoord;

	// BACKGROUND
	if(OBJECT == 3){
		gl_Position = vec4(v_pos, 0.0, 1.0);
		return;
	}
	// CAT
	if(OBJECT == 2){
		gl_Position = transform * vec4(v_pos.x + 0.33, v_pos.y + 0.4, 0.0, 6.5);
		return;
	}

	float offset_x = -0.875 + (int(POSITION / 11) % 2) * 0.077;
	float offset_y = floor(POSITION / 11) * 0.04 + 0.66;
	
	vec4 new_position = vec4(
	  (1.7 / 11) * (mod(POSITION, 11) + v_pos.x) + offset_x,
	 -(1.7 / 11) * (floor(POSITION / 11) + v_pos.y) + offset_y,
	  0.0, 1.0);

	 gl_Position = new_position;
} 