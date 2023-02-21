#version 330 core
out vec4 fragment_color;

in vec2 v_TexCoord;

uniform sampler2D bg_Texture;
uniform sampler2D cat_Texture;
uniform int OBJECT;
uniform float bg_transparency;

void main(){
	if(OBJECT == 0){ // EMPTY
		fragment_color = vec4(0.8f, 0.8f, 0.8f, bg_transparency);
	}
	else if(OBJECT == 1){ // WALL
		fragment_color = vec4(0.5f, 0.6f, 3.0f, bg_transparency);
	}
	else if(OBJECT == 2){ // CAT
		fragment_color = vec4(0.0f, 0.0f, 0.0f, bg_transparency * 1.1f) * texture(cat_Texture, v_TexCoord);
	}
	else if(OBJECT == 3){ // BACKGROUNDS
		fragment_color = texture(bg_Texture, v_TexCoord);
	}
}