
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