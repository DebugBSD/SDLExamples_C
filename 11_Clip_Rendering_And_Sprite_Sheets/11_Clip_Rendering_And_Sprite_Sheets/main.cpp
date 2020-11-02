/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>

// Definitions
// ----------------------------------------------------------------------------

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Texture wrapper class
class LTexture
{
public:
	// Initializes variables
	LTexture();

	// Deallocates memory
	~LTexture();

	// Loads image at specified path
	bool loadFromFile(const char* pPath);

	// Deallocate texture
	void free();

	// Render texture at given point
	void render(int x = 0, int y = 0, SDL_Rect * clip = NULL);

	// Gets image dimensions
	int getWidth();
	int getHeight();

private:
	// The actual hardware texture
	SDL_Texture* m_pTexture;

	// Image dimensions
	int m_Width;
	int m_Height;

};

// Some global variables
// ----------------------------------------------------------------------------

// The Window we'll be rendering to
SDL_Window* gWindow = NULL;

// The Window Renderer
SDL_Renderer* gRenderer = NULL;

// Scene textures
SDL_Rect gSpriteClips[4];
LTexture gSpriteSheetTexture;

// Starts up SDL and creates the window
bool Init();

// Loads media
bool LoadMedia();

// Free media files and shut down SDL
void Close();


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
				}

				// Clear Screen
				SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
				SDL_RenderClear(gRenderer);

				// Render the top left sprite
				gSpriteSheetTexture.render(0, 0, &gSpriteClips[0]);

				// Render the top right sprite
				gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[1].w, 0, &gSpriteClips[1]);

				// Render the bottom left sprite
				gSpriteSheetTexture.render(0, SCREEN_HEIGHT - gSpriteClips[2].h, &gSpriteClips[2]);

				// Render the bottom right sprite
				gSpriteSheetTexture.render(SCREEN_WIDTH - gSpriteClips[3].w, SCREEN_HEIGHT - gSpriteClips[3].h, &gSpriteClips[3]);

				// Update screen
				SDL_RenderPresent(gRenderer);
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
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (!gRenderer)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Initialize renderer color
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				// Initialize PNG loading
				int imgFlags = IMG_INIT_PNG;
				if (!(IMG_Init(imgFlags) & imgFlags))
				{
					printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia(void)
{
	// Loading success flag
	bool success = true;

	// Load Foo texture
	if (!gSpriteSheetTexture.loadFromFile("dots.png"))
	{
		printf("Failed to load Foo texture image!\n");
		success = false;
	}
	else
	{
		// Set top left sprite
		gSpriteClips[0].x = 0;
		gSpriteClips[0].y = 0;
		gSpriteClips[0].w = 100;
		gSpriteClips[0].h = 100;

		// Set top left sprite
		gSpriteClips[1].x = 100;
		gSpriteClips[1].y = 0;
		gSpriteClips[1].w = 100;
		gSpriteClips[1].h = 100;

		// Set top left sprite
		gSpriteClips[2].x = 0;
		gSpriteClips[2].y = 100;
		gSpriteClips[2].w = 100;
		gSpriteClips[2].h = 100;

		// Set top left sprite
		gSpriteClips[3].x = 100;
		gSpriteClips[3].y = 100;
		gSpriteClips[3].w = 100;
		gSpriteClips[3].h = 100;
	}

	// Nothing to load
	return success;
}

void Close()
{
	// Deallocate surface
	gSpriteSheetTexture.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}


LTexture::LTexture()
{
	// Initialize
	m_pTexture = NULL;
	m_Width = m_Height = 0;
}

LTexture::~LTexture()
{
	// Deallocate 
	free();
}

bool LTexture::loadFromFile(const char* pPath)
{
	// The final texture
	SDL_Texture* newTexture = NULL;

	// Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(pPath);
	if (!loadedSurface)
	{
		printf("Unable to load image %s! SDL Error: %s\n", pPath, SDL_GetError());
	}
	else
	{
		// Color key image
		SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));

		// Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (!newTexture)
		{
			printf("Unable to create texture from %s!: SDL Error: %s\n", pPath, SDL_GetError());
		}
		else
		{
			m_Width = loadedSurface->w;
			m_Height = loadedSurface->h;
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	m_pTexture = newTexture;
	return m_pTexture != NULL;
}

void LTexture::free()
{
	// Free texture if it exists
	if (m_pTexture != NULL)
	{
		SDL_DestroyTexture(m_pTexture);
		m_pTexture = NULL;
		m_Width = m_Height = 0;
	}
}

void LTexture::render(int x, int y, SDL_Rect *pClip)
{
	// Get rendering space and render to screen;
	SDL_Rect renderQuad{ x, y, m_Width, m_Height };

	// Set clip rendering dimensions
	if (pClip != NULL)
	{
		renderQuad.w = pClip->w;
		renderQuad.h = pClip->h;
	}

	// Render to screen
	SDL_RenderCopy(gRenderer, m_pTexture, pClip, &renderQuad);
}

int LTexture::getWidth()
{
	return m_Width;
}

int LTexture::getHeight()
{
	return m_Height;
}
