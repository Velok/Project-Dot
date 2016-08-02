#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>


const float FPS = 20.0;
const float TIMER_RATE = 1.0 / FPS;
const int SCREEN_W = 1240;
const int SCREEN_H = 700;
const int MAP_HEIGHT = 43;
const int MAP_WIDTH = 77;
const int VERTICAL_BORDER = 14;
const int HORIZONTAL_BORDER = 12;
const float DEGREE0 = 0;
const float DEGREE90 = ALLEGRO_PI / 2.0;
const float DEGREE180 = ALLEGRO_PI;
const float DEGREE270 = - (ALLEGRO_PI / 2.0);

struct player_props {

	float coord_x;
	float coord_y;
	int collision_border[4]; // the area where the player is drawn (deltaX1, deltaY1 / deltaX2, deltaY2)
	float last_coord_x;
	float last_coord_y;

	ALLEGRO_COLOR color;
	float rotation;

	bool control_left;
	bool control_right;
	bool control_up;
	bool control_down;
	bool automatic_moving;
	bool sfx;
	bool basic_text;

	int extension_count;
};

struct tile_props {

	float coord_x;
	float coord_y;

	ALLEGRO_COLOR color;
	float rotation;
};

// prototypes
void initialisation();
void definePlayerProps();
void defineTileProps();
void createResources();
void destroyResources();
void destroyElements();
void drawMap();
void drawText(float duration, char *text);
void sendMessage(float duration, char *text);
int detectPoint(int point);
void clearPoint();
void PointCollector();
void collisionDetection();
void setPlayerStart();
void drawPlayer();
void drawTile(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color, float coord_x, float coord_y, float angle, int flags);
void storeLastPosition(struct player_props *element);
void resetToLastPosition(struct player_props *element);

//global variables
ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *refresh_timer = NULL;
ALLEGRO_KEYBOARD_STATE keystate;

ALLEGRO_BITMAP *dot = NULL;
ALLEGRO_BITMAP *point = NULL;
ALLEGRO_BITMAP *wall_simple = NULL;
ALLEGRO_BITMAP *wall_corner = NULL;
ALLEGRO_BITMAP *wall_tpiece = NULL;
ALLEGRO_BITMAP *wall_diagonal = NULL;

ALLEGRO_SAMPLE *exploration = NULL;
ALLEGRO_SAMPLE *pickup = NULL;
ALLEGRO_SAMPLE *collision = NULL;
ALLEGRO_SAMPLE *levelup = NULL;

ALLEGRO_FONT *message_font = NULL;

bool keys[4];

bool doexit = false;
bool redraw = true;

int cycle_count = 0;
float textduration = 0;
char *textstring;

float grid_size = 32;

int map[MAP_HEIGHT][MAP_WIDTH] = { // map[y][x]

		{0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   },
		{0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   },
		{0x08,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0f,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0f,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x09},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x02,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x08,0x07,0x15,0   ,0x08,0x07,0x09,0   ,0x07,0x0f,0x07,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x05,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0x05,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0   ,0x06,0   ,0x08,0x07,0x09,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x15,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0x06,0   ,0   ,0x06,0   ,0x06,0x11,0x06,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x0a,0x07,0x17,0   ,0x0a,0x07,0x0b,0   ,0   ,0x06,0   ,0x0a,0x07,0x0b,0   ,0x01,0   ,0   ,0x04,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0x13,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0c},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x17,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x16,0x15,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x0d,0x07,0x07,0x07,0x07,0x07,0x07,0x09,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x07,0x07,0x07,0x07,0x0c,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0x15,0x14,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x14,0x17,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x0a,0x07,0x07,0x0e,0x0e,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0x12,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x05,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x0a,0x07,0x07,0x07,0x07,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x03,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x0d,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0b,0   ,0   ,0   ,0   ,0   ,0   ,0x0d,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0e,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x09,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x08,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0f,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x09,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0x05,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x05,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0x10,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x0d,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0b,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0x05,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0x06,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0   ,0x06},
		{0x0a,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0e,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0e,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0e,0x07,0x07,0x07,0x07,0x07,0x07,0x0e,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x0b},
	};

struct player_props dot_props;

