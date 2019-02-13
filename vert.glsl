attribute vec3 pos;

varying vec3 p;

void main()
{
	gl_Position = vec4(pos, 1.0);

	p = pos;
}
