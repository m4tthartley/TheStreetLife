
float gear_ratios[] = {
	0.0f,
	2.66f, // 1
	1.78f, // 2
	1.30f, // 3
	1.0f,  // 4
	0.74f, // 5
	0.50f, // 6
	2.90f, // reverse
};

#if 0

float differential_ratio = 3.42f;
float transmission_efficiency = 0.7f;
float wheel_radius = 0.34f;

float get_rpm() {
	float rpm = player.rear_wheel_rotation_speed * gear_ratios[1] * differential_ratio * 60.0f / PI2;
	if (rpm < 1000.0f) rpm = 1000.0f;
	return rpm;
}

float drive_force() {
	float torque = get_torque(rpm) * input.gas;
	float drive_force = torque * gear_ratios[1] * differential_ratio * transmission_efficiency / wheel_radius;
}

float2 slip_ratio() {
	return div2(add2f(neg2(player.velocity), player.rear_wheel_rotation_speed*wheel_radius), normalize2(player.velocity));
}

void player_physics() {
	/*float2 steer_dir = normalize2(make_float2(player.rotation.x * cosf(input.steering) + player.rotation.y * sinf(input.steering),
	player.rotation.y * cosf(input.steering) + player.rotation.x * sinf(input.steering)));*/
	float d = player.velocity.x>0.75f && player.velocity.y>0.75f ? length2(player.velocity) : 1.0f;
	//debug_print("d %f \n", d);
	//d = length2(player.velocity);
	float2 steer_dir = normalize2(make_float2(sinf(input.steering), cosf(input.steering)));

	/*glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glBegin(GL_LINES);
	glVertex2f(0.0f, 0.0f);
	float2 end = mul2f(steer_dir, 1.0f);
	glVertex2f(end.x, end.y);
	glEnd();*/

	/*float steer_rad = atan2(player.rotation.x, player.rotation.y) + atan2(steer_dir.x, steer_dir.y);
	steer_dir = normalize2(make_float2(sinf(steer_rad), cosf(steer_rad)));*/

	float coef_friction = 0.6f; // aprox data from Corvette
	float car_front_area = 2.2f; // aprox data from Corvette
	float air_density = 1.45f;
	float cdrag = 0.04f * coef_friction * car_front_area * air_density;
	float speed = length2(player.velocity);

	float2 drag = make_float2(-cdrag * player.velocity.x * speed,
							  -cdrag * player.velocity.y * speed);
	float2 resistance = mul2f(player.velocity, -cdrag * 40.0f);
	float2 traction = mul2f(player.rotation, 100.0f * input.gas); // random number
	float2 force = add2(add2(traction, drag), resistance);
	float2 a = mul2f(force, rain.dt);
	player.velocity = add2(player.velocity, a);

	
	float front_rad = atan2(player.velocity.x, player.velocity.y) + atan2(steer_dir.x, steer_dir.y);
	float2 front_velocity = mul2f(normalize2(make_float2(sinf(front_rad), cosf(front_rad))), length2(player.velocity));
	
	
	

	/*player.front_pos = add2(player.front_pos, mul2(steer_dir, mul2f(player.velocity, 1.0f)));
	player.back_pos = add2(player.back_pos, mul2(player.rotation, mul2f(player.velocity, 1.0f)));*/
	player.front_pos = add2(player.front_pos, mul2f(front_velocity, rain.dt));
	player.back_pos = add2(player.back_pos, mul2f(player.velocity, rain.dt));

	player.rotation = normalize2(sub2(player.front_pos, player.back_pos));
	player.center = div2f(add2(player.front_pos, player.back_pos), 2.0f);
	player.front_pos = add2(player.center, mul2f(player.rotation, player.length/2.0f));
	player.back_pos = sub2(player.center, mul2f(player.rotation, player.length/2.0f));
}
#endif

// Fuck physics

/*
	THE DRIFT BIBLE
	by Josh Parrett

	current angle = direction of velocity

	bigger difference between speed and velocity
	more angle added to current angle

	less difference between speed and velocity
	reduce angle added to current angle

	bigger difference between speed and velocity
	slower acceleration rate

	less difference between speed and velocity
	higher acceleration rate

	higher velocity
	faster turn rate

	lower velocity
	slower turn rate

	no turn
	turn = current angle

	clutch off
	speed = velocity

	clutch on
	speed = gears[gear] * (revs / limit)
*/

float gear_speeds[] = { 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f }; // meters per second

