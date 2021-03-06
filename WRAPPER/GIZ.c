/*

The MIT License (MIT)

Copyright (c) 2016 Gameblabla

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), 
to deal in the Software without restriction, 
including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, 
and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

*/
/*
 * PORT FOR GIZMONDO (using GizSDL)
 * For now, it's just like the PC version.
*/

#define FPS_VIDEO 60
#define MAX_IMAGE 512
#ifndef JOYSTICK
	#define JOYSTICK
#endif

#if defined(DEBUG) || defined(DEBUG_CRAZY)
	#include <stdio.h>
#endif

#include <SDL/SDL.h>

#ifdef SOUND_ENABLED
	#include <SDL/SDL_mixer.h>
	#define MAX_SFX 32

	Mix_Music* music;
	Mix_Chunk* sfx_id[MAX_SFX];
#endif

#ifdef IMAGE_CODEC_ENABLED
	#include <SDL/SDL_image.h>
#endif

#include "INPUT.h"
struct input BUTTON;

const float real_FPS = 1000/FPS_VIDEO;

unsigned short done = 0;
char* game_name = 0;

SDL_Surface *sprites_img[MAX_IMAGE];
SDL_Surface *screen;

#ifdef JOYSTICK
	SDL_Joystick *joy;
#endif

#define Buttons_UP SDLK_UP
#define Buttons_LEFT SDLK_LEFT
#define Buttons_RIGHT SDLK_RIGHT
#define Buttons_DOWN SDLK_DOWN
#define Buttons_A SDLK_x
#define Buttons_B SDLK_c
#define Buttons_C SDLK_v
#define Buttons_D SDLK_b
#define Buttons_START SDLK_SPACE
#define Buttons_SELECT SDLK_BACKSPACE
#define Buttons_QUIT SDLK_ESCAPE

#define Joypad_A 0 
#define Joypad_B 1
#define Joypad_C 2
#define Joypad_D 3
#define Joypad_START 7
#define Joypad_SELECT 5 

void msleep(unsigned char milisec)
{
	SDL_Delay(milisec);
}

void Init_video(char* argv[])
{
	#ifdef JOYSTICK
		SDL_Init( SDL_INIT_VIDEO | SDL_INIT_JOYSTICK );
		SDL_JoystickEventState( SDL_ENABLE );
	#else
		SDL_Init( SDL_INIT_VIDEO );
	#endif
	
	screen = SDL_SetVideoMode(320, 240, 16, SDL_SWSURFACE);
	SDL_ShowCursor(0);
}

void Load_Image(unsigned short a, const char* directory)
{
	
	SDL_Surface *tmp;

	#ifdef DEBUG
		fprintf(stderr, "Clearing surface for image %d...\n", a);
	#endif

	if (sprites_img[a] != NULL)
	{
		SDL_FreeSurface(sprites_img[a]);
	}

	#ifdef DEBUG
		fprintf(stderr, "Trying to load image %d (%s)...\n", a, directory);
	#endif

	#ifdef IMAGE_CODEC_ENABLED
		tmp = IMG_Load(directory);
	#else
		tmp = SDL_LoadBMP(directory);
	#endif

	#ifdef DEBUG
		fprintf(stderr, "Loading image %d (%s) was successful\n", a, directory);
	#endif

	#ifdef DEBUG
		fprintf(stderr, "Trying to set color for transparency on image %d\n", a);
	#endif

	SDL_SetColorKey(tmp, (SDL_SRCCOLORKEY | SDL_RLEACCEL), SDL_MapRGB(tmp->format, 255, 0, 255));

	sprites_img[a] = SDL_DisplayFormat(tmp);

	SDL_FreeSurface(tmp);

	#ifdef DEBUG
		fprintf(stderr, "It seemed to work.\n");
	#endif

}


