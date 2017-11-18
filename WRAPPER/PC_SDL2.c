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

/*	SDL2 port for PC platforms. Recommended port for Windows, Mac, GNU/Linux and BSDs  */

#if defined(DEBUG) || defined(DEBUG_CRAZY)
	#include <stdio.h>
#endif

#ifdef UNIX
	#include <time.h>
#endif

#include <SDL2/SDL.h>

#ifdef SOUND_ENABLED
	#include <SDL2/SDL_mixer.h>
	#define MAX_SFX 32
	Mix_Music* music;
	Mix_Chunk* sfx_id[MAX_SFX];
#endif

#ifdef IMAGE_CODEC_ENABLED
	#include <SDL2/SDL_image.h>
#endif

#include "INPUT.h"
struct input BUTTON;

const float real_FPS = 1000/FPS_VIDEO;

unsigned short done = 0;
char* game_name = 0;

SDL_Window* game_screen;
SDL_Renderer* renderer;

SDL_GameController *pad;
SDL_Joystick *joy;
short instanceID;

SDL_Texture *sprites_img[MAX_IMAGE];
unsigned short sprites_w_[MAX_IMAGE];
unsigned short sprites_h_[MAX_IMAGE];

#define Buttons_UP SDL_SCANCODE_UP
#define Buttons_LEFT SDL_SCANCODE_LEFT
#define Buttons_RIGHT SDL_SCANCODE_RIGHT
#define Buttons_DOWN SDL_SCANCODE_DOWN
#define Buttons_A SDL_SCANCODE_X
#define Buttons_B SDL_SCANCODE_C
#define Buttons_C SDL_SCANCODE_V
#define Buttons_D SDL_SCANCODE_B
#define Buttons_START SDL_SCANCODE_SPACE
#define Buttons_SELECT SDL_SCANCODE_RETURN
#define Buttons_QUIT SDL_SCANCODE_ESCAPE

#define Joypad_A SDL_CONTROLLER_BUTTON_A 
#define Joypad_B SDL_CONTROLLER_BUTTON_B
#define Joypad_C SDL_CONTROLLER_BUTTON_X
#define Joypad_D SDL_CONTROLLER_BUTTON_Y
#define Joypad_START SDL_CONTROLLER_BUTTON_START 
#define Joypad_SELECT SDL_CONTROLLER_BUTTON_BACK 

void msleep(unsigned char milisec)
{
	#ifdef UNIX
		struct timespec req={0};
		time_t sec=(unsigned short)(milisec/1000);

		milisec=milisec-(sec*1000);
		req.tv_sec=sec;
		req.tv_nsec=milisec*1000000L;

		while(nanosleep(&req,&req)==-1)
		continue;
	#else
		SDL_Delay(milisec);
	#endif
}

void Init_video(char* argv[])
{
	SDL_Init( SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER );
	//game_screen = SDL_CreateWindow(game_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, 0);  
	game_screen = SDL_CreateWindow(game_name, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 320, 240, SDL_WINDOW_FULLSCREEN_DESKTOP);  
	
	/* Create renderer, its resolution, set to HARDWARE aceleration and Vsync turned on */
	renderer = SDL_CreateRenderer(game_screen, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	SDL_RenderSetLogicalSize(renderer, 320, 240);
	
	/* Clear everything on screen */
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
	SDL_RenderPresent(renderer);
	
	/* Hide the mouse and grab it */
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void Load_Image(unsigned short a, const char* directory)
{
	SDL_Surface* tmp;
	
	if (sprites_img[a] != NULL)
	{
		SDL_DestroyTexture(sprites_img[a]);
	}
	
	#ifdef DEBUG
		fprintf(stderr, "Trying to load image %d (%s)...\n", a, directory);
	#endif

	#ifdef IMAGE_CODEC_ENABLED
		tmp = IMG_Load(directory);
	#else
		tmp = SDL_LoadBMP(directory);
	#endif
	
	sprites_w_[a] = tmp->w;
	sprites_h_[a] = tmp->h;
	
	#ifdef DEBUG
		fprintf(stderr, "Loading image %d (%s) in memory was successful\n", a, directory);
	#endif
		
	#ifdef DEBUG
		fprintf(stderr, "Setting transparency for %d\n", a);
	#endif
	
	SDL_SetColorKey(tmp, 1, SDL_MapRGB(tmp->format, 255, 0, 255));
	
	#ifdef DEBUG
		fprintf(stderr, "Done !\n");
	#endif
	
	#ifdef DEBUG
		fprintf(stderr, "Set RLE on Surface %d\n", a);
	#endif
	
	SDL_SetSurfaceRLE(tmp, 1);
	
	#ifdef DEBUG
		fprintf(stderr, "Done !\n");
	#endif

	#ifdef DEBUG
		fprintf(stderr, "Creating texture from surface %d\n", a);
	#endif
	
	sprites_img[a] = SDL_CreateTextureFromSurface(renderer, tmp);
	
	#ifdef DEBUG
		fprintf(stderr, "Done !\n");
	#endif

	#ifdef DEBUG
		fprintf(stderr, "Clearing surfaces...\n");
	#endif
	
	SDL_FreeSurface(tmp);
	
	#ifdef DEBUG
		fprintf(stderr, "Done !\n");
	#endif
}


void Put_image(unsigned short a, short x, short y)
{
	
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put image %d on screen and update its position\n X: %d \n Y: %d\n", a, x ,y);
	#endif	
	
	SDL_Rect position;
	position.x = x;
	position.y = y;
	
	position.w = sprites_w_[a];
	position.h = sprites_h_[a];
	SDL_RenderCopy(renderer, sprites_img[a], NULL, &position);

}

void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned char f)
{
	SDL_Rect position;
	position.x = x;
	position.y = y;
	position.w = w;
	position.h = h;
	
	SDL_Rect frame;
	frame.x = f*w;
	frame.y = 0;
	frame.w = w;
	frame.h = h;
	
	#ifdef DEBUG_CRAZY
		fprintf(stderr, "Put sprite %d on screen and update its position\n X: %d \n Y: %d\n Frame: %d\n", a, x ,y, f);
	#endif
	
	SDL_RenderCopy(renderer, sprites_img[a], &frame, &position);
}

void Clear_screen()
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);
}

