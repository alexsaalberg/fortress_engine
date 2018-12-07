#version  330 core

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;

layout(location = 0) in vec3 vPosition;
layout(location = 2) in int block_type;

const int BLOCK_AIR = 0;
const int BLOCK_GRASS = 1;
const int BLOCK_STONE = 2;
const int BLOCK_YELLOW = 3;
const int BLOCK_ORANGE = 4;
const int BLOCK_RED = 5;
const int BLOCK_VIOLET = 6;
const int BLOCK_BLUE = 7;
const int BLOCK_BLACK = 8;
const int BLOCK_WHITE = 9;
const int BLOCK_REDOUTLINE = 10;

out vec3 fPosition_World;
out vec4 block_color;

void main()
{
    gl_Position = P * V * M * vec4(vPosition, 1.0); //Move position to camera space
    fPosition_World = (M * vec4(vPosition, 1.0f)).xyz;

	switch(block_type) {
		case BLOCK_GRASS:
			block_color = vec4(0.1f, 0.9f, 0.1f, 1.0f);
			break;
		case BLOCK_STONE:
			block_color = vec4(0.5f, 0.5f, 0.5f, 1.0f);
			break;
		case BLOCK_YELLOW:
			block_color = vec4(1.0f, 1.0f, 0.0f, 1.0f);
			break;
		case BLOCK_ORANGE:
			block_color = vec4(1.0f, 0.5f, 0.0f, 1.0f);
			break;
		case BLOCK_RED:
			block_color = vec4(1.0f, 0.0f, 0.0f, 1.0f);
			break;
		case BLOCK_VIOLET:
			block_color = vec4(0.7f, 0.2f, 0.7f, 1.0f);
			break;
		case BLOCK_BLUE:
			block_color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
			break;
		case BLOCK_BLACK:
			block_color = vec4(0.0f, 0.0f, 0.0f, 1.0f);
			break;
		case BLOCK_WHITE:
			block_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);
			break;
		case BLOCK_REDOUTLINE:
			block_color = vec4(1.0f, 0.0f, 0.0f, 0.5f);
			break;
		default:
			block_color = vec4(0.0f, 1.0f, 1.0f, 1.0f);
			break;
	}
/*
    if(block_type == BLOCK_GRASS) {
        block_color = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    } else if (block_type == BLOCK_STONE) {
        block_color = vec4(0.2f, 0.9f, 0.2f, 1.0f);
    } else if (block_type == BLOCK_REDOUTLINE) {
        block_color = vec4(1.0f, 0.0, 0.0f, 0.5f);
    } else if
	
	} else {
        block_color = vec4(0.2f, 0.3f, 0.3f, 1.0f);
    }
	*/
}

