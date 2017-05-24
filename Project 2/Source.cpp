#include <GL/freeglut.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <png.h>
#include < windows.h>
#include <mmsystem.h>

#define _CRT_SECURE_NO_WARNINGS
#define PI 3.14159265358979323846
#define RAD2DEG 180.0/PI
#define DEG2RAD PI/180.0

// Keyboard defines
#define KEY_ESCAPE 27
#define KEY_SPACEBAR 32

// Keyboard arrows
#define KEY_UP 101
#define KEY_DOWN 103
#define KEY_LEFT 100
#define KEY_RIGHT 102

// 2D function defines
#define Translate(x,y) glTranslated(x, y, 0.0)
#define Scale(x,y) glScalef(x, y, 1.0)
#define Rotate(angle) glRotatef(RAD2DEG*angle, 0.0, 0.0, 1.0)

// Game configrations
#define MAX_ACC_PLAYER  0.15
#define MAX_PLAYER_SPEED 4
#define MAX_ENEMY_SPEED 1.5
#define MAX_ENEMIES 8
#define MAX_BULLET_SHOOT 8
#define MAX_BULLET_SPEED 6

// Structures type definitions
typedef struct {
	int width;
	int height;
	char *title;
	float z_near;
	float z_far;
} glutWindow;

typedef struct {
	double  x, y, phi, dx, dy, vmax, vmax2;
	int hp;
} Player;

typedef struct {
	double  x, y, phi, dx, dy;
	bool alive;
} Enemy;

typedef struct {
	double  x, y, dx, dy, bullet_phi;
	bool active;
}  Bullet;

typedef struct {
	double  x, y;
} Direction;

// Default initialization when the game runs
static void initialize();

// Keyboard
void keyboard(unsigned char, int, int);
void keyPress(int, int, int);
void keyRelease(int, int, int);
void mouseHandler(int, int, int, int);

// Loop
void Timer(int);

// Player
void drawPlayer(Player *p);
void drawEnemy(Enemy *e);
void drawBullet(Bullet *b);
void drawBackground(double);
void drawScreen();
void drawPolygon(double, double, double, double);
void playerMovements();
void enemyMovements();
void moveBullet();
void checkBoundries();

// Collision
bool checkCollision(double, double, double, double, double, double);
bool checkCollision(Player p, Bullet b);
bool checkCollision(Enemy e, Bullet b);
bool checkCollision(Player p, Enemy e);
bool checkCollision(Enemy e1, Enemy e2);

// Display
void display();
void myReshape(int, int);
void setWindowValues();
void mainMenu();
void loadAllTextures();

// Static global variables
static glutWindow win;

GLuint menuBackground[1];
GLuint Title[1];
GLuint Play[1];
GLuint Resume[1];
GLuint newGame[1];
GLuint Options[1];
GLuint Exit[1];
GLuint Plane[2];
GLuint Hp[2];
GLuint Ammo[2];
GLuint Background[1];
GLuint Score[1];
GLuint Numbers[10];
GLuint gameOver[1];
GLint x = 0;
GLint y = 0;

// Keys at starting state
static int up = 0;
static int down = 0;
static int left = 0;
static int right = 0;
static int shoot = 0;
static int score1 = 0;
static int score2 = 0;
static Enemy enemies[MAX_ENEMIES];
static Bullet bullets[MAX_BULLET_SHOOT];
static Player player;
static Direction dir;
static double menuBackgroundMov;
static bool menu = true;
static bool pause = false;

int main(int argc, char **argv) {
	//sndPlaySound(TEXT("D:/back.WAV"), SND_ASYNC| SND_LOOP);
	setWindowValues();
	srand((unsigned int)time(NULL));
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(win.width, win.height);
	glutCreateWindow(win.title);
	glutDisplayFunc(display);
	glutIdleFunc(display);
	// Keyboard
	glutIgnoreKeyRepeat(1);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyPress);
	glutSpecialUpFunc(keyRelease); // register Keyboard Handler
	glutMouseFunc(mouseHandler);

	loadAllTextures();

	glutReshapeFunc(myReshape);
	glutTimerFunc(33, Timer, 0);
	initialize();

	glutMainLoop(); // run GLUT mainloop

	return 0;
}

static void initialize() {

	player.x = win.width / 2;
	player.y = win.height / 2;
	player.dx = player.dy = 0;
	player.hp = 100;
	player.vmax = MAX_PLAYER_SPEED;
	player.vmax2 = MAX_PLAYER_SPEED * MAX_PLAYER_SPEED;
}

