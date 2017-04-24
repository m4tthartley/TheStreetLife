
struct {
	float gas;
	float reverse;
	float steering;
} input;

struct {
	float2 front_pos;
	float2 back_pos;
	float2 center;
	float2 rotation;
	float2 velocity;
	float length;
} player;

float2 camera = {0};

#include "physx.c"

void gfx_draw_rect(float x, float y, float w, float h, float rotation) {
	float w12 = w/2.0f;
	float h12 = h/2.0f;
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation/PI * 180.0f, 0.0f, 0.0f, -1.0f); // note: negative z to get the direction I want
	glBegin(GL_QUADS);
		glVertex2f(-w12, -h12);
		glVertex2f( w12, -h12);
		glVertex2f( w12,  h12);
		glVertex2f(-w12,  h12);
	glEnd();
	glPopMatrix();
}

void gfx_draw_sprite(GLuint tex, float x, float y, float w, float h, float rotation) {
	float w12 = w/2.0f;
	float h12 = h/2.0f;
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, tex);
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation/PI * 180.0f, 0.0f, 0.0f, -1.0f); // note: negative z to get the direction I want
	glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(-w12, -h12);
		glTexCoord2f(1, 1); glVertex2f(w12, -h12);
		glTexCoord2f(1, 0); glVertex2f(w12, h12);
		glTexCoord2f(0, 0); glVertex2f(-w12, h12);
	glEnd();
	glPopMatrix();
	glDisable(GL_TEXTURE_2D);
}

float screen_world_height;
float screen_world_height_half;
float screen_world_width;
float screen_world_width_half;

GLuint car_texture;

Sound test;

void init() {
	screen_world_height = 25.0f;
	screen_world_height_half = screen_world_height/2.0f;
	screen_world_width = (float)window_width/(float)window_height * screen_world_height;
	screen_world_width_half = screen_world_width/2.0f;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-screen_world_width_half,
			screen_world_width_half,
			-screen_world_height_half,
			screen_world_height_half,
			10.0f, -10.0f);
	glMatrixMode(GL_MODELVIEW);

	player.rotation = make_float2(0.0f, 1.0f);
	player.length = 2.5f;
	player.front_pos = make_float2(0.0f, player.length*0.5f);
	player.back_pos = make_float2(0.0f, -player.length*0.5f);

	car_texture = load_image("ps13.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
}

void update() {
	input.gas = rain.input.keys[KEYBOARD_UP];
	input.reverse = rain.input.keys[KEYBOARD_DOWN];
	input.steering = -rain.input.keys[KEYBOARD_LEFT] + rain.input.keys[KEYBOARD_RIGHT];

	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	float w = screen_world_width_half;
	float h = screen_world_height_half;
	float scale = 3.0f;
	glPushMatrix();
	glTranslatef(-fmod(camera.x, scale), -fmod(camera.y, scale), 0);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < (h/scale)+1; ++i) {
		glVertex2f(-w-scale, i*scale);
		glVertex2f(w+scale, i*scale);
		if (i) {
			glVertex2f(-w-scale, -i*scale);
			glVertex2f(w+scale, -i*scale);
		}
	}
	for (int i = 0; i < (w/scale)+1; ++i) {
		glVertex2f(i*scale, -h-scale);
		glVertex2f(i*scale, h+scale);
		if (i) {
			glVertex2f(-i*scale, -h-scale);
			glVertex2f(-i*scale, h+scale);
		}
	}
	glEnd();
	glPopMatrix();
		
	player_physics();

	//float2 back_dir = make_float2(sinf(player.rotation), cosf(player.rotation));
	//player.pos = add2(player.pos, mul2f(, input.gas*0.05f));
	//player.rotation += input.steering*0.02f;
	
	//float2 player_center = div2f(add2(player.front_pos, player.back_pos), 2.0f);
	camera = player.center;
	
//	camera.y += input.gas*0.1f;
//	camera.y -= input.reverse*0.1f;
//	camera.x += input.steering*0.1f;
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	//gfx_draw_rect(player.front_pos.x - camera.x, player.front_pos.y - camera.y, 0.7f, 2.0f, 0/*player.rotation*/);

	gfx_draw_sprite(car_texture, player.center.x - camera.x, player.center.y - camera.y, (player.length*1.8f)/2.0f, player.length*1.8f, atan2(player.rotation.x, player.rotation.y));

	glPushMatrix();
	glTranslatef(-camera.x, -camera.y, 0.0f);
	glPointSize(4.0f);
	glBegin(GL_POINTS);
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	glVertex2f(player.front_pos.x, player.front_pos.y);
	glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
	glVertex2f(player.back_pos.x, player.back_pos.y);
	glEnd();
	glPopMatrix();
}