struct tile_props	default_values,
					message_font_props,
					point_props,
					wall_vertical_props,
					wall_horizontal_props,
					wall_ulCorner_props,
					wall_urCorner_props,
					wall_llCorner_props,
					wall_lrCorner_props,
					wall_ulDiagonal_props,
					wall_urDiagonal_props,
					wall_llDiagonal_props,
					wall_lrDiagonal_props,
					wall_leftTpiece_props,
					wall_rightTpiece_props,
					wall_upTpiece_props,
					wall_downTpiece_props;

// main
int main(void) {

	dot_props.control_right = true;
	dot_props.control_left = false;
	dot_props.control_up = false;
	dot_props.control_down = false;

	dot_props.extension_count = 1;
	dot_props.automatic_moving = false;
	dot_props.sfx = false;
	dot_props.basic_text = false;

	initialisation();
	definePlayerProps();
	defineTileProps();
	createResources();

	al_set_target_backbuffer(display);

	setPlayerStart();

	drawMap();
	drawPlayer();

	al_flip_display();

	al_start_timer(refresh_timer);

	while (!doexit) { //main Loop
	
		ALLEGRO_EVENT ev;
		al_wait_for_event(event_queue, &ev);

		if (ev.type == ALLEGRO_EVENT_TIMER) {

			if (cycle_count <= 30000) {
			
				++cycle_count;
			}
		}

		if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
		
			doexit = true;
		}

		if (!dot_props.automatic_moving) {
			
			if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {

				storeLastPosition(&dot_props);

				switch (ev.keyboard.keycode) {
				
					case ALLEGRO_KEY_DOWN:

						if (dot_props.control_down) {
						
							++dot_props.coord_y;
						}
						break;

					case ALLEGRO_KEY_UP:

						if (dot_props.control_up) {
						
							--dot_props.coord_y;
						}
						break;

					case ALLEGRO_KEY_LEFT:

						if (dot_props.control_left) {
						
							--dot_props.coord_x;
						}
						break;

					case ALLEGRO_KEY_RIGHT:

						if (dot_props.control_right) {
						
							++dot_props.coord_x;
						}
						break;
				}
			}

			redraw = true;		
		}

		if (dot_props.automatic_moving) {
		
			if (ev.type == ALLEGRO_EVENT_TIMER) {

				storeLastPosition(&dot_props);

				al_get_keyboard_state(&keystate);

				if(al_key_down(&keystate, ALLEGRO_KEY_DOWN)) {

					if (dot_props.control_down) {
					
						++dot_props.coord_y;
					}
				}

				if(al_key_down(&keystate, ALLEGRO_KEY_UP)) {

					if (dot_props.control_up) {
					
						--dot_props.coord_y;
					}
				}

				if(al_key_down(&keystate, ALLEGRO_KEY_LEFT)) {

					if (dot_props.control_left) {
					
						--dot_props.coord_x;
					}
				}

				if(al_key_down(&keystate, ALLEGRO_KEY_RIGHT)) {

					if (dot_props.control_right) {
					
						++dot_props.coord_x;
					}
				}

				redraw = true;
			}
		}
		
		if (redraw == true) {

			collisionDetection();
			PointCollector();

			al_clear_to_color(al_map_rgb(0, 0, 0));
			drawMap();
			drawText(textduration, textstring);
			drawPlayer();
			al_flip_display();
			redraw = false;
		}
	}

	destroyElements();
	return 0;
}

void initialisation() {

	al_init();
	al_init_image_addon();
	al_install_keyboard();
	al_install_audio();
	al_init_acodec_addon();
	al_init_font_addon();
	al_init_ttf_addon();
	al_reserve_samples(1);

	al_set_new_display_option(ALLEGRO_VSYNC, 1, ALLEGRO_DONTCARE);

	display = al_create_display(SCREEN_W, SCREEN_H);
	refresh_timer = al_create_timer(TIMER_RATE);
	event_queue = al_create_event_queue();

	al_register_event_source(event_queue, al_get_keyboard_event_source());
	al_register_event_source(event_queue, al_get_display_event_source(display));
	al_register_event_source(event_queue, al_get_timer_event_source(refresh_timer));

	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
	al_clear_to_color(al_map_rgb(0, 0, 0));
	al_flip_display();
}

void definePlayerProps() {

	dot_props.color = al_map_rgba(255, 255, 255, 255);
	dot_props.collision_border[0] = 0;
	dot_props.collision_border[1] = 0;
	dot_props.collision_border[2] = 0;
	dot_props.collision_border[3] = 0;
}