void keyboard(unsigned char key, int mousePositionX, int mousePositionY) {

	switch (key) {

	case KEY_ESCAPE:
		if (!menu) {
			menu = true;
			pause = true;
		}
		break;

	case KEY_SPACEBAR:
		shoot = 1;
		//PlaySoundA((LPCSTR) "laser.WAV", NULL, SND_FILENAME | SND_ASYNC);
		break;

	default:
		break;
	}
}

// Special keys
void keyPress(int key, int x, int y) {

	switch (key) {
	case KEY_LEFT:
		left = 1;
		break;

	case KEY_UP:
		up = 1;
		break;

	case KEY_RIGHT:
		right = 1;
		break;

	case KEY_DOWN:
		down = 1;
		break;

	default:
		break;
	}
}

// Special key released
void keyRelease(int key, int x, int y) {

	switch (key) {
	case KEY_LEFT:
		left = 0;
		break;

	case KEY_UP:
		up = 0;
		break;

	case KEY_RIGHT:
		right = 0;
		break;

	case KEY_DOWN:
		down = 0;
		break;

	default:
		break;
	}
}

// Mouse events
void mouseHandler(int button, int state, int mouse_x, int mouse_y)
{
	if (menu)
	{
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		{
			x = mouse_x;
			y = win.height - mouse_y;
			//printf("\nx: %d, y: %d", x, y);
			if (pause)
			{
				if ((x >= 525 && x <= 825) && (y <= 495 && y >= 395))
				{
					// Resume button
					menu = false;
				}
				else if ((x >= 525 && x <= 825) && (y <= 375 && y >= 275))
				{
					// New Game button
					initialize();
					for (int i = 0; i < MAX_ENEMIES; i++)
					{
						enemies[i].alive = false;
					}
					for (int i = 0; i < MAX_BULLET_SHOOT; i++)
					{
						bullets[i].active = false;
					}
					menu = false;
					score1 = 0;
					score2 = 0;
				}
				else if ((x >= 525 && x <= 825) && (y <= 255 && y >= 155))
				{
					// Options button
					
				}
				else if ((x >= 525 && x <= 825) && (y <= 135 && y >= 35))
				{
					// Exit button
					exit(0);
				}
			}
			else
			{
				if ((x >= 525 && x <= 825) && (y <= 495 && y >= 395))
				{
					// Play button
					menu = false;
					//printf("Play Clicked");
				}
				else if ((x >= 525 && x <= 825) && (y <= 375 && y >= 275))
				{
					// Options button
					printf("Options Clicked");
				}
				else if ((x >= 525 && x <= 825) && (y <= 255 && y >= 155))
				{
					// Exit button
					exit(0);
				}
			}
		}
	}
	else if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		shoot = 1;
	}
}

// Loop
void Timer(int value) {

	playerMovements();
	moveBullet();
	enemyMovements();
	checkBoundries();

	for (int i = 0; i < MAX_BULLET_SHOOT; i++)
	{
		if (bullets[i].active) {
			for (int j = 0; j < MAX_ENEMIES; j++) {
				if (checkCollision(enemies[j], bullets[i])) {
					bullets[i].active = false;
					enemies[j].alive = false;
					if (score1 >= 9)
					{
						score1 = -1;
						score2++;
					}
					score1++;
				}
			}
		}
	}

	// Destroying enemies when collide with player
	for (int i = 0; i < MAX_ENEMIES; i++)
	{
		if (checkCollision(player, enemies[i]))
		{
			enemies[i].alive = false;
			player.hp -= 20;
			//printf("%d", player.hp);
		}
	}

	glutPostRedisplay();
	glutTimerFunc(33, Timer, value);
}