void Put_image(unsigned short a, short x, short y)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;

	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put image %d on screen and update its position\n X: %d \n Y: %d\n", a, x ,y);
	#endif

	SDL_BlitSurface(sprites_img[a], NULL, screen, &position);
}

void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned char f)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;

	SDL_Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;

	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put sprite %d on screen and update its position\n X: %d \n Y: %d\n Frame: %d\n", a, x ,y, f);
	#endif

	SDL_BlitSurface(sprites_img[a], &frame, screen, &position);
}


void Clear_screen()
{
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Screen was cleared.\n");
	#endif

    SDL_FillRect(screen, 0, 0);
}

void Faster_clearing(short x, short y, unsigned short w, unsigned short h)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;

	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Screen was cleared.\n");
	#endif

    SDL_FillRect(screen, &position, 0);
}

void Update_video()
{
	#ifdef UNCAPPED
		SDL_Flip(screen);
	#else
		Uint32 start;
		start = SDL_GetTicks();
		SDL_Flip(screen);
		if(real_FPS > SDL_GetTicks()-start) msleep(real_FPS-(SDL_GetTicks()-start));
	#endif
}

void Faster_update(short x, short y, short w, short h)
{
	#ifdef UNCAPPED
		SDL_UpdateRect(screen, x, y, w, h);
	#else
		Uint32 start;
		start = SDL_GetTicks();
		SDL_UpdateRect(screen, x, y, w, h);
		if(real_FPS > SDL_GetTicks()-start) msleep(real_FPS-(SDL_GetTicks()-start));
	#endif
}

void Sync_refresh()
{
	#ifndef UNCAPPED
		Uint32 start;
		start = SDL_GetTicks();
		if(real_FPS > SDL_GetTicks()-start) msleep(real_FPS-(SDL_GetTicks()-start));
	#endif
}

void Draw_Pixel(unsigned short x, unsigned short y, unsigned char R, unsigned char G, unsigned char B)
{
	SDL_LockSurface(screen);
	
	int color_draw;
	SDL_Rect scr_draw;
	scr_draw.x = x;
	scr_draw.y = y;
	scr_draw.w = 1;
	scr_draw.h = 1;
	
	color_draw = SDL_MapRGB(screen->format, R, G, B);
	
	SDL_FillRect(screen, &scr_draw, color_draw);
	
	SDL_UnlockSurface(screen);
}

void Controls()
{
    Uint8 *keystate = SDL_GetKeyState(NULL);
    unsigned char joy_b[8];
    short x_joy = 0, y_joy = 0;
    
		#ifdef JOYSTICK
			unsigned char i;
			
			if (SDL_NumJoysticks() > 0)
			{
				joy = SDL_JoystickOpen(0);
			}
        
			for(i=0;i<8;i++)
			{
				joy_b[i] = SDL_JoystickGetButton(joy, i) ? 1 : 0;
			}
			
			x_joy = SDL_JoystickGetAxis(joy, 0);
			y_joy = SDL_JoystickGetAxis(joy, 1);
			
			SDL_JoystickUpdate();
		#endif
		
		BUTTON.UP = keystate[Buttons_UP] ? 1 : 0;
		BUTTON.DOWN = keystate[Buttons_DOWN] ? 1 : 0;
		BUTTON.LEFT = keystate[Buttons_LEFT] ? 1 : 0;
		BUTTON.RIGHT = keystate[Buttons_RIGHT] ? 1 : 0;
		
		BUTTON.A = (keystate[Buttons_A] || joy_b[Joypad_A]) ? 1 : 0;
		BUTTON.B = (keystate[Buttons_B] || joy_b[Joypad_B]) ? 1 : 0;
		BUTTON.C = (keystate[Buttons_C] || joy_b[Joypad_C]) ? 1 : 0;
		BUTTON.D = (keystate[Buttons_D] || joy_b[Joypad_D]) ? 1 : 0;
		BUTTON.START = (keystate[Buttons_START] || joy_b[Joypad_START]) ? 1 : 0;
		BUTTON.SELECT = (keystate[Buttons_SELECT] || joy_b[Joypad_SELECT]) ? 1 : 0;
		BUTTON.QUIT = keystate[Buttons_QUIT] ? 1 : 0;
			
		#ifdef JOYSTICK		
			if (y_joy < -5000)
			{
				BUTTON.UP = 1;
			}
			else if (y_joy > 5000)
			{
				BUTTON.DOWN = 1;
			}

			if (x_joy < -5000)
			{
				BUTTON.LEFT = 1;
			}
			else if (x_joy > 5000)
			{
				BUTTON.RIGHT = 1;
			}
        	#endif

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                {
                    done = 1;
                    break;
                }
            }
        }
}

