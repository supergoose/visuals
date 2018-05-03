#version 150 core

#define PI 3.1415926535897932384626433832795

uniform vec3 uBoundaryPos;
uniform float uBoundaryRadiusSquared;

in vec3 iPosition;
in float iDirection;
in float iSpeed;
in vec4 iColor;

out vec3 position;
out float direction;
out float speed;
out vec4 color;

void main()
{
	position = iPosition;
	direction = iDirection;
	speed = iSpeed;
	color = iColor;

	float distSqrd = (position.x-uBoundaryPos.x)*(position.x-uBoundaryPos.x)+(position.y-uBoundaryPos.y)*(position.y-uBoundaryPos.y);
	if(distSqrd > uBoundaryRadiusSquared)
	{
		float incidence = atan(position.y-uBoundaryPos.y, position.x-uBoundaryPos.x);
		float normal = incidence+PI;
      
		float reflection = direction - normal;
		direction -= reflection;
	}

	position += vec3(cos(direction)*speed, sin(direction)*speed, 0.0f);

}