void playerMovements() {
	if (!player.hp <= 0)
	{
		if (left && right) {
			//If both left and right keys are pressed just do nothing
		}
		else if (left == 1) {
			player.phi = player.phi + DEG2RAD * 7;
			//printf("%d", DEG2RAD);
		}
		else if (right == 1) {
			player.phi = player.phi - DEG2RAD * 7;
		}

		if (up && down) {
			// If both left and right keys are pressed just stop the player
			player.dx = 0.0;
			player.dy = 0.0;
		}
		else if (up == 1) {
			player.dx = player.dx - MAX_ACC_PLAYER * sin(player.phi);
			player.dy = player.dy + MAX_ACC_PLAYER * cos(player.phi);
		}
		else if (down == 1) {
			player.dx = player.dx + (MAX_ACC_PLAYER - 0.09) * sin(player.phi);
			player.dy = player.dy - (MAX_ACC_PLAYER - 0.09) * cos(player.phi);
		}
		// When keys are released stop the player
		else if (up == 0 && down == 0) {

			if (player.dx > 0) {
				player.dx -= 0.2;
			}

			if (player.dy > 0) {
				player.dy -= 0.2;
			}

			if (player.dx < 0 || player.dy < 0) {
				player.dx = 0;
				player.dy = 0;
			}
		}

		double temp;
		// Limiting to max speed when (moving backwards)
		if (down == 1 && (temp = (player.dx * player.dx + player.dy * player.dy)) > (player.vmax)) {
			temp = player.vmax / sqrt(temp);
			player.dx *= temp - 0.5;
			player.dy *= temp - 0.5;
		}
		// Limiting to max speed when (moving forward)
		else if ((temp = (player.dx * player.dx + player.dy * player.dy)) > player.vmax2) {
			//printf("\nx:%f, y:%f", player.dx, player.dy);
			temp = player.vmax / sqrt(temp);
			player.dx *= temp;
			player.dy *= temp;
		}

		player.x = player.x + player.dx;
		player.y = player.y + player.dy;
	}
}

void enemyMovements() {
	int i = 0;
	if (!menu) {
		for (i = 0; i < MAX_ENEMIES; i++)
		{
			if (enemies[i].alive == 0)
			{
				enemies[i].alive = 1;

				switch (rand() % 4) {
				case 0:
					enemies[i].x = rand() % win.width;
					enemies[i].y = win.height + 20;
					break;
				case 1:
					enemies[i].x = win.width + 20;
					enemies[i].y = rand() % win.height;
					break;
				case 2:
					enemies[i].x = -20;
					enemies[i].y = rand() % win.height;
					break;
				case 3:
					enemies[i].x = rand() % win.width;
					enemies[i].y = -20;
					break;
				}
			}

			double hyp;
			dir.x = player.x - enemies[i].x;
			dir.y = player.y - enemies[i].y;
			enemies[i].phi = atan2(dir.y, dir.x);
			hyp = sqrt(dir.x*dir.x + dir.y*dir.y);
			dir.x /= hyp;
			dir.y /= hyp;

			if (!checkCollision(player, enemies[i]))
			{
				if (!player.hp <= 0)
				{
					if (!checkCollision(enemies[i], enemies[i + 1])) {
						enemies[i].x += dir.x * MAX_ENEMY_SPEED;
						enemies[i].y += dir.y * MAX_ENEMY_SPEED;
					}
				}
			}

		}
	}

	/*for (i = 0; i < MAX_ENEMIES; i++)
	{
		if (enemies[i].alive == 1) {
			enemies[i].x = enemies[i].x + enemies[i].dx;
			enemies[i].y = enemies[i].y + enemies[i].dy;
		}
	}*/
}

void moveBullet() {
	if (!menu && !player.hp <= 0) {
		int i = 0;

		if (shoot == 1) {

			for (i = 0; i < MAX_BULLET_SHOOT; i++) {
				//printf("%d\n", i);
				if (!bullets[i].active) {
					bullets[i].active = true;
					bullets[i].x = player.x;
					bullets[i].y = player.y;
					bullets[i].bullet_phi = player.phi;
					bullets[i].dx = -MAX_BULLET_SPEED * sin(player.phi);
					bullets[i].dy = MAX_BULLET_SPEED * cos(player.phi);
					break;
				}
			}

			// Reseting shoot to 0 so that it doesn't fire continuously
			shoot = 0;
		}

		// Moving bullets
		for (i = 0; i < MAX_BULLET_SHOOT; i++)
		{
			if (bullets[i].active) {
				bullets[i].x = bullets[i].x + bullets[i].dx;
				bullets[i].y = bullets[i].y + bullets[i].dy;
			}
			// Making bullets inactive when going outside screen
			if (bullets[i].active && (bullets[i].x > win.width || bullets[i].x < 0 || bullets[i].y > win.height || bullets[i].y < 0)) {
				bullets[i].active = false;
			}
		}
	}
}