void player_physics() {
	static float2 velocity = {0};
	static float rpm = 0.1f;
	static float wheel_speed = 0.0f;
	static int gear = 1;
	static float wheel_dir;

	float2 rotation = make_float2(sinf(player.rotation), cosf(player.rotation));

	// Velocity
	if (rain.keys[KEY_E].pressed) {
		gear = min(gear+1, 6);
		rpm = wheel_speed/gear_speeds[gear];
	}
	if (rain.keys[KEY_Q].pressed) {
		gear = max(gear-1, 1);
		rpm = wheel_speed/gear_speeds[gear];
	}

	if (input.gas > 0.5f) {
		rpm *= 1.0f + (20.0f/gear_speeds[gear])*rain.dt;
		rpm = min(rpm, 1.0f);
	} else {
		rpm *= 1.0f - (0.8f*rain.dt);
		rpm = max(rpm, 0.1f);
	}

	wheel_speed = gear_speeds[gear] * (rpm);
	float velocity_scalar = length2(velocity);
	float diff = min(wheel_speed-velocity_scalar, 70.0f);
	float accel;
	/* if (diff > 0.0f) accel = 1.0f - (diff)/60.0f;
	else accel = (diff*7.0f)/60.0f; */
    
    accel = (diff*7.0f)/50.0f;

	float slip = max(min(diff, 7.5f), 0.0f) / 7.5f;

	velocity = add2(velocity, mul2f(rotation, accel * (1.0f-(slip*0.5f)) * 5.0f * rain.dt));

	/*float2 drag = mul2f(velocity, -0.1f * rain.dt);
	velocity = add2(velocity, drag);*/

	float2 breaking = mul2f(velocity, input.reverse * -1.0f * rain.dt);
	velocity = add2(velocity, breaking);

	player.pos = add2(player.pos, mul2f(velocity, rain.dt));

	// Rotation
	wheel_dir += (input.steering*(4.5f - velocity_scalar/gear_speeds[6]))*rain.dt;
	if (input.steering > -0.5f && input.steering < 0.5f) {
		if (wheel_dir > 0.0f) wheel_dir -= (3.5f - velocity_scalar/gear_speeds[6])*rain.dt;
		else wheel_dir += (3.5f - velocity_scalar/gear_speeds[6])*rain.dt;
	}
//	wheel_dir = min(wheel_dir, 0.2f / (slip / (velocity_scalar * 2.0f)));
//	wheel_dir = max(wheel_dir, -0.2f / (slip / (velocity_scalar * 2.0f)));
    wheel_dir = min(wheel_dir, 0.1f * (velocity_scalar * slip));
    wheel_dir = max(wheel_dir, -0.1f * (velocity_scalar * slip));
	player.rotation += (wheel_dir*velocity_scalar*0.01f)*rain.dt;
    
	// Readjust velocity
	rotation = make_float2(sinf(player.rotation), cosf(player.rotation));
    
    float direction = atan2(velocity.x, velocity.y);
	if (player.rotation < direction-PI) player.rotation += PI2;
	if (player.rotation > direction+PI) player.rotation -= PI2;
    player.rotation += wheel_dir * (slip / 40.0f); // MATT FIX THIS, it almost works for going right

	float rot_diff = dot2(rotation, normalize2(velocity));
	velocity = mul2f(lerp2(normalize2(velocity), rotation, 1.0f-slip), length2(velocity));

	debug_print("vel %f %f, rpm %f, wheel_speed %f, gear %i, slip %f, rotation %f \n", velocity.x, velocity.y, rpm, wheel_speed, gear, slip, player.rotation);

	// Visualisations
	{
		glPushMatrix();
		glTranslatef(4.0f, 0.0f, 0.0f);
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f);
		glVertex2f(0.2f, -2.0f);
		glVertex2f(0.2f, 2.0f);
		glVertex2f(-0.2f, 2.0f);
		glEnd();

		glTranslatef(0.0f, -2.0f+(rpm*2.0f), 0.0f);
		glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f*rpm);
		glVertex2f(0.2f, -2.0f*rpm);
		glVertex2f(0.2f, 2.0f*rpm);
		glVertex2f(-0.2f, 2.0f*rpm);
		glEnd();
		glPopMatrix();
	}
	{
		float f = wheel_speed/gear_speeds[6];

		glPushMatrix();
		glTranslatef(4.4f, 0.0f, 0.0f);
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f);
		glVertex2f(0.2f, -2.0f);
		glVertex2f(0.2f, 2.0f);
		glVertex2f(-0.2f, 2.0f);
		glEnd();

		glTranslatef(0.0f, -2.0f+(f*2.0f), 0.0f);
		glColor4f(0.5f, 0.0f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f*f);
		glVertex2f(0.2f, -2.0f*f);
		glVertex2f(0.2f, 2.0f*f);
		glVertex2f(-0.2f, 2.0f*f);
		glEnd();
		glPopMatrix();
	}
	{
		float f = velocity_scalar/gear_speeds[6];

		glPushMatrix();
		glTranslatef(4.8f, 0.0f, 0.0f);
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f);
		glVertex2f(0.2f, -2.0f);
		glVertex2f(0.2f, 2.0f);
		glVertex2f(-0.2f, 2.0f);
		glEnd();

		glTranslatef(0.0f, -2.0f+(f*2.0f), 0.0f);
		glColor4f(0.0f, 0.0f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f*f);
		glVertex2f(0.2f, -2.0f*f);
		glVertex2f(0.2f, 2.0f*f);
		glVertex2f(-0.2f, 2.0f*f);
		glEnd();
		glPopMatrix();
	}
	{
		float f = wheel_dir;

		glPushMatrix();
		glTranslatef(0.0f, 4.0f, 0.0f);
		glRotatef(90, 0, 0, -1);
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -2.0f);
		glVertex2f(0.2f, -2.0f);
		glVertex2f(0.2f, 2.0f);
		glVertex2f(-0.2f, 2.0f);
		glEnd();

		glTranslatef(0.0f, (f*1.0f), 0.0f);
		glColor4f(0.0f, 0.5f, 0.0f, 1.0f);
		glBegin(GL_QUADS);
		glVertex2f(-0.2f, -1.0f*f);
		glVertex2f(0.2f, -1.0f*f);
		glVertex2f(0.2f, 1.0f*f);
		glVertex2f(-0.2f, 1.0f*f);
		glEnd();
		glPopMatrix();
	}

}

