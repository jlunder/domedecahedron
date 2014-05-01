precision lowp float;

uniform vec4 u_color;

varying vec2 v_texCoord;

void main(void)
{
	gl_FragColor = u_color * max(0.0, 1.0 - length(v_texCoord));
}