void Clear_Image(unsigned short a)
{
	if (sprites_img[a] != NULL)
	{
		SDL_FreeSurface(sprites_img[a]);
	}
}

void Clear_Images()
{
	short i;

	for (i=0;i<MAX_IMAGE+1;i++)
	{
		if (sprites_img[i] != NULL)
		{
			SDL_FreeSurface(sprites_img[i]);
		}
	}
}

#ifdef SOUND_ENABLED

		void Init_sound()
		{
			#ifdef DEBUG
				fprintf(stderr, "Init sound system\n");
			#endif
				Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,1024);
				Mix_AllocateChannels(MAX_SFX);
		}
		
		void Clean_Music()
		{
			#ifdef DEBUG
				fprintf(stderr, "Stop and clear music from memory\n");
			#endif

			if (music)
			{
				Mix_HaltMusic();
				Mix_FreeMusic(music);
				music = NULL;
			}
		}

		void Load_Music(const char* directory)
		{
			#ifdef DEBUG
				fprintf(stderr, "Load music in memory\n");
			#endif
			music = Mix_LoadMUS(directory);
		}

		void Play_Music(char loop)
		{
			if (loop == 1)
			{
				#ifdef DEBUG
					fprintf(stderr, "Playing music in loop\n");
				#endif
				Mix_PlayMusic(music, -1);
			}
			else
			{
				#ifdef DEBUG
					fprintf(stderr, "Playing music\n");
				#endif
				Mix_PlayMusic(music, 0);
			}
		}


		void Load_SFX(unsigned char i, const char* directory)
		{
			#ifdef DEBUG
				fprintf(stderr, "Load sound effect %d (%s) in memory\n", i, directory);
			#endif

			if (sfx_id[i])
			{
				Mix_FreeChunk(sfx_id[i]);
				sfx_id[i] = NULL;
			}
			sfx_id[i] = Mix_LoadWAV(directory);
		}

		void Play_SFX(unsigned char i)
		{
			#ifdef DEBUG
				fprintf(stderr, "Play sound effect %d loaded in memory\n", i);
			#endif

			Mix_PlayChannel(-1, sfx_id[i], 0) ;
		}

		void Unload_SFX()
		{
			short i;

			#ifdef DEBUG
				fprintf(stderr, "Free sound effect from memory\n");
			#endif

			for (i=0;i<MAX_SFX;i++)
			{
				if (sfx_id[i])
				{
					Mix_FreeChunk(sfx_id[i]);
					sfx_id[i] = NULL;
				}
			}
		}

#else
		void Init_sound()
		{
		}
		
		void Clean_Music()
		{
		}

		void Load_Music(const char* directory)
		{
		}

		void Play_Music(char loop)
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
#endif



void Clearing()
{
	#ifdef SOUND_ENABLED
		Clean_Music();
	#endif

	#ifdef DEBUG
		fprintf(stderr, "Clean surface and free SDL from memory\n");
	#endif

	Clear_Images();

	SDL_FreeSurface(screen);
	
	#ifdef SOUND_ENABLED
		Unload_SFX();
		Mix_CloseAudio();
	#endif
	
   	SDL_Quit();
}
