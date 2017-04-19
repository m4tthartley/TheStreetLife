
struct {
	float gas;
	float reverse;
	float steering;
} input;

struct {
	float x;
	float y;
	float rotation;
} player;

void gfx_draw_rect(float x, float y, float w, float h, float rotation) {
	float w12 = w/2.0f;
	float h12 = h/2.0f;
	glPushMatrix();
	glTranslatef(x, y, 0.0f);
	glRotatef(rotation, 0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);
		glVertex2f(-w12, -h12);
		glVertex2f( w12, -h12);
		glVertex2f( w12,  h12);
		glVertex2f(-w12,  h12);
	glEnd();
	glPopMatrix();
}

void init() {
	
}

void update() {
	glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	
	player.y += input.gas * 0.01f;
	player.y -= input.reverse * 0.01f;
	player.rotation -= input.steering;
	
	glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
	gfx_draw_rect(player.x, player.y, 0.07f, 0.2f, player.rotation);
}