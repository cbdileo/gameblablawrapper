/*   COPYRIGHT (C) 2014-2015 GAMEBLABLA   Licensed under the Apache License, Version 2.0 (the "License");   you may not use this file except in compliance with the License.   You may obtain a copy of the License at       http://www.apache.org/licenses/LICENSE-2.0   Unless required by applicable law or agreed to in writing, software   distributed under the License is distributed on an "AS IS" BASIS,   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.   See the License for the specific language governing permissions and   limitations under the License.*//* * JAGUAR PORT OF THE GAMEBLABLA'S WRAPPER * USING REMOVER'S C LIBRARY, THANKS REMOVER'S TEAM ! * UNFINISHED, CURRENTLY NOT WORKING * (PROBABLY WILL STAY THAT WAY UNTIL THERE IS A NEW LIBRARY) *  * WILL BE PARTIALLY COMPATIBLE WITH OTHER PORTS.*/#include <jagdefs.h>#include <jagtypes.h>#include <stdlib.h>#include <interrupt.h>#include <display.h>#include <sprite.h>#include <screen.h>#include <fb2d.h>#include <sound.h>#include <joypad.h>#include <console.h> #include "input.h"struct input BUTTON;unsigned short done = 0;const unsigned short FPS = 60;char* game_name = 0;sprite *sprites_img[64];long joy1; //Need a varible for reading Joypad input.joypad_state *j_state = malloc(sizeof(joypad_state)); //Creat space in memory for joypad input.// JAGUAR// CONTROLLER KEY MAPPINGS#define Buttons_UP JOYPAD_UP#define Buttons_LEFT JOYPAD_LEFT#define Buttons_RIGHT JOYPAD_RIGHT#define Buttons_DOWN JOYPAD_DOWN#define Buttons_A JOYPAD_C#define Buttons_B JOYPAD_B#define Buttons_C JOYPAD_A#define Buttons_D JOYPAD_D#define Buttons_START JOYPAD_PAUSE#define Buttons_SELECT JOYPAD_OPTION#define Buttons_QUIT JOYPAD_0void Init_video(char* argv[]){   	TOMREGS->vmode = RGB16|CSYNC|BGEN|PWIDTH4|VIDEN;  // Setup the type of diplay we wish to work with.	init_interrupts(); // Initialize the basic interupts.	init_display_driver(); // Initalize the display driver.	  	display *d = new_display(0); // Create a pointer for the display so we can reference it later.	d-> x = 0; // set the x position of the display (in relation to the top-left corner).	d-> y = 0; // set the y position of the display (in relation to the top-left corner).		show_display(d);  //Turn on the display, so we can actually see it on the screen.		// Sadly, the default resolution is not 320x240 but something like 352x256	// There must be way to set it to 320X240, JagDoom is using 256x240 after all}/* *	Because Jaguar games are in ROM,  *  there are no directories or what so ever.*/void Load_Image(unsigned short a, phrase directory, int w, int h){	// Black is the transparent colour	sprites_img[a] = new_sprite( w , h , -640 , -480 , DEPTH16 , &directory); /* Set X and Y outside so we can't see it */	attach_sprite_to_display_at_layer(sprites_img[a] , d , a );}void Put_image(unsigned short a, short x, short y){	sprites_img[a]->x = x;	sprites_img[a]->y = y;}void Put_sprite(unsigned short a, short x, short y, unsigned short w, unsigned short h, unsigned char f){	// Obviously Incomplete	// No idea how to show sprites on-screen	sprites_img[a]->x = x; 	sprites_img[a]->y = y;}void Clear_screen(){}void Faster_clearing(short x, short y, unsigned short w, unsigned short h){}void Update_video(){	vsync();}void Faster_update(short x, short y, short w, short h){	Update_video();}void Sync_refresh(){	vsync();}void Controls(){	read_joypad_state(j_state); // This function will find out if we are getting any controller input.	joy1 = j_state->j1; // we take that input and put it in the variable that we created earlier for it.	BUTTON.UP 	= (joy1 & Buttons_UP)	 ? 1 : 0;	BUTTON.DOWN 	= (joy1 & Buttons_DOWN)	 ? 1 : 0;	BUTTON.LEFT 	= (joy1 & Buttons_LEFT)	 ? 1 : 0;	BUTTON.RIGHT 	= (joy1 & Buttons_RIGHT) ? 1 : 0;			BUTTON.A 	= (joy1 & Buttons_A) ? 1 : 0;	BUTTON.B 	= (joy1 & Buttons_B) ? 1 : 0;	BUTTON.C 	= (joy1 & Buttons_C) ? 1 : 0;	BUTTON.D 	= (joy1 & Buttons_D) ? 1 : 0;	BUTTON.START 	= (joy1 & Buttons_START) ? 1 : 0;	BUTTON.SELECT 	= (joy1 & Buttons_SELECT) ? 1 : 0;}void Clear_Image(unsigned short a){}void Clear_Images(){}#ifdef SOUND_ENABLED	void Init_sound()	{	}	void Load_Music(const char* directory)	{	}	void Play_Music(int loop)	{	}	void Clean_Music()	{	}	void Load_SFX(int i, const char* directory)	{	}	void Play_SFX(int i)	{	}	void Unload_GFX()	{	}#else	void Init_sound()	{	}		void Clean_Music()	{	}	void Load_Music(const char* directory)	{	}	void Play_Music(int loop)	{	}	void Load_SFX(int i, const char* directory)	{	}	void Play_SFX(int i)	{	}	void Unload_GFX()	{	}#endifvoid Clearing(){	}