void defineTileProps() {

	default_values.color = al_map_rgba(60, 60, 60, 255);
	default_values.rotation = DEGREE0;

	message_font_props.color = al_map_rgba(0, 255, 0, 255);

	point_props = default_values;
	point_props.color = al_map_rgba(100, 100, 100, 255);

	wall_vertical_props = default_values;
	wall_vertical_props.rotation = DEGREE90;

	wall_horizontal_props = default_values;

	wall_ulCorner_props = default_values;

	wall_urCorner_props = default_values;
	wall_urCorner_props.rotation = DEGREE90;

	wall_llCorner_props = default_values;
	wall_llCorner_props.rotation = DEGREE270;

	wall_lrCorner_props = default_values;
	wall_lrCorner_props.rotation = DEGREE180;

	wall_ulDiagonal_props = default_values;

	wall_urDiagonal_props = default_values;
	wall_urDiagonal_props.rotation = DEGREE90;

	wall_llDiagonal_props = default_values;
	wall_llDiagonal_props.rotation = DEGREE270;

	wall_lrDiagonal_props = default_values;
	wall_lrDiagonal_props.rotation = DEGREE180;

	wall_leftTpiece_props = default_values;
	wall_leftTpiece_props.rotation = DEGREE90;

	wall_rightTpiece_props = default_values;
	wall_rightTpiece_props.rotation = DEGREE270;

	wall_upTpiece_props = default_values;
	wall_upTpiece_props.rotation = DEGREE180;

	wall_downTpiece_props = default_values;
}

void createResources() {

	dot = al_load_bitmap("data/images/dot.png");
	point = al_load_bitmap("data/images/point.png");
	wall_simple = al_load_bitmap("data/images/wall_simple.png");
	wall_corner = al_load_bitmap("data/images/wall_corner.png");
	wall_tpiece = al_load_bitmap("data/images/wall_tpiece.png");
	wall_diagonal = al_load_bitmap("data/images/wall_diagonal.png");

	exploration = al_load_sample("data/sounds/exploration.ogg");
	collision = al_load_sample("data/sounds/collision.ogg");
	levelup = al_load_sample("data/sounds/levelup.ogg");
	pickup = al_load_sample("data/sounds/pickup.ogg");

	message_font = al_load_ttf_font("data/fonts/message_font.ttf", 32, 0);
}

void destroyResources() {

	al_destroy_bitmap(dot);
	al_destroy_bitmap(point);
	al_destroy_bitmap(wall_simple);
	al_destroy_bitmap(wall_corner);
	al_destroy_bitmap(wall_tpiece);
	al_destroy_bitmap(wall_diagonal);

	al_destroy_sample(exploration);
	al_destroy_sample(collision);
	al_destroy_sample(levelup);
	al_destroy_sample(pickup);
}

