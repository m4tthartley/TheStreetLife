
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

	float coef_friction = 0.3f; // aprox data from Corvette
	float car_front_area = 2.2f; // aprox data from Corvette
	float air_density = 1.29f;
	float cdrag = 0.5f * coef_friction * car_front_area * air_density;
	float speed = length2(player.velocity);

	float2 drag = make_float2(-cdrag * player.velocity.x * speed,
							  -cdrag * player.velocity.y * speed);
	float2 resistance = mul2f(player.velocity, -cdrag * 30.0f);
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

void player_physics() {
	static float2 velocity = {0};
	static float rpm = 0.0f;
	static int gear = 1;

	if (rain.input.keys['1']) gear = 1, rpm = 0.2f;
	if (rain.input.keys['2']) gear = 2, rpm = 0.2f;
	if (rain.input.keys['3']) gear = 3, rpm = 0.2f;
	if (rain.input.keys['4']) gear = 4, rpm = 0.2f;
	if (rain.input.keys['5']) gear = 5, rpm = 0.2f;
	if (rain.input.keys['6']) gear = 6, rpm = 0.2f;

	float2 rotation = make_float2(sinf(player.rotation), cosf(player.rotation));
	float2 drag = mul2f(mul2(velocity, make_float2(1, 1)), -0.5f);
	//float2 friction = mul2f(velocity, -3.0f);
	rpm += (input.gas*2.0f - 1.0f)*0.01f;
	if (rpm > 1.0f) rpm = 1.0f;
	if (input.gas > 0.5f) {
		if (rpm < 0.2f) rpm = 0.2f;
	} else {
		if (rpm < 0.0f) rpm = 0.0f;
	}
	
	float2 accel = mul2f(rotation, rpm*4.0f*(1.0f/gear_ratios[gear]));
	accel = add2(accel, drag);
	velocity = add2(velocity, mul2f(accel, 1.0f));

	player.pos = add2(player.pos, mul2f(velocity, rain.dt));

	player.rotation += input.steering * 0.01f;

	debug_print("vel %f %f, rpm %f, gear %i \n", velocity.x, velocity.y, rpm, gear);
}
