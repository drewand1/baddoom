#version 330 core

layout(location = 0) in vec2 aPos
layout(location = 1) in vec4 aColor
layout(location = 2) in vec2 uv

out vec4 bColor

void main() {
	gl_Position = vec4(aPos, 0, 1.0);
	bColor = aColor;
}