void Faster_clearing(short x, short y, unsigned short w, unsigned short h)
{
	Clear_screen();
}

void Update_video()
{  
	Uint32 start;
	start = SDL_GetTicks();
	SDL_RenderPresent(renderer);
	if(real_FPS > SDL_GetTicks()-start) msleep(real_FPS-(SDL_GetTicks()-start));
}

void Faster_update(short x, short y, short w, short h)
{
	Update_video();
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
	SDL_Rect r;
    r.x = x;
    r.y = y;
    r.w = 1;
    r.h = 1;
	
	SDL_SetRenderDrawColor( renderer, R, G, B, 255 );
	SDL_RenderFillRect( renderer, &r );
}


void Controls()
{
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		short x_joy, y_joy;

		x_joy = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTX);
		y_joy = SDL_GameControllerGetAxis(pad, SDL_CONTROLLER_AXIS_LEFTY);
		
		BUTTON.UP = state[Buttons_UP] ? 1 : 0;
		BUTTON.DOWN = state[Buttons_DOWN] ? 1 : 0;
		BUTTON.LEFT = state[Buttons_LEFT] ? 1 : 0;
		BUTTON.RIGHT = state[Buttons_RIGHT] ? 1 : 0;
		
		BUTTON.A = (state[Buttons_A] || SDL_GameControllerGetButton(pad, Joypad_A)) ? 1 : 0;
		BUTTON.B = (state[Buttons_B] || SDL_GameControllerGetButton(pad, Joypad_B)) ? 1 : 0;
		BUTTON.C = (state[Buttons_C] || SDL_GameControllerGetButton(pad, Joypad_C)) ? 1 : 0;
		BUTTON.D = (state[Buttons_D] || SDL_GameControllerGetButton(pad, Joypad_D)) ? 1 : 0;
		BUTTON.START = (state[Buttons_START] || SDL_GameControllerGetButton(pad, Joypad_START)) ? 1 : 0;
		BUTTON.SELECT = (state[Buttons_SELECT] || SDL_GameControllerGetButton(pad, Joypad_SELECT)) ? 1 : 0;
		BUTTON.QUIT = state[Buttons_QUIT] ? 1 : 0;

        if (y_joy < -6000)
        {
			BUTTON.UP = 1;
        }
		else if (y_joy > 6000)
        {
			BUTTON.DOWN = 1;
        }

        if (x_joy < -6000)
        {
			BUTTON.LEFT = 1;
        }
        else if (x_joy > 6000)
        {
			BUTTON.RIGHT = 1;
        }
		
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
                
				case SDL_CONTROLLERDEVICEADDED:
					if( SDL_IsGameController( 0 ) ) 
					{
						pad = SDL_GameControllerOpen( 0 );
						if (pad) 
						{
							joy = SDL_GameControllerGetJoystick( pad );
							instanceID = SDL_JoystickInstanceID( joy );
						}
					}
				break;

            }
        }
		
}


void Clear_Image(unsigned short a)
{
	if (sprites_img[a] != NULL)
	{
		SDL_DestroyTexture(sprites_img[a]);
	}
}

void Clear_Images()
{
	short i;
	
	for (i=0;i<MAX_IMAGE+1;i++) 
	{
		if (sprites_img[i] != NULL)
		{
			SDL_DestroyTexture(sprites_img[i]);
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

		void Play_Music(int loop)
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

		void Load_SFX(int i, char* directory)
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

		void Play_SFX(int i)
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
		fprintf(stderr, "Clean surface and free SDL2 from memory\n");
	#endif
	
	Clear_Images();

	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(game_screen);
	
	#ifdef SOUND_ENABLED
		Unload_SFX();
		Mix_CloseAudio();
	#endif
	
    SDL_Quit();
}
