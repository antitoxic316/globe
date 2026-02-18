#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform float R;

uniform vec2 globeAngles;
uniform vec4 rotQuaternion;

out vec3 ourColor;

#define PI 3.1415926538

void main()
{
  float lon = PI / 180.0f * globeAngles.x;
  float lat = PI / 180.0f * globeAngles.y;

  vec3 globePos;
  globePos.x = R * cos(lat) * cos(lon); 
  globePos.y = R * sin(lat);
  globePos.z = R * cos(lat) * sin(lon);

	globePos = globePos + 2.0 * cross(rotQuaternion.xyz, cross(rotQuaternion.xyz, globePos) + rotQuaternion.w * globePos);

  gl_Position = projection * model * vec4(aPos+globePos, 1.0);
	ourColor = vec3(gl_Position.x, gl_Position.y, gl_Position.z);
}