// Player collision detection around the boders of screen
void checkBoundries() {

	if (player.x > win.width) {
		player.x = win.width;
	}
	if (player.x < 0) {
		player.x = 0;
	}
	if (player.y > win.height) {
		player.y = win.height;
	}
	if (player.y < 0) {
		player.y = 0;
	}
}

void display() {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0.0f, 0.0f, -3.0f);

	if (menu) {
		mainMenu();
	}
	else
	{
		drawScreen();
		for (int i = 0; i < MAX_ENEMIES; i++) {
			if (checkCollision(player, enemies[i])) {
				glColor3f(1.0f, 0.0f, 0.0f);
			}
		}
		if (player.hp <= 0)
		{
			glColor3f(1.0f, 0.0f, 0.0f);
		}
		drawPlayer(&player);

		// Drawing bullets
		for (int i = 0; i < MAX_BULLET_SHOOT; i++) {
			if (bullets[i].active) {
				drawBullet(&bullets[i]);
			}
		}

		// Drawing enemies
		for (int j = 0; j < MAX_ENEMIES; j++) {
			if (enemies[j].alive) {
				drawEnemy(&enemies[j]);
			}
		}

		// Drawing health
		if (player.hp <= 0)
		{
			glBindTexture(GL_TEXTURE_2D, gameOver[0]);
			drawPolygon((win.width / 2) - 150, (win.height / 2) - 50, 0, 0);
		}
		else if (player.hp <= 20) {
			glBindTexture(GL_TEXTURE_2D, Hp[1]);
			drawPolygon(1165, 17, -270, -70);
		}
		else if (player.hp <= 40) {
			glBindTexture(GL_TEXTURE_2D, Hp[1]);
			drawPolygon(1165, 17, -270, -70);
			drawPolygon(1200, 17, -270, -70);
		}
		else if (player.hp <= 60) {
			glBindTexture(GL_TEXTURE_2D, Hp[1]);
			drawPolygon(1165, 17, -270, -70);
			drawPolygon(1200, 17, -270, -70);
			drawPolygon(1235, 17, -270, -70);
		}
		else if (player.hp <= 80) {
			glBindTexture(GL_TEXTURE_2D, Hp[1]);
			drawPolygon(1165, 17, -270, -70);
			drawPolygon(1200, 17, -270, -70);
			drawPolygon(1235, 17, -270, -70);
			drawPolygon(1270, 17, -270, -70);
		}
		else if (player.hp <= 100) {
			glBindTexture(GL_TEXTURE_2D, Hp[1]);
			drawPolygon(1165, 17, -270, -70);
			drawPolygon(1200, 17, -270, -70);
			drawPolygon(1235, 17, -270, -70);
			drawPolygon(1270, 17, -270, -70);
			drawPolygon(1305, 17, -270, -70);
		}
	}

	glutSwapBuffers();
}

void myReshape(int w, int h) {

	glMatrixMode(GL_PROJECTION); // select projection matrix
	glViewport(0, 0, win.width, win.height); // set the viewport
	glMatrixMode(GL_PROJECTION);  // set matrix mode
	glLoadIdentity(); // reset projection matrix
	glOrtho(0.0, win.width, 0.0, win.height, win.z_near, win.z_far);
	glMatrixMode(GL_MODELVIEW); // specify which matrix is the current matrix
	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f); // specify the clear value for the depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // specify implementation-specific hints
	glClearColor(0.0, 0.0, 0.02, 1.0); // specify clear values for the color buffers
}

// GLUT window configration
void setWindowValues() {

	win.width = 1350;
	win.height = 690;
	win.title = "Space Wars - Danish Rehan";
	win.z_near = 1.0f;
	win.z_far = 100.0f;
}

// Collision detection
bool checkCollision(double x1, double y1, double x2, double y2, double s1, double s2)
{
	return (x1 - s1 < x2 + s2 &&
		x1 + s1 > x2 - s2 &&
		y1 - s1 < y2 + s2 &&
		y1 + s1 > y2 - s2);
}

bool checkCollision(Player p, Bullet b)
{
	return checkCollision(p.x, p.y, b.x, b.y, 5, 1);
}

bool checkCollision(Enemy e, Bullet b)
{
	return checkCollision(e.x, e.y, b.x, b.y, 7, 7);
}

bool checkCollision(Player p, Enemy e)
{
	return checkCollision(p.x, p.y, e.x, e.y, 15, 15);
}

bool checkCollision(Enemy e1, Enemy e2)
{
	return checkCollision(e1.x, e1.y, e2.x, e2.y, 10, 10);
}

