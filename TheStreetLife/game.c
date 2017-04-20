
struct {
	float gas;
	float reverse;
	float steering;
} input;

struct {
	float2 pos;
	float rotation;
} player;

float2 camera = {0};

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

float screen_world_height;
float screen_world_height_half;
float screen_world_width;
float screen_world_width_half;

void init() {
	screen_world_height = 10.0f;
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
}

void update() {
	glClearColor(0.0f, 0.1f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glPushMatrix();
	glTranslatef(-fmod(camera.x, 1.0f), -fmod(camera.y, 1.0f), 0);
	glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
	glBegin(GL_LINES);
	for (int i = 0; i < screen_world_height_half+1; ++i) {
		glVertex2f(-screen_world_width_half-1, i);
		glVertex2f(screen_world_width_half+1, i);
		if (i) {
			glVertex2f(-screen_world_width_half-1, -i);
			glVertex2f(screen_world_width_half+1, -i);
		}
	}
	for (int i = 0; i < screen_world_width_half+1; ++i) {
		glVertex2f(i, -screen_world_height_half-1);
		glVertex2f(i, screen_world_height_half+1);
		if (i) {
			glVertex2f(-i, -screen_world_height_half-1);
			glVertex2f(-i, screen_world_height_half+1);
		}
	}
	glEnd();
	glPopMatrix();
	
	player.pos = add2(player.pos, mul2f(make_float2(sinf(player.rotation), cosf(player.rotation)), input.gas*0.05f));
//	player.pos.y += input.gas * 0.01f;
//	player.pos.y -= input.reverse * 0.01f;
	player.rotation += input.steering*0.02f;
//	printf("rot %f\n", player.rotation/180.0f * PI);
	
	camera = player.pos;
	
//	camera.y += input.gas*0.1f;
//	camera.y -= input.reverse*0.1f;
//	camera.x += input.steering*0.1f;
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	gfx_draw_rect(player.pos.x - camera.x, player.pos.y - camera.y, 0.7f, 2.0f, player.rotation);
}