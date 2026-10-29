#pragma once

namespace Shaders {

const char* basicVtx = R"(

#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec2 aUv;
layout(location = 2) in mat4 model;

out vec2 uv;
out vec4 outColor;

uniform mat4 view;
uniform mat4 projection;
uniform vec4 color;

void main() {
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	uv = aUv;
	outColor = color;
}

)";

const char* basicFrag = R"(

#version 330 core

in vec2 uv;
in vec4 outColor;
out vec4 fragColor;

uniform sampler2D tex;

void main() {
	fragColor = texture(tex, uv) * outColor;
}

)";

}