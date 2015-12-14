/*This source code copyrighted by Lazy Foo' Productions (2004-2015)
and may not be redistributed without written permission.*/

//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>
#include <windows.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
	{
		printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
		{
			printf( "Warning: Linear texture filtering not enabled!" );
		}

		//Create window
		gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
		if( gWindow == NULL )
		{
			printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
			if( gRenderer == NULL )
			{
				printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
				success = false;
			}
			else
			{
				//Initialize renderer color
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				//Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
			}
		}
	}

	return success;
}

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void close()
{
	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

void DrawRect(int x1, int y1, int x2, int y2, int x3, int y3,int x4,int y4)
{
    SDL_RenderDrawLine( gRenderer, x1, y1, x2, y2 );
    SDL_RenderDrawLine( gRenderer, x1, y1, x3, y3 );
    SDL_RenderDrawLine( gRenderer, x3, y3, x4, y4 );
    SDL_RenderDrawLine( gRenderer, x2, y2, x4, y4 );
}

void Rotate(int angleD, int x, int y,float *x1,float *y1,float *x2, float *y2, float *x3, float *y3, float *x4, float *y4)
{
    //float angleR=angleD*M_PI/180;

    *x1=x+(*x1-x)*cosf(angleD*M_PI/180)-(*y1-y)*sinf(angleD*M_PI/180);
    *y1=y+(*x1-x)*sinf(angleD*M_PI/180)+(*y1-y)*cosf(angleD*M_PI/180);

    *x2=x+(*x2-x)*cosf(angleD*M_PI/180)-(*y2-y)*sinf(angleD*M_PI/180);
    *y2=y+(*x2-x)*sinf(angleD*M_PI/180)+(*y2-y)*cosf(angleD*M_PI/180);

    *x3=x+(*x3-x)*cosf(angleD*M_PI/180)-(*y3-y)*sinf(angleD*M_PI/180);
    *y3=y+(*x3-x)*sinf(angleD*M_PI/180)+(*y3-y)*cosf(angleD*M_PI/180);

    *x4=x+(*x4-x)*cosf(angleD*M_PI/180)-(*y4-y)*sinf(angleD*M_PI/180);
    *y4=y+(*x4-x)*sinf(angleD*M_PI/180)+(*y4-y)*cosf(angleD*M_PI/180);
    //printf("(%f) - (%f) - (%f) - (%f) - (%f) - (%f) - (%f) - (%f)\n",*x1,*y1,*x2,*y2,*x3,*y3,*x4,*y4);
    //printf("(%f)\n",*x1);

}


int main( int argc, char* args[] )
{
	//Start up SDL and create window
	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		//Load media
		if( !loadMedia() )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;
                int H=50;
                int W=100;
				int x=200;
				int y=200;
				float x1;
				float x2;
				float x3;
				float x4;
				float y1;
				float y2;
				float y3;
				float y4;
				x1=x;
				y1=y;
				x2=x+W-1;
				y2=y;
				x3=x;
				y3=y-H+1;
				x4=x+W-1;
				y4=y-H+1;
			//While application is running
			int k=0;
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
				}

				//Clear screen
				SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
				SDL_RenderClear( gRenderer );
                SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0xFF, 0xFF );
				DrawRect(static_cast<int>(x1),static_cast<int>(y1),static_cast<int>(x2),static_cast<int>(y2),static_cast<int>(x3),static_cast<int>(y3),static_cast<int>(x4),static_cast<int>(y4));
				Rotate(10,250,175,&x1,&y1,&x2,&y2,&x3,&y3,&x4,&y4);
				Sleep(10);
                SDL_RenderPresent( gRenderer );
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}
