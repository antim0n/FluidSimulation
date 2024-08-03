#version 330 core

layout(origin_upper_left, pixel_center_integer) in vec4 gl_FragCoord;

uniform float threshold;
uniform float radius;
uniform vec2 particle_positions[600];
uniform vec2 particle_velocities[600];

void main()
{
	gl_FragColor = vec4(0, 0, 0, 0);
	float metaball = 0;
	float velocity_x = 0;
    float velocity_y = 0;
	int counter = 0;
	for(int i = 0; i < 600; i++)
	{
        float d = distance(vec2(gl_FragCoord), particle_positions[i]);
		if(d < radius)
		{
			float diffx = vec2(gl_FragCoord).x - particle_positions[i].x;
			float diffy = vec2(gl_FragCoord).y - particle_positions[i].y;
			metaball += 1 / (diffx * diffx + diffy * diffy);
		}
        if(d < radius)
        {
            velocity_x += particle_velocities[i].x;
            velocity_y += particle_velocities[i].y;
			counter++;
        }
	}

	if(counter > 0)
	{
		velocity_x = velocity_x / counter;
        velocity_y = velocity_y / counter;
	}

	if(metaball > threshold)
	{
        vec3 c = vec3(0, 0, 1.0);
		//vec3 color1;
        //vec3 color2;
        //float t = min(1.f, sqrt(velocity_x * velocity_x + velocity_y * velocity_y) / 2.5f);
        //if (t < 0.11)
        //{
        //    t = (t - 0.0f) / 0.11f;
        //    color1 = vec3(0, 0, 1.0);
        //    color2 = vec3(0, 1.0, 1.0);
        //}
        //else if (t < 0.27)
        //{
        //    t = (t - 0.11f) / 0.16f;
        //    color1 = vec3(0, 1.0, 1.0);
        //    color2 = vec3(0, 1.0, 0);
        //}
        //else if (t < 0.36f)
        //{
        //    t = (t - 0.27f) / 0.09f;
        //    color1 = vec3(0, 1.0, 0);
        //    color2 = vec3(1.0, 1.0, 0);
        //}
        //else
        //{
        //    t = (t - 0.36f) / 0.64f;
        //    color1 = vec3(1.0, 1.0, 0);
        //    color2 = vec3(1.0, 0, 0);
        //}
        //
        //c.x = min(color1.x + ((color2.x - color1.x) * t), 255.f);
        //c.y = min(color1.y + ((color2.y - color1.y) * t), 255.f);
        //c.z = min(color1.z + ((color2.z - color1.z) * t), 255.f);

		gl_FragColor = vec4(c, 1.0);
	}
}