/*

The MIT License (MIT)

Copyright (c) 2016 Gameblabla

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

*/

#define FPS_VIDEO 60
#define MAX_IMAGE 512
#define MAX_PATH 128

#include <libndls.h>
#include "n2DLib.h"

#include "INPUT.h"
struct input BUTTON;

unsigned short done = 0;
const float real_FPS = 1000/FPS_VIDEO;
char* game_name = "";

unsigned short *sprites_img[MAX_IMAGE];

#define Buttons_UP KEY_NSPIRE_UP
#define Buttons_LEFT KEY_NSPIRE_LEFT
#define Buttons_RIGHT KEY_NSPIRE_RIGHT
#define Buttons_DOWN KEY_NSPIRE_DOWN
#define Buttons_A KEY_NSPIRE_DOC
#define Buttons_B KEY_NSPIRE_DEL
#define Buttons_C KEY_NSPIRE_VAR
#define Buttons_D KEY_NSPIRE_CTRL
#define Buttons_START KEY_NSPIRE_ENTER
#define Buttons_SELECT KEY_NSPIRE_MENU
#define Buttons_QUIT KEY_NSPIRE_ESC

void Init_video(char* argv[])
{
	enable_relative_paths(argv);
	initBuffering();
	clearBufferB();
	updateScreen();
}

void Load_Image(unsigned short a, const char* directory)
{
	char final[MAX_PATH];
	
	snprintf(final, sizeof(final), "./%s.tns", directory);
	
	#ifdef DEBUG
		fprintf(stderr, "Clearing surface for image %d...\n", a);
	#endif
	
	if (sprites_img[a])
	{
		sprites_img[a] = NULL;
	}
	
	#ifdef DEBUG
		fprintf(stderr, "Trying to load image %d (%s)...\n", a, directory);
	#endif
	
	sprites_img[a] = loadBMP(final, 0xf81f);
	
	#ifdef DEBUG
		fprintf(stderr, "Loading image %d (%s) was successful\n", a, directory);
	#endif
	
	
}

void Put_image(unsigned short a, short x, short y)
{
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put image %d on screen and update its position\n X: %d \n Y: %d\n", a, x ,y);
	#endif

	drawSprite(sprites_img[a], x, y, 0, 0);
}

void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned char f)
{
	Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;
	
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put sprite %d on screen and update its position\n X: %d \n Y: %d\n Frame: %d\n", a, x ,y, f);
	#endif

	drawSpritePart(sprites_img[a], x, y, &frame, 0, 0);
}

void Clear_screen()
{
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Screen was cleared.\n");
	#endif
	
    clearBufferB();
}

void Faster_clearing(short x, short y, unsigned short w, unsigned short h)
{
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Screen was cleared.\n");
	#endif
	
	fillRect(x, y, w, h, 0);
}

void Update_video()
{	
	updateScreen();
}

void Faster_update(short x, short y, short w, short h)
{
	updateScreen();
}

void Sync_refresh()
{
}

void Draw_Pixel(unsigned short x, unsigned short y, unsigned char R, unsigned char G, unsigned char B)
{
	setPixel(x, y, (R >> 3) << 11 | (G >> 2) << 5 | (B >> 3));
}

void Draw_Rect(unsigned short x, unsigned short y, unsigned short width, unsigned short height, unsigned char R, unsigned char G, unsigned char B)
{
	unsigned int _x = max(x, 0), _y = max(y, 0), _w = min(320 - _x, width - _x + x), _h = min(240 - _y, height - _y + y), i, j;
	if(_x < 320 && _y < 240)
	{
		for(j = _y; j < _y + _h; j++)
			for(i = _x; i < _x + _w; i++)
				setPixelUnsafe(i, j, (R >> 3) << 11 | (G >> 2) << 5 | (B >> 3));
	}
}


void Controls()
{
	BUTTON.UP 		= isKeyPressed(Buttons_UP)	 	? 1 : 0;
	BUTTON.DOWN 	= isKeyPressed(Buttons_DOWN)	? 1 : 0;
	BUTTON.LEFT 	= isKeyPressed(Buttons_LEFT)	? 1 : 0;
	BUTTON.RIGHT 	= isKeyPressed(Buttons_RIGHT) 	? 1 : 0;
		
	BUTTON.A 		= isKeyPressed(Buttons_A) ? 1 : 0;
	BUTTON.B 		= isKeyPressed(Buttons_B) ? 1 : 0;
	BUTTON.C 		= isKeyPressed(Buttons_C) ? 1 : 0;
	BUTTON.D 		= isKeyPressed(Buttons_D) ? 1 : 0;
	BUTTON.START 	= isKeyPressed(Buttons_START) ? 1 : 0;
	BUTTON.SELECT 	= isKeyPressed(Buttons_SELECT) ? 1 : 0;
	BUTTON.QUIT 	= isKeyPressed(Buttons_QUIT) ? 1 : 0;
}

void Clear_Image(unsigned short a)
{
	if (sprites_img[a])
	{
		sprites_img[a] = 0;
	}
}

void Clear_Images()
{
	unsigned short i;

	for (i=0;i<MAX_IMAGE;i++)
	{
		if (sprites_img[i])
		{
			sprites_img[i] = NULL;
		}
	}
}


void Init_sound()
{
}

void Load_Music(const char* directory)
{
}

void Play_Music(char loop)
{
}

void Clean_Music()
{
}

void Load_SFX(unsigned char i, const char* directory)
{
}

void Play_SFX(unsigned char i)
{
}

void Unload_SFX()
{
}


void Clearing()
{
	#ifdef DEBUG
		fprintf(stderr, "Clean surface and free SDL from memory\n");
	#endif
	
	Clear_Images();
	deinitBuffering();
}