void mainMenu() {


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, menuBackground[0]);
	drawBackground(menuBackgroundMov);
	menuBackgroundMov += 0.00004;

	if (pause)
	{
		// Title
		glBindTexture(GL_TEXTURE_2D, Title[0]);
		drawPolygon((win.width / 2) - 150, 515, 0, 60);
		// Resume
		glBindTexture(GL_TEXTURE_2D, Resume[0]);
		drawPolygon((win.width / 2) - 150, 395, 0, 0);
		// New Game
		glBindTexture(GL_TEXTURE_2D, newGame[0]);
		drawPolygon((win.width / 2) - 150, 275, 0, 0);
		// Options
		glBindTexture(GL_TEXTURE_2D, Options[0]);
		drawPolygon((win.width / 2) - 150, 155, 0, 0);
		//Exit
		glBindTexture(GL_TEXTURE_2D, Exit[0]);
		drawPolygon((win.width / 2) - 150, 35 , 0, 0);
	}
	else
	{
		// Title
		glBindTexture(GL_TEXTURE_2D, Title[0]);
		drawPolygon((win.width / 2) - 150, 515, 0, 60);
		// Play
		glBindTexture(GL_TEXTURE_2D, Play[0]);
		drawPolygon((win.width / 2) - 150, 395, 0, 0); // 525
		// Options
		glBindTexture(GL_TEXTURE_2D, Options[0]);
		drawPolygon((win.width / 2) - 150, 275, 0, 0);
		//Exit
		glBindTexture(GL_TEXTURE_2D, Exit[0]);
		drawPolygon((win.width / 2) - 150, 155, 0, 0);
	}
}

// Drawing game screen
void drawScreen() {
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, Background[0]);
	drawBackground(0);

	glBindTexture(GL_TEXTURE_2D, Hp[0]);
	drawPolygon(1100, 17, -240, -70);

	glBindTexture(GL_TEXTURE_2D, Title[0]);
	drawPolygon((win.width / 2) - 115, 575, -70, -10);

	glBindTexture(GL_TEXTURE_2D, Score[0]);
	drawPolygon(5, 17, -130, -70);

	glBindTexture(GL_TEXTURE_2D, Numbers[score2]);
	drawPolygon(175, 17, -270, -70);

	glBindTexture(GL_TEXTURE_2D, Numbers[score1]);
	drawPolygon(205, 17, -270, -70);
	int c = 0;
	for (int j = 0; j < MAX_BULLET_SHOOT; j++) {
		if (!bullets[j].active) {
			c++;
		}
	}

	glBindTexture(GL_TEXTURE_2D, Ammo[0]);
	switch (c) {
	case 1:
		drawPolygon(615, 17, -290, -70);
		break;
	case 2:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		break;
	case 3:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		break;
	case 4:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		drawPolygon(660, 17, -290, -70);
		break;
	case 5:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		drawPolygon(660, 17, -290, -70);
		drawPolygon(675, 17, -290, -70);
		break;
	case 6:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		drawPolygon(660, 17, -290, -70);
		drawPolygon(675, 17, -290, -70);
		drawPolygon(690, 17, -290, -70);
		break;
	case 7:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		drawPolygon(660, 17, -290, -70);
		drawPolygon(675, 17, -290, -70);
		drawPolygon(690, 17, -290, -70);
		drawPolygon(705, 17, -290, -70);
		break;
	case 8:
		drawPolygon(615, 17, -290, -70);
		drawPolygon(630, 17, -290, -70);
		drawPolygon(645, 17, -290, -70);
		drawPolygon(660, 17, -290, -70);
		drawPolygon(675, 17, -290, -70);
		drawPolygon(690, 17, -290, -70);
		drawPolygon(705, 17, -290, -70);
		drawPolygon(720, 17, -290, -70);
		break;
	}
}

// Drawing player
void drawPlayer(Player *p) {
	
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glLineWidth(1.5);
	glEnable(GL_LINE_SMOOTH);
	glBindTexture(GL_TEXTURE_2D, Plane[0]);
	glPushMatrix();
	Translate(p->x, p->y);
	Rotate(p->phi);
	Scale(5, 5);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5, 1);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-4.5f, -4.5f, 0.0f);
	glTexCoord2f(1, 0);
	glVertex3f(4.5f, -4.5f, 0.0f);
	glEnd();
	glPopMatrix();
}

