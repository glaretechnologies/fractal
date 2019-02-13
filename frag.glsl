uniform float time;
uniform vec3 col0; // colour 0
uniform vec3 col1; // colour 1
uniform vec3 col2; // colour 2
uniform vec2 c; // Complex number that controls the Julia fractal
uniform vec2 trap; // Trap parameters

varying vec3 p; // position

// See https://en.wikipedia.org/wiki/Orbit_trap for the trap colouring technique

// Interpolate in 'RYB' colour space, see 'Paint Inspired Color Mixing and Compositing for Visualization', 
// http://vis.computer.org/vis2004/DVD/infovis/papers/gossett.pdf
// We use this colour space because it looks better for interpolation than RGB.
vec3 lerpedCol(float x, float y, float z)
{
	float onex = 1.0 - x;
	float oney = 1.0 - y;
	float onez = 1.0 - z;

	return 
		vec3(1.0, 1.0, 1.0) * (onex * oney * onez) + // white
		vec3(1.0, 0.0, 0.0) * (x * oney * onez) + // red
		vec3(1.0, 1.0, 0.0) * (onex * y * onez) + // yellow 
		vec3(1.0, 0.5, 0.0) * (x * y * onez) + // orange
		vec3(0.163, 0.373, 0.6) * (onex * oney * z) + // blue
		vec3(0.5, 0.0, 0.5)  * (x * oney * z) + // purple
		vec3(0.0, 0.66, 0.2) * (onex * y * z) + // green
		vec3(0.0, 0.0, 0.0) * (x * y * z); // black
		//vec3(0.2, 0.094, 0.0) * (x * y * z); // brown
}

float square(float x) { return x*x; }

void main()
{
	float zoom_scale = 3.0;
	
	float x = p.x * zoom_scale;
	float y = p.y * zoom_scale * 0.6; // Screen aspect ratio adjustment!

	float circle_trap_r2 = trap.x; // radius^2 of circule trap
	float trapmix_t_1 = trap.y; // Blend parameter between line and circle traps.

	vec3 col = vec3(1.0);

	float closest_line = 1.0e30;
	float closest_circ = 1.0e30;
	float closest_point = 1.0e30;
	for(int i=0; i<256/*16*/; i++)
	{
		float newx = x*x - y*y + c.x;
		y = 2.0*x*y + c.y;
		x = newx;

		float x2 = x*x;
		float y2 = y*y;
		float r2 = x2 + y2;

		if(r2 < 100000.0)
		{
			float line_dist = min(sqrt(x2), sqrt(y2)) * 5.0 + r2 * 0.01; // (something similar to) distance to x and y axes
			float circ_dist = abs(r2 - circle_trap_r2); // (something similar to) distance to circle
			float grid_dist = min(min(
					(square(fract(x)      ) + square(fract(y       ))),
					(square(fract(x) - 1.0) + square(fract(y       )))
				),
				min(
					(square(fract(x)      ) + square(fract(y) - 1.0)),
					(square(fract(x) - 1.0) + square(fract(y) - 1.0))
				));

			// Nudge colour towards trap colours, based on how close point is to the various traps.
			float nudge_factor = 0.25;
			float line_nudge  = 2.0 * sqrt(min(1.0, 0.05 / (line_dist)));
			float circ_nudge  = 3.0 * sqrt(min(1.0, 0.05 / circ_dist));
			float point_nudge = 1.0 * sqrt(min(1.0, 0.05 / (r2 * r2 * grid_dist)));
			col += nudge_factor * ((col0 - col) * line_nudge + (col1 - col) * circ_nudge + (col2 - col) * point_nudge);
		}
	}

	vec3 col_rgb = lerpedCol(col.x, col.y, col.z);
	gl_FragColor = vec4(col_rgb, 1.0);
}