#if 0
void player_physics() {
	static float2 velocity = {0};
	static float rpm = 0.0f;
	static float drive_shaft_speed = 0.0f;
	static float wheel_speed = 0.0f;
	static int gear = 1;

	if (rain.keys[KEY_1].pressed) drive_shaft_speed = (gear_ratios[1]/gear_ratios[gear])*drive_shaft_speed, gear = 1;
	if (rain.keys[KEY_2].pressed) drive_shaft_speed = (gear_ratios[2]/gear_ratios[gear])*drive_shaft_speed, gear = 2;
	if (rain.keys[KEY_3].pressed) drive_shaft_speed = (gear_ratios[3]/gear_ratios[gear])*drive_shaft_speed, gear = 3;
	if (rain.keys[KEY_4].pressed) drive_shaft_speed = (gear_ratios[4]/gear_ratios[gear])*drive_shaft_speed, gear = 4;
	if (rain.keys[KEY_5].pressed) drive_shaft_speed = (gear_ratios[5]/gear_ratios[gear])*drive_shaft_speed, gear = 5;
	if (rain.keys[KEY_6].pressed) drive_shaft_speed = (gear_ratios[6]/gear_ratios[gear])*drive_shaft_speed, gear = 6;

	float gear_ratio = gear_ratios[gear];

	float2 rotation = make_float2(sinf(player.rotation), cosf(player.rotation));
	float2 drag = mul2f(mul2(velocity, make_float2(1, 1)), -0.5f);
	//float2 friction = mul2f(velocity, -3.0f);
	rpm += (input.gas*2.0f - 1.0f)*4.0f * rain.dt;
	if (rpm > 1.0f) rpm = 1.0f;
	if (input.gas > 0.5f) {
		if (rpm < 0.2f) rpm = 0.2f;
	} else {
		if (rpm < 0.0f) rpm = 0.0f;
	}
	
	drive_shaft_speed += (rpm - drive_shaft_speed)*(((drive_shaft_speed-drive_shaft_speed/2.0f)+0.1f)*gear_ratio)*10.0f*rain.dt;
	rpm -= (rpm - (drive_shaft_speed))*20.0f*rain.dt;

	float2 accel = mul2f(rotation, drive_shaft_speed/gear_ratio * 10.0f);
	accel = add2(accel, drag);
	velocity = add2(velocity, mul2f(accel, rain.dt));
	/*wheel_speed = drive_shaft_speed / gear_ratio;
	velocity = add2(velocity, mul2f(rotation, wheel_speed - length2(velocity)));*/

	player.pos = add2(player.pos, mul2f(velocity, rain.dt));

	player.rotation += input.steering * 0.01f;

	debug_print("vel %f %f, rpm %f, drive_shaft_speed %f, gear %i \n", velocity.x, velocity.y, rpm, drive_shaft_speed, gear);
}
#endif