// Drawing enemy
void drawEnemy(Enemy *e) {

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBindTexture(GL_TEXTURE_2D, Plane[1]);
	glLineWidth(1.5);
	glEnable(GL_LINE_SMOOTH);
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	Translate(e->x, e->y);
	Rotate(e->phi - 90);
	Scale(7, 7);
	glBegin(GL_TRIANGLES);
	glTexCoord2f(0.5, 1);
	glVertex3f(0.0f, 4.0f, 0.0f);
	glTexCoord2f(0, 0);
	glVertex3f(-4.0f, -4.0f, 0.0f);
	glTexCoord2f(1, 0);
	glVertex3f(4.0f, -4.0f, 0.0f); 
	glEnd();
	glPopMatrix();
}

// Drawing bullet
void drawBullet(Bullet *b) {

	glDisable(GL_TEXTURE_2D);
	glLineWidth(0.5);
	glColor3f(1.0f, 1.0f, 0.0f);
	glPushMatrix();
	Translate(b->x, b->y);
	Rotate(b->bullet_phi);
	Scale(2, 2);
	glBegin(GL_TRIANGLES);
	glVertex3f(0.0f, 2.0f, 0.0f);
	glVertex3f(-1.0f, -1.0f, 0.0f);
	glVertex3f(1.0f, -1.0f, 0.0f);
	glEnd();
	glPopMatrix();
}

// Drawing background
void drawBackground(double menuBackgroundMov)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	glBegin(GL_POLYGON);
	glTexCoord2f(1 - menuBackgroundMov, 1);
	glVertex3f(win.width, win.height, 0.0f);
	glTexCoord2f(0 - menuBackgroundMov, 1);
	glVertex3f(0.0f, win.height, 0.0f);
	glTexCoord2f(0 - menuBackgroundMov, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1 - menuBackgroundMov, 0);
	glVertex3f(win.width, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();
}

// Drawing polygons
void drawPolygon(double t1, double t2, double w, double h)
{
	glColor3f(1.0f, 1.0f, 1.0f);
	glPushMatrix();
	Translate(t1, t2);
	glBegin(GL_POLYGON);
	glTexCoord2f(1, 1);
	glVertex3f(300.0f + w, 100.0f + h, 0.0f);
	glTexCoord2f(0, 1);
	glVertex3f(0.0f, 100.0f + h, 0.0f);
	glTexCoord2f(0, 0);
	glVertex3f(0.0f, 0.0f, 0.0f);
	glTexCoord2f(1, 0);
	glVertex3f(300.0f + w, 0.0f, 0.0f);
	glEnd();
	glPopMatrix();
}

