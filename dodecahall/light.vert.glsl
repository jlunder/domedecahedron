uniform mat4 u_viewProjectionMatrix;
uniform mat4 u_modelMatrix;

attribute vec4 a_vertex;

varying vec4 v_color;
varying vec2 v_texCoord;

void main(void)
{
	gl_Position = u_viewProjectionMatrix * u_modelMatrix * a_vertex;
	//gl_Position = vec4(a_texCoord.x, a_texCoord.y, 0.0, 0.0);
	v_texCoord = a_vertex.xy;
}
