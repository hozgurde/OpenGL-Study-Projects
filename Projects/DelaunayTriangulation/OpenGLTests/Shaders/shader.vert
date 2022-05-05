#version 330													

uniform mat4 model;
uniform mat4 projection;
uniform int myColor;
layout (location = 0) in vec3 pos;

out vec4 vColor;
void main(){
	gl_Position = model * vec4(pos.x, pos.y, pos.z, 1.0);
	
	if(myColor == 0){
		vColor = vec4(0.0, 0.0, 0.0, 1.0);
		gl_PointSize = 3.0;
	}else if(myColor == 1){
		vColor = vec4(1.0, 0.0, 0.0, 1.0);
		gl_PointSize = 6.0;
	}else{
		vColor = vec4(0.0, 0.0, 1.0, 1.0);
	}
	
}