GLuint png_texture_load(const char *file_name, int *width, int *height)
{
	png_byte header[8];

	FILE *fp = fopen(file_name, "rb");
	if (fp == 0)
	{
		perror(file_name);
		return 0;
	}

	// read the header
	fread(header, 1, 8, fp);

	if (png_sig_cmp(header, 0, 8))
	{
		fprintf(stderr, "error: %s is not a PNG.\n", file_name);
		fclose(fp);
		return 0;
	}

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
	if (!png_ptr)
	{
		fprintf(stderr, "error: png_create_read_struct returned 0.\n");
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, (png_infopp)NULL, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// create png info struct
	png_infop end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		fprintf(stderr, "error: png_create_info_struct returned 0.\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp)NULL);
		fclose(fp);
		return 0;
	}

	// the code in this if statement gets called if libpng encounters an error
	if (setjmp(png_jmpbuf(png_ptr))) {
		fprintf(stderr, "error from libpng\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// init png reading
	png_init_io(png_ptr, fp);

	// let libpng know you already read the first 8 bytes
	png_set_sig_bytes(png_ptr, 8);

	// read all the info up to the image data
	png_read_info(png_ptr, info_ptr);

	// variables to pass to get info
	int bit_depth, color_type;
	png_uint_32 temp_width, temp_height;

	// get info about png
	png_get_IHDR(png_ptr, info_ptr, &temp_width, &temp_height, &bit_depth, &color_type,
		NULL, NULL, NULL);

	//if (width) { *width = temp_width; }
	//if (height) { *height = temp_height; }

	//printf("%s: %lux%lu %d\n", file_name, temp_width, temp_height, color_type);

	if (bit_depth != 8)
	{
		fprintf(stderr, "%s: Unsupported bit depth %d.  Must be 8.\n", file_name, bit_depth);
		return 0;
	}

	GLint format;
	switch (color_type)
	{
	case PNG_COLOR_TYPE_RGB:
		format = GL_RGB;
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		format = GL_RGBA;
		break;
	default:
		fprintf(stderr, "%s: Unknown libpng color type %d.\n", file_name, color_type);
		return 0;
	}

	// Update the png info struct.
	png_read_update_info(png_ptr, info_ptr);

	// Row size in bytes.
	int rowbytes = png_get_rowbytes(png_ptr, info_ptr);

	// glTexImage2d requires rows to be 4-byte aligned
	rowbytes += 3 - ((rowbytes - 1) % 4);

	// Allocate the image_data as a big block, to be given to opengl
	png_byte * image_data = (png_byte *)malloc(rowbytes * temp_height * sizeof(png_byte) + 15);
	if (image_data == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG image data\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		fclose(fp);
		return 0;
	}

	// row_pointers is for pointing to image_data for reading the png with libpng
	png_byte ** row_pointers = (png_byte **)malloc(temp_height * sizeof(png_byte *));
	if (row_pointers == NULL)
	{
		fprintf(stderr, "error: could not allocate memory for PNG row pointers\n");
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		free(image_data);
		fclose(fp);
		return 0;
	}

	// set the individual row_pointers to point at the correct offsets of image_data
	for (unsigned int i = 0; i < temp_height; i++)
	{
		row_pointers[temp_height - 1 - i] = image_data + i * rowbytes;
	}

	// read the png into image_data through row_pointers
	png_read_image(png_ptr, row_pointers);

	// Generate the OpenGL texture object
	GLuint texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D, 0, format, temp_width, temp_height, 0, format, GL_UNSIGNED_BYTE, image_data);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// clean up
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
	free(image_data);
	free(row_pointers);
	fclose(fp);
	return texture;
}

void loadAllTextures()
{
	int a = 977;
	int b = 817;

	Title[0] = png_texture_load("D:\\Sprites\\Title.png", &a, &b);

	menuBackground[0] = png_texture_load("D:\\Sprites\\menuBackground.png", &a, &b);

	Play[0] = png_texture_load("D:\\Sprites\\Play.png", &a, &b);

	Resume[0] = png_texture_load("D:\\Sprites\\Resume.png", &a, &b);

	newGame[0] = png_texture_load("D:\\Sprites\\newGame.png", &a, &b);

	Options[0] = png_texture_load("D:\\Sprites\\Options.png", &a, &b);

	Exit[0] = png_texture_load("D:\\Sprites\\Exit.png", &a, &b);

	Plane[0] = png_texture_load("D:\\Sprites\\Plane.png", &a, &b);
	Plane[1] = png_texture_load("D:\\Sprites\\Enemy.png", &a, &b);

	Hp[0] = png_texture_load("D:\\Sprites\\Hp.png", &a, &b);
	Hp[1] = png_texture_load("D:\\Sprites\\Health.png", &a, &b);

	Ammo[0] = png_texture_load("D:\\Sprites\\Ammo.png", &a, &b);
	Ammo[1] = png_texture_load("D:\\Sprites\\noAmmo.png", &a, &b);

	Background[0] = png_texture_load("D:\\Sprites\\Background.png", &a, &b);

	Score[0] = png_texture_load("D:\\Sprites\\Score.png", &a, &b);

	Numbers[0] = png_texture_load("D:\\Sprites\\0.png", &a, &b);
	Numbers[1] = png_texture_load("D:\\Sprites\\1.png", &a, &b);
	Numbers[2] = png_texture_load("D:\\Sprites\\2.png", &a, &b);
	Numbers[3] = png_texture_load("D:\\Sprites\\3.png", &a, &b);
	Numbers[4] = png_texture_load("D:\\Sprites\\4.png", &a, &b);
	Numbers[5] = png_texture_load("D:\\Sprites\\5.png", &a, &b);
	Numbers[6] = png_texture_load("D:\\Sprites\\6.png", &a, &b);
	Numbers[7] = png_texture_load("D:\\Sprites\\7.png", &a, &b);
	Numbers[8] = png_texture_load("D:\\Sprites\\8.png", &a, &b);
	Numbers[9] = png_texture_load("D:\\Sprites\\9.png", &a, &b);

	gameOver[0] = png_texture_load("D:\\Sprites\\gameOver.png", &a, &b);
}