void destroyElements() {

	al_destroy_timer(refresh_timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	destroyResources();	
}

void drawMap () {

	for (int y = 0; y < MAP_HEIGHT; ++y) {
	
		for (int x = 0; x < MAP_WIDTH; ++x) {
		
			switch (map[y][x]) {

				case 0x02:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x03:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x04:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x05:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x10:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;				

				case 0x11:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x12:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;	

				case 0x13:
					drawTile(point, point_props.color, x, y, point_props.rotation, 0);
					break;

				case 0x06:
					drawTile(wall_simple, wall_vertical_props.color, x, y, wall_vertical_props.rotation, 0);
					break;

				case 0x07:
					drawTile(wall_simple, wall_horizontal_props.color, x, y, wall_horizontal_props.rotation, 0);
					break;

				case 0x08:
					drawTile(wall_corner, wall_ulCorner_props.color, x, y, wall_ulCorner_props.rotation, 0);
					break;

				case 0x09:
					drawTile(wall_corner, wall_urCorner_props.color, x, y, wall_urCorner_props.rotation, 0);
					break;

				case 0x0a:
					drawTile(wall_corner, wall_llCorner_props.color, x, y, wall_llCorner_props.rotation, 0);
					break;

				case 0x0b:
					drawTile(wall_corner, wall_lrCorner_props.color, x, y, wall_lrCorner_props.rotation, 0);
					break;

				case 0x0c:
					drawTile(wall_tpiece, wall_leftTpiece_props.color, x, y, wall_leftTpiece_props.rotation, 0);
					break;

				case 0x0d:
					drawTile(wall_tpiece, wall_rightTpiece_props.color, x, y, wall_rightTpiece_props.rotation, 0);
					break;

				case 0x0e:
					drawTile(wall_tpiece, wall_upTpiece_props.color, x, y, wall_upTpiece_props.rotation, 0);
					break;

				case 0x0f:
					drawTile(wall_tpiece, wall_downTpiece_props.color, x, y, wall_downTpiece_props.rotation, 0);
					break;

				case 0x14:
					drawTile(wall_diagonal, wall_ulDiagonal_props.color, x, y, wall_ulDiagonal_props.rotation, 0);
					break;

				case 0x15:
					drawTile(wall_diagonal, wall_urDiagonal_props.color, x, y, wall_urDiagonal_props.rotation, 0);
					break;

				case 0x16:
					drawTile(wall_diagonal, wall_llDiagonal_props.color, x, y, wall_llDiagonal_props.rotation, 0);
					break;

				case 0x17:
					drawTile(wall_diagonal, wall_lrDiagonal_props.color, x, y, wall_lrDiagonal_props.rotation, 0);
					break;
			}
		}
	}
}

void drawText(float duration, char *text) {

	float cycles = duration * FPS;

	if (dot_props.basic_text) {
	
		if (cycle_count <= (int)cycles) {
		
			al_draw_text(message_font, message_font_props.color, 10, 10, ALLEGRO_ALIGN_LEFT, text);
		}
	}
}

void sendMessage(float duration, char *text) {

	cycle_count = 0;
	textduration = duration;
	textstring = text;
}

int detectPoint(int point) {

	for (int y = dot_props.collision_border[0]; y <= dot_props.collision_border[2]; ++y) {
	
		for (int x = dot_props.collision_border[3]; x <= dot_props.collision_border[1]; ++x) {
		
			if (map[(int)dot_props.coord_y + y][(int)dot_props.coord_x + x] == point) {
			
				return 1;
			}
		}
	}

	return 0;
}

void clearPoint() {

	for (int y = dot_props.collision_border[0]; y <= dot_props.collision_border[2]; ++y) {
	
		for (int x = dot_props.collision_border[3]; x <= dot_props.collision_border[1]; ++x) {
		
			map[(int)dot_props.coord_y + y][(int)dot_props.coord_x + x] = 0   ;
		}
	}
}

void PointCollector() {

	if (detectPoint(0x02)) {
	
		if (dot_props.sfx) {
		
			al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}
		
		sendMessage(4, "Du kannst dich nun nach links bewegen");

		map[10][23] = 0x00;
		map[10][21] = 0x00;
		map[9][22] = 0x00;
		map[11][22] = 0x00;
		dot_props.control_left = true;
		clearPoint();
	}

	if (detectPoint(0x03)) {
		
		if (dot_props.sfx) {
		
			al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		sendMessage(4, "Du kannst dich nun nach oben bewegen");

		dot_props.control_up = true;
		clearPoint();
	}

	if (detectPoint(0x04)) {
		
		if (dot_props.sfx) {
		
			al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		sendMessage(4, "Du kannst dich nun nach unten bewegen");

		dot_props.control_down = true;
		clearPoint();
	}

	if (detectPoint(0x05)) {
		
		++dot_props.extension_count;

		if (dot_props.sfx) {
			
			if (dot_props.extension_count == 7) {
			
				al_play_sample(levelup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
			else {

				al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
			}
		}

		switch (dot_props.extension_count) {
		
			case 2:
				sendMessage(4, "Suche weiter um dich zu finden!");
				break;

			case 6:
				sendMessage(4, "Tor drei bitte auf..");
				map[21][47] = 0x0b;
				map[21][48] = 0x00;
				map[21][49] = 0x00;
				map[21][50] = 0x00;
				map[21][51] = 0x0a;
				map[20][47] = 0x06;
				map[20][51] = 0x06;
				break;

			case 7:
				sendMessage(4, "Glueckwunsch! Du hast komplett zu dir gefunden.");
				break;				
		}

		clearPoint();
	}

	if (detectPoint(0x10)) {
		
		if (dot_props.sfx) {
		
			al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		sendMessage(4, "Lauf Forre... ach lassen wir das.");

		map[29][8] = 0x0a;
		map[30][8] = 0x00;
		map[31][8] = 0x00;
		map[32][8] = 0x08;
		map[29][9] = 0x07;
		map[32][9] = 0x07;

		dot_props.automatic_moving = true;
		clearPoint();
	}

	if (detectPoint(0x11)) {
		
		if (dot_props.sfx) {
		
			al_play_sample(exploration, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		map[17][38] = 0x00;
		map[18][38] = 0x08;
		map[18][39] = 0x07;
		sendMessage(4, "Ja hei isch di Waelt ae groessi!");

		grid_size = 16;	
		clearPoint();
	}

	if (detectPoint(0x12)) {
		
		if (dot_props.sfx) {
		
			al_play_sample(exploration, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		}

		sendMessage(4, "Wer lesen kann ist klar im Vorteil.");

		dot_props.basic_text = true;
		clearPoint();
	}

	if (detectPoint(0x13)) {
		
		sendMessage(4, "SFX FTW!");

		dot_props.sfx = true;
		al_play_sample(pickup, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
		clearPoint();
	}
}

void collisionDetection() {

	int colliders[] = {0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0x14, 0x15, 0x16, 0x17};

	for (int y = dot_props.collision_border[0]; y <= dot_props.collision_border[2]; ++y) {
	
		for (int x = dot_props.collision_border[3]; x <= dot_props.collision_border[1]; ++x) {
		
			for (int i = 0; i < sizeof(colliders) / sizeof(int); ++i) {
			
				if (map[(int)dot_props.coord_y + y][(int)dot_props.coord_x + x] == colliders[i]) {
					
					if (dot_props.sfx) {
					
						al_play_sample(collision, 1, 0, 1, ALLEGRO_PLAYMODE_ONCE, NULL);
					}

					resetToLastPosition(&dot_props);
					break;
				}
			}
		}
	}
}

void setPlayerStart() {

	for (int y = 0; y < MAP_HEIGHT; ++y) {
	
		for (int x = 0; x < MAP_WIDTH; ++x) {
		
			switch (map[y][x]) {

				case 0x01:
					dot_props.coord_x = x;
					dot_props.coord_y = y;
					break;
			}
		}
	}
}

void drawPlayer() {

	for (int i = 1; i <= dot_props.extension_count; ++i) {
	
		switch (i) {
		
			case 1:
				drawTile(dot, dot_props.color, dot_props.coord_x, dot_props.coord_y, dot_props.rotation, 0);
				break;

			case 2:
				dot_props.collision_border[2] = 1;
				drawTile(dot, dot_props.color, dot_props.coord_x, dot_props.coord_y + 1, dot_props.rotation, 0);
				break;

			case 3:
				dot_props.collision_border[2] = 2;
				dot_props.collision_border[1] = 1;
				drawTile(dot, dot_props.color, dot_props.coord_x + 1, dot_props.coord_y + 2, dot_props.rotation, 0);
				break;

			case 4:
				dot_props.collision_border[3] = -1;
				drawTile(dot, dot_props.color, dot_props.coord_x - 1, dot_props.coord_y, dot_props.rotation, 0);
				break;

			case 5:
				dot_props.collision_border[0] = -1;
				drawTile(dot, dot_props.color, dot_props.coord_x, dot_props.coord_y - 1, dot_props.rotation, 0);
				break;

			case 6:
				dot_props.collision_border[2] = 2;
				dot_props.collision_border[3] = -1;
				drawTile(dot, dot_props.color, dot_props.coord_x - 1, dot_props.coord_y + 2, dot_props.rotation, 0);
				break;

			case 7:
				dot_props.collision_border[1] = 1;
				drawTile(dot, dot_props.color, dot_props.coord_x + 1, dot_props.coord_y, dot_props.rotation, 0);
				break;
		}
	}
}

void drawTile(ALLEGRO_BITMAP *bitmap, ALLEGRO_COLOR color, float coord_x, float coord_y, float rotation, int flags) {

	al_draw_tinted_scaled_rotated_bitmap(	bitmap,
											color,
											16,
											16,
											HORIZONTAL_BORDER + coord_x * grid_size,
											VERTICAL_BORDER + coord_y * grid_size,
											(1.0 / 32.0) * grid_size,
											(1.0 / 32.0) * grid_size,
											rotation,
											flags);
}

void storeLastPosition(struct player_props *element) {

	element->last_coord_x = element->coord_x;
	element->last_coord_y = element->coord_y;
}

void resetToLastPosition(struct player_props *element) {

	element->coord_x = element->last_coord_x;
	element->coord_y = element->last_coord_y;
}
