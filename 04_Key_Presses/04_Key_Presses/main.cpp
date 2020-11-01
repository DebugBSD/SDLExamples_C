/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Key press surfaces constants
enum KeyPressSurfaces
{
	KEY_PRESS_SURFACE_DEFAULT,
	KEY_PRESS_SURFACE_UP,
	KEY_PRESS_SURFACE_DOWN,
	KEY_PRESS_SURFACE_LEFT,
	KEY_PRESS_SURFACE_RIGHT,
	KEY_PRESS_SURFACE_TOTAL
};

// Some global variables
// The Window we'll be rendering to
SDL_Window* gWindow = NULL;

// The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

// Current displayed image
SDL_Surface* gCurrentSurface = NULL;

// The images that correspond to a keypress
SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

// Starts up SDL and creates the window
bool Init();

// Loads media
bool LoadMedia();

// Free media files and shut down SDL
void Close();

// Loads individual image
SDL_Surface* loadSurface(const char* pPath);

int main(int argc, char* args[])
{
	if (!Init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		// Load media
		if (LoadMedia())
		{
			// Main loop flag
			bool quit = false;

			// Event Handler 
			SDL_Event e;

			// Set default current surface
			gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

			// Handle events on queue
			while (!quit)
			{
				while (SDL_PollEvent(&e) != 0)
				{
					// User requests to quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN)
					{
						// Select surfaces based on key press
						switch (e.key.keysym.sym)
						{
						case SDLK_UP:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
							break;
						case SDLK_DOWN:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
							break;
						case SDLK_LEFT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
							break;
						case SDLK_RIGHT:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
							break;
						default:
							gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
							break;
						}
					}
				}

				// Apply the image
				SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

				// Update the surface 
				SDL_UpdateWindowSurface(gWindow);
			}
		}
	}

	// Free resources and close SDL
	Close();

	return 0;
}

bool Init()
{
	// Initialization flag

	bool success = true;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			gScreenSurface = SDL_GetWindowSurface(gWindow);
		}
	}

	return success;
}

bool LoadMedia()
{
	const char* pFile = "press.bmp";
	// Loading success flag
	bool success = true;

	// Load default surface
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = SDL_LoadBMP(pFile);
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pFile, SDL_GetError());
		success = false;
	}

	// Load default surface
	pFile = "up.bmp";
	gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = SDL_LoadBMP(pFile);
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pFile, SDL_GetError());
		success = false;
	}

	pFile = "down.bmp";
	gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = SDL_LoadBMP(pFile);
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pFile, SDL_GetError());
		success = false;
	}

	pFile = "left.bmp";
	gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = SDL_LoadBMP(pFile);
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pFile, SDL_GetError());
		success = false;
	}

	pFile = "right.bmp";
	gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = SDL_LoadBMP(pFile);
	if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pFile, SDL_GetError());
		success = false;
	}

	return success;
}

void Close()
{
	// Deallocate surfaces
	for (int i = 0; i < KEY_PRESS_SURFACE_TOTAL; i++)
	{
		SDL_FreeSurface(gKeyPressSurfaces[i]);
		gKeyPressSurfaces[i] = NULL;
	}

	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	SDL_Quit();
}

SDL_Surface* loadSurface(const char* pPath)
{
	// Load image at specified path
	SDL_Surface* loadedSurface = SDL_LoadBMP(pPath);
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pPath, SDL_GetError());
	}
	return loadedSurface;
}
