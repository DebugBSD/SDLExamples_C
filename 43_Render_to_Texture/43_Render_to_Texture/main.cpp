/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/
#define _CRT_SECURE_NO_WARNINGS
//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <fstream>
#include <cmath>
#include <vector>

// Definitions
// ----------------------------------------------------------------------------

// Dimensions of the level
const int LEVEL_WIDTH = 1280;
const int LEVEL_HEIGHT = 960;

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
const int WALKING_ANIMATION_FRAMES = 4;
const int SCREEN_FPS = 60;
const int SCREEN_TICKS_PER_FRAME = 1000 / SCREEN_FPS;

// Button constants
const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 4;

// Total data points
const int TOTAL_DATA = 10;

// Particle count
const int TOTAL_PARTICLES = 20;

// Tile constants
const int TILE_WIDTH = 80;
const int TILE_HEIGHT = 80;
const int TOTAL_TILES = 192;
const int TOTAL_TILE_SPRITES = 12;

// The different tile sprites
const int TILE_RED = 0;
const int TILE_GREEN = 1;
const int TILE_BLUE = 2;
const int TILE_CENTER = 3;
const int TILE_TOP = 4;
const int TILE_TOPRIGHT = 5;
const int TILE_RIGHT = 6;
const int TILE_BOTTOMRIGHT = 7;
const int TILE_BOTTOM = 8;
const int TILE_BOTTOMLEFT = 9;
const int TILE_LEFT = 10;
const int TILE_TOPLEFT = 11;

// Tile class
class Tile
{
public:
	// Constructior 
	Tile(int x, int y, int tileType);

	// Shows the tile
	void render(SDL_Rect& camera);

	// Get the tile type
	int getType() { return m_Type; }

	// Get the collision box
	SDL_Rect getBox() { return m_Box; }

private:
	// Collider
	SDL_Rect m_Box;

	// Type of tile
	int m_Type;
};

// Forward declarations
class Particle;

enum LButtonSprite
{
	BUTTON_SPRITE_MOUSE_OUT = 0,
	BUTTON_SPRITE_MOUSE_OVER_MOTION,
	BUTTON_SPRITE_MOUSE_DOWN,
	BUTTON_SPRITE_MOUSE_UP,
	BUTTON_SPRITE_TOTAL
};

// A cricle structure
struct Circle
{
	int x;
	int y;
	int r;
};

class LWindow
{
public:
	// Initialize internals
	LWindow();

	// Create window
	bool init();

	// Create renderer from internal window
	SDL_Renderer* createRenderer();

	// Handles window events
	void handleEvent(SDL_Event& e);

	// Deallocates internals
	void free();

	// Window dimensions
	int getWidth() { return m_Width; }
	int getHeight() { return m_Height; }

	// Window focii
	bool hasMouseFocus() { return m_MouseFocus; }
	bool hasKeyboardFocus() { return m_KeyboardFocus; }
	bool isMinimized() { return m_Minimized; }

	// Get SDL Window handle
	SDL_Window* getWindow() { return m_pWindow; }
private:

	// Window data
	SDL_Window* m_pWindow;

	// Window dimensions
	int m_Width;
	int m_Height;

	// Window focus
	bool m_MouseFocus;
	bool m_KeyboardFocus;
	bool m_Fullscreen;
	bool m_Minimized;

};

// The dot that will move around on the screen
class Dot
{
public:
	// The dimensions of the dot
	static const int DOT_WIDTH = 20;
	static const int DOT_HEIGHT = 20;

	// Maximum axis velocity of the dot
	static const int DOT_VEL = 10;

	// Constructor
	Dot(int x = 0, int y = 0);

	// Destructor
	~Dot();

	// Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	// Moves the dot and checks collision against tiles
	void move(Tile* tiles[]);

	// Center the camera over the dot
	void setCamera(SDL_Rect& camera);

	// Shows the dot on the screen relative to the camera
	void render(SDL_Rect& camera);

private:
	// Collision box of the dot
	SDL_Rect m_Box;

	// Velocity of the dot
	int m_VelX;
	int m_VelY;

};

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

	// Create image from font string
	bool loadFromRenderedText(const char* pTextureText, SDL_Color textColor);

	// Create a blank texture
	bool createBlank(int width, int height, SDL_TextureAccess access);

	// Deallocate texture
	void free();

	// Set color modulation
	void setColor(Uint8 red, Uint8 green, Uint8 blue);

	// Set blending mode
	void setBlendMode(SDL_BlendMode blending);

	// Set alpha modulation
	void setAlpha(Uint8 alpha);

	// Render texture at given point
	void render(int x = 0, int y = 0, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* pCenter = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

	// Set self as render target
	void setAsRenderTarget();

	// Gets image dimensions
	int getWidth();
	int getHeight();

	// Pixel manipulators
	bool lockTexture();
	bool unlockTexture();
	void* getPixels();
	void copyPixels(void* pixels);
	int getPitch();
	Uint32 getPixel32(unsigned int x, unsigned int y);

private:
	// The actual hardware texture
	SDL_Texture* m_pTexture;
	void* m_Pixels;
	int m_Pitch;
	// Image dimensions
	int m_Width;
	int m_Height;

};

// A test animation stream
class DataStream
{
public:
	// Constructor
	DataStream();

	// Load initial data
	bool loadMedia();

	// Deallocator
	void free();

	// Gets current frame data
	void* getBuffer();

private:
	// Internal data
	SDL_Surface* m_Images[4];
	int m_CurrentImage;
	int m_DelayFrames;
};

// Bitmap font
class LBitmapFont
{
public:
	// Default constructor
	LBitmapFont();

	// Generate the font
	bool buildFont(LTexture* bitmap);

	// Shows the text
	void renderText(int x, int y, const char* pText);

private:
	// The font texture;
	LTexture* m_pBitmap;

	// Individual characters in the surface
	SDL_Rect m_Chars[256];

	// Spacing variables
	int m_NewLine;
	int m_Space;
};

// Particle
class Particle
{
public:
	// Initialize position and animation
	Particle(int x, int y);

	// Shows the particle
	void render();

	// Checks if particle is dead
	bool isDead();

private:
	// Offsets;
	int m_PosX;
	int m_PosY;

	// Current frame of animation;
	int m_Frame;

	// Type of particle;
	LTexture* m_pTexture;
};

// The mouse button
class LButton
{
public:
	// Initializes internal variables
	LButton();

	// Sets top left position
	void setPosition(int x = 0, int y = 0);

	// Handles mouse event
	void handleEvent(SDL_Event* e);

	// Show button sprites
	void render();

private:
	// Top left position
	SDL_Point m_Position;

	// Currently used global sprite
	LButtonSprite m_CurrentSprite;
};

class LTimer
{
public:
	// Initializes variables
	LTimer();

	// The various check actions
	void start();
	void stop();
	void pause();
	void resume();

	// Gets the timer's time
	Uint32 getTicks();

	// Checks the status of the timer
	bool isStarted() { return m_Started; }
	bool isPaused() { return m_Paused && m_Started; }

private:

	// The clock time when the timer started
	Uint32 m_StartTicks;

	// The ticks stored when the timer was paused
	Uint32 m_PausedTicks;

	// The timer status
	bool m_Paused;
	bool m_Started;
};

// Some global variables
// ----------------------------------------------------------------------------

// Our custom window
LWindow gWindow;

// The Window Renderer
SDL_Renderer* gRenderer = NULL;

// Globally used font
TTF_Font* gFont = NULL;

SDL_Rect gTileClips[TOTAL_TILE_SPRITES];
LTexture gTileTexture;

// Dot texture 
LTexture gFooTexture;
LTexture gDotTexture;

// Particles textures
LTexture gBlueTexture;
LTexture gRedTexture;
LTexture gGreenTexture;
LTexture gShimmerTexture;

// Foo Font texture
LBitmapFont gBitmapFont;

//Scene textures
LTexture gStreamingTexture;

//Animation stream
DataStream gDataStream;

// Target texture
LTexture gTargetTexture;

// Starts up SDL and creates the window
bool Init();

// Loads media
bool LoadMedia();

// Free media files and shut down SDL
void Close();

// Box collision detection
bool CheckCollision(SDL_Rect a, SDL_Rect b);

// Checks collision box against set of tiles
bool TouchesWall(SDL_Rect box, Tile* tiles[]);

// Set tiles from tile map
bool SetTiles(Tile* tiles[]);

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

			// Rotation variables
			double angle = 0;
			SDL_Point screenCenter{ SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };

			// Handle events on queue
			while (!quit)
			{
				// The renderer text flag
				bool renderText = false;

				// Process input
				while (SDL_PollEvent(&e) != 0)
				{
					// User requests to quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}

					// Handle window and user events
					//dot.handleEvent(e);

				}

				// Update game
				//dot.move(NULL);
				// rotate
				angle += 2;
				if (angle > 360)
				{
					angle -= 360;
				}

				// Set self as render target
				gTargetTexture.setAsRenderTarget();

				// Render game
				// Clear Screen
				SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
				SDL_RenderClear(gRenderer);

				// Render red filled quad
				SDL_Rect fillRect{ SCREEN_WIDTH / 4, SCREEN_HEIGHT / 4, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 };
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0,0,0xFF);
				SDL_RenderFillRect(gRenderer, &fillRect);

				// Render green outlined quad
				SDL_Rect outlineRect{ SCREEN_WIDTH / 6, SCREEN_HEIGHT / 6, SCREEN_WIDTH * 2 / 3, SCREEN_HEIGHT * 2 / 3 };
				SDL_SetRenderDrawColor(gRenderer, 0, 0xFF, 0, 0xFF);
				SDL_RenderDrawRect(gRenderer, &outlineRect);

				// Draw blue horizontal line
				SDL_SetRenderDrawColor(gRenderer, 0,0,0xff, 0xff);
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

				// Draw vertical line of yellow dots
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0, 0xFF);
				for (int i = 0; i < SCREEN_HEIGHT; i += 4)
				{
					SDL_RenderDrawPoint(gRenderer, SCREEN_WIDTH / 2, i);
				}

				// Reset render target
				SDL_SetRenderTarget(gRenderer, NULL);

				// Show rendered to texture
				gTargetTexture.render(0, 0, NULL, angle, &screenCenter);
				// Draw HUD

				// Update screen
				SDL_RenderPresent(gRenderer);

				// Other stuff

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
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		if (!gWindow.init())
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			gRenderer = gWindow.createRenderer();
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

				// Initialize SDL_ttf
				if (TTF_Init() == -1)
				{
					printf("SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError());
					success = false;
				}

				// Initialize SDL_Mixer
				if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
				{
					printf("SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError());
					success = false;
				}
			}
		}
	}

	return success;
}

bool LoadMedia()
{
	//Loading success flag
	bool success = true;

	// Load blank texture
	if (!gTargetTexture.createBlank(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_TEXTUREACCESS_TARGET))
	{
		printf("Failed to create streaming texture!\n");
		success = false;
	}

	return success;
}

void Close()
{
	//Free loaded images
	gStreamingTexture.free();
	gDataStream.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}


bool CheckCollision(SDL_Rect a, SDL_Rect b)
{
	//The sides of the rectangles
	int leftA, leftB;
	int rightA, rightB;
	int topA, topB;
	int bottomA, bottomB;

	//Calculate the sides of rect A
	leftA = a.x;
	rightA = a.x + a.w;
	topA = a.y;
	bottomA = a.y + a.h;

	//Calculate the sides of rect B
	leftB = b.x;
	rightB = b.x + b.w;
	topB = b.y;
	bottomB = b.y + b.h;

	//If any of the sides from A are outside of B
	if (bottomA <= topB)
	{
		return false;
	}

	if (topA >= bottomB)
	{
		return false;
	}

	if (rightA <= leftB)
	{
		return false;
	}

	if (leftA >= rightB)
	{
		return false;
	}

	//If none of the sides from A are outside B
	return true;
}

bool TouchesWall(SDL_Rect box, Tile* tiles[])
{
	// Go through the tiles
	for (int i = 0; i < TOTAL_TILES; i++)
	{
		// If the tile is a wall type tile
		if ((tiles[i]->getType() >= TILE_CENTER) && (tiles[i]->getType() <= TILE_TOPLEFT))
		{
			// If the collision box touches the wall tile
			if (CheckCollision(box, tiles[i]->getBox()))
			{
				return true;
			}
		}
	}

	// If no wall tiles were touched
	return false;
}

bool SetTiles(Tile* tiles[])
{
	// Success flag
	bool tilesLoaded = true;

	// The tile offsets
	int x = 0;
	int y = 0;

	// Open the map
	std::ifstream map("lazy.map");

	// If the map couldn't be loaded
	if (map.fail())
	{
		printf("Unable to load map file!\n");
		tilesLoaded = false;
	}
	else
	{
		// Initialize the tiles
		for (int i = 0; i < TOTAL_TILES; i++)
		{
			// Determines what kind of tile will be made
			int tileType = -1;

			// Read from map file
			map >> tileType;

			// If there was a problem in reading the map
			if (map.fail())
			{
				// Stop loading the map
				printf("Error loading map: Unexpected end of file!\n");
				tilesLoaded = false;
				break;
			}

			// If the number is a valid tile number
			if ((tileType >= 0) && (tileType < TOTAL_TILE_SPRITES))
			{
				tiles[i] = new Tile(x, y, tileType);
			}
			else
			{
				// Stop loading the map
				printf("Error loading map: Invalid tile type at %d!\n", i);
				tilesLoaded = false;
				break;
			}

			// Move to the next tile spot
			x += TILE_WIDTH;

			// If we've gone too far
			if (x >= LEVEL_WIDTH)
			{
				x = 0;

				// Move to the next row
				y += TILE_HEIGHT;
			}
		}

		// Clip the sprite sheet
		if (tilesLoaded)
		{
			gTileClips[TILE_RED].x = 0;
			gTileClips[TILE_RED].y = 0;
			gTileClips[TILE_RED].w = TILE_WIDTH;
			gTileClips[TILE_RED].h = TILE_HEIGHT;

			gTileClips[TILE_GREEN].x = 0;
			gTileClips[TILE_GREEN].y = 80;
			gTileClips[TILE_GREEN].w = TILE_WIDTH;
			gTileClips[TILE_GREEN].h = TILE_HEIGHT;

			gTileClips[TILE_BLUE].x = 0;
			gTileClips[TILE_BLUE].y = 160;
			gTileClips[TILE_BLUE].w = TILE_WIDTH;
			gTileClips[TILE_BLUE].h = TILE_HEIGHT;

			gTileClips[TILE_TOPLEFT].x = 80;
			gTileClips[TILE_TOPLEFT].y = 0;
			gTileClips[TILE_TOPLEFT].w = TILE_WIDTH;
			gTileClips[TILE_TOPLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_LEFT].x = 80;
			gTileClips[TILE_LEFT].y = 80;
			gTileClips[TILE_LEFT].w = TILE_WIDTH;
			gTileClips[TILE_LEFT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMLEFT].x = 80;
			gTileClips[TILE_BOTTOMLEFT].y = 160;
			gTileClips[TILE_BOTTOMLEFT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMLEFT].h = TILE_HEIGHT;

			gTileClips[TILE_TOP].x = 160;
			gTileClips[TILE_TOP].y = 0;
			gTileClips[TILE_TOP].w = TILE_WIDTH;
			gTileClips[TILE_TOP].h = TILE_HEIGHT;

			gTileClips[TILE_CENTER].x = 160;
			gTileClips[TILE_CENTER].y = 80;
			gTileClips[TILE_CENTER].w = TILE_WIDTH;
			gTileClips[TILE_CENTER].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOM].x = 160;
			gTileClips[TILE_BOTTOM].y = 160;
			gTileClips[TILE_BOTTOM].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOM].h = TILE_HEIGHT;

			gTileClips[TILE_TOPRIGHT].x = 240;
			gTileClips[TILE_TOPRIGHT].y = 0;
			gTileClips[TILE_TOPRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_TOPRIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_RIGHT].x = 240;
			gTileClips[TILE_RIGHT].y = 80;
			gTileClips[TILE_RIGHT].w = TILE_WIDTH;
			gTileClips[TILE_RIGHT].h = TILE_HEIGHT;

			gTileClips[TILE_BOTTOMRIGHT].x = 240;
			gTileClips[TILE_BOTTOMRIGHT].y = 160;
			gTileClips[TILE_BOTTOMRIGHT].w = TILE_WIDTH;
			gTileClips[TILE_BOTTOMRIGHT].h = TILE_HEIGHT;
		}
	}

	map.close();

	return tilesLoaded;
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
	// Remove existing texture
	free();

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
		// Convert surface to display format
		SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		if (!formattedSurface)
		{
			SDL_Log("Unable to convert loaded surface to display format! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Create blank streamable texture
			newTexture = SDL_CreateTexture(gRenderer, SDL_GetWindowPixelFormat(gWindow.getWindow()), SDL_TEXTUREACCESS_STREAMING, formattedSurface->w, formattedSurface->h);
			if (!newTexture)
			{
				printf("Unable to create texture from %s!: SDL Error: %s\n", pPath, SDL_GetError());
			}
			else
			{
				// Enable blending on texture
				SDL_SetTextureBlendMode(newTexture, SDL_BLENDMODE_BLEND);

				// Lock texture for manipulation 
				SDL_LockTexture(newTexture, &formattedSurface->clip_rect, &m_Pixels, &m_Pitch);

				// Copy loaded/formatted surface pixels
				memcpy(m_Pixels, formattedSurface->pixels, formattedSurface->pitch * formattedSurface->h);

				// Get image dimensions
				m_Width = loadedSurface->w;
				m_Height = loadedSurface->h;

				// Get pixel data in editable format
				Uint32* pixels = (Uint32*)m_Pixels;
				int pixelCount = (m_Pitch / 4) * m_Height;

				// Map colors
				Uint32 colorKey = SDL_MapRGB(formattedSurface->format, 0, 0xFF, 0xFF);
				Uint32 transparent = SDL_MapRGBA(formattedSurface->format, 0, 0xFF, 0xFF, 0);

				// Color key pixels
				for (int i = 0; i < pixelCount; i++)
				{
					if (pixels[i] == colorKey)
					{
						pixels[i] = transparent;
					}
				}

				// Unlock texture to update
				SDL_UnlockTexture(newTexture);
				m_Pixels = NULL;

			}
			// Get rid of old formatted surface
			SDL_FreeSurface(formattedSurface);
		}

		// Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}
	m_pTexture = newTexture;
	return m_pTexture != NULL;
}

bool LTexture::loadFromRenderedText(const char* pTextureText, SDL_Color textColor)
{

	// Get rid of preexisting texture
	free();

	// Render text surface
	SDL_Surface* pTextSurface = TTF_RenderText_Solid(gFont, pTextureText, textColor);
	if (!pTextSurface)
	{
		printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
	}
	else
	{
		// Create texture from surface pixels
		m_pTexture = SDL_CreateTextureFromSurface(gRenderer, pTextSurface);
		if (!m_pTexture)
		{
			printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
		}
		else
		{
			// Get image dimensions
			m_Width = pTextSurface->w;
			m_Height = pTextSurface->h;
		}

		// Get rid of old surface
		SDL_FreeSurface(pTextSurface);
	}

	return m_pTexture != NULL;

	return true;
}

bool LTexture::createBlank(int width, int height , SDL_TextureAccess access)
{
	// Create uninitialized texture
	m_pTexture = SDL_CreateTexture(gRenderer, SDL_PIXELFORMAT_RGBA8888, access, width, height);
	if (m_pTexture == NULL)
	{
		printf("Unable to create blank texture! SDL Error: %s\n", SDL_GetError());
		return false;
	}
	else
	{
		m_Width = width;
		m_Height = height;
		return true;
	}
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

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue)
{
	// Modulate texture
	SDL_SetTextureColorMod(m_pTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending)
{
	// Set blending function
	SDL_SetTextureBlendMode(m_pTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha)
{
	// Set texture alpha
	SDL_SetTextureAlphaMod(m_pTexture, alpha);
}

void LTexture::render(int x, int y, SDL_Rect* pClip, double angle, SDL_Point* pCenter, SDL_RendererFlip flip)
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
	SDL_RenderCopyEx(gRenderer, m_pTexture, pClip, &renderQuad, angle, pCenter, flip);
}

void LTexture::setAsRenderTarget()
{
	// Make self render target
	SDL_SetRenderTarget(gRenderer, m_pTexture);
}

int LTexture::getWidth()
{
	return m_Width;
}

int LTexture::getHeight()
{
	return m_Height;
}

bool LTexture::lockTexture()
{
	bool success = true;

	// Texture is already lockd
	if (m_Pixels != NULL)
	{
		SDL_Log("Texture is already locked!\n");
		success = false;
	}
	else // Lock texture
	{
		if (SDL_LockTexture(m_pTexture, NULL, &m_Pixels, &m_Pitch) != 0)
		{
			SDL_Log("Unable to lock the texture! %s\n", SDL_GetError());
			success = false;
		}
	}
	return success;
}

bool LTexture::unlockTexture()
{
	bool success = true;

	// Texture is not lockd
	if (m_Pixels == NULL)
	{
		SDL_Log("Texture is not locked!\n");
		success = false;
	}
	else // Unlock texture
	{
		SDL_UnlockTexture(m_pTexture);
		m_Pixels = NULL;
		m_Pitch = 0;
	}

	return success;
}

void* LTexture::getPixels()
{
	return m_Pixels;
}

void LTexture::copyPixels(void* pixels)
{
	// Texture is locked
	if (m_Pixels != NULL)
	{
		// Copy to locked pixels
		memcpy(m_Pixels, pixels, m_Pitch * m_Height);
	}
}

int LTexture::getPitch()
{
	return m_Pitch;
}

Uint32 LTexture::getPixel32(unsigned int x, unsigned int y)
{
	// Convert the pixels to 32bit
	Uint32* pixels = (Uint32*)m_Pixels;

	// Get the pixel requested
	return pixels[(y * (m_Pitch / 4)) + x];
}

LButton::LButton()
{
	m_Position.x = m_Position.y = 0;

	m_CurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
}

void LButton::setPosition(int x, int y)
{
	m_Position.x = x;
	m_Position.y = y;
}

void LButton::handleEvent(SDL_Event* e)
{
	// if mouse event happened
	if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP)
	{
		// Check if mouse is in button
		bool inside = true;

		// Get mouse position
		int x, y;
		SDL_GetMouseState(&x, &y);

		// Mouse is left of the button
		if (x < m_Position.x)
		{
			inside = false;
		}
		// Mouse is right of the button
		else if (x > m_Position.x + BUTTON_WIDTH)
		{
			inside = false;
		}
		// Mouse is above the button
		else if (y < m_Position.y)
		{
			inside = false;
		}
		// Mouse is below the button
		else if (y > m_Position.y + BUTTON_HEIGHT)
		{
			inside = false;
		}
		// Mouse is outside button
		if (!inside)
		{
			m_CurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
		}
		// Mouse is inside button
		else
		{
			// Set mouse over sprite
			switch (e->type)
			{
			case SDL_MOUSEMOTION:
				m_CurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
				break;
			case SDL_MOUSEBUTTONDOWN:
				m_CurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
				break;
			case SDL_MOUSEBUTTONUP:
				m_CurrentSprite = BUTTON_SPRITE_MOUSE_UP;
				break;
			}
		}
	}
}

void LButton::render()
{
	// Show current button sprite
	//gButtonSpriteSheetTexture.render(m_Position.x, m_Position.y, &gSpriteClips[m_CurrentSprite]);
}

LTimer::LTimer()
{
	// Initializes the variables;

	m_StartTicks = m_PausedTicks = 0;
	m_Paused = m_Started = false;
}

void LTimer::start()
{
	//Initialize the variables
	m_StartTicks = SDL_GetTicks();
	m_PausedTicks = 0;

	m_Paused = false;
	m_Started = true;
}

void LTimer::stop()
{
	//Initialize the variables
	m_StartTicks = 0;
	m_PausedTicks = 0;

	m_Paused = false;
	m_Started = false;
}

void LTimer::pause()
{
	// if the timer is running and isn't already paused
	if (m_Started && !m_Paused)
	{
		// Pause the timer
		m_Paused = true;

		// Calculate the paused ticks
		m_PausedTicks = SDL_GetTicks() - m_StartTicks;
		m_StartTicks = 0;
	}

}

void LTimer::resume()
{
	// If the timer is running and paused
	if (m_Started && m_Paused)
	{
		// Unpause the timer
		m_Paused = false;

		// Reset the starting ticks
		m_StartTicks = SDL_GetTicks() - m_PausedTicks;

		// Reset paused ticks
		m_PausedTicks = 0;
	}
}

Uint32 LTimer::getTicks()
{
	// The actual timer time
	Uint32 time = 0;

	// If the timer is running
	if (m_Started)
	{
		// If the timer is paused
		if (m_Paused)
		{
			// Return the number of ticks when the timer was paused
			time = m_PausedTicks;
		}
		else
		{
			// Return the current time minus the start time
			time = SDL_GetTicks() - m_StartTicks;
		}
	}

	return time;
}

Dot::Dot(int x, int y)
{

	m_Box.x = 0;
	m_Box.y = 0;
	m_Box.w = DOT_WIDTH;
	m_Box.h = DOT_HEIGHT;

	//Initialize the velocity
	m_VelX = 0;
	m_VelY = 0;

}

Dot::~Dot()
{
}

void Dot::handleEvent(SDL_Event& e)
{
	// If a key was pressed
	if (e.type == SDL_KEYDOWN && e.key.repeat == 0)
	{
		// Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_VelY -= DOT_VEL; break;
		case SDLK_DOWN: m_VelY += DOT_VEL; break;
		case SDLK_LEFT: m_VelX -= DOT_VEL; break;
		case SDLK_RIGHT: m_VelX += DOT_VEL; break;
		}
	}
	else if (e.type == SDL_KEYUP && e.key.repeat == 0)
	{
		// Adjust the velocity
		switch (e.key.keysym.sym)
		{
		case SDLK_UP: m_VelY += DOT_VEL; break;
		case SDLK_DOWN: m_VelY -= DOT_VEL; break;
		case SDLK_LEFT: m_VelX += DOT_VEL; break;
		case SDLK_RIGHT: m_VelX -= DOT_VEL; break;
		}
	}
}

void Dot::setCamera(SDL_Rect& camera)
{
	// Center the camera over the dot
	camera.x = (m_Box.x + DOT_WIDTH / 2) - SCREEN_WIDTH / 2;
	camera.y = (m_Box.y + DOT_HEIGHT / 2) - SCREEN_HEIGHT / 2;

	// Keep the camera in bounds
	if (camera.x < 0) camera.x = 0;
	if (camera.y < 0) camera.y = 0;
	if (camera.x > LEVEL_WIDTH - camera.w) camera.x = LEVEL_WIDTH - camera.w;
	if (camera.y > LEVEL_HEIGHT - camera.h) camera.y = LEVEL_HEIGHT - camera.h;

}

void Dot::move(Tile* tiles[])
{
	// Move the dot left or right
	m_Box.x += m_VelX;

	// IF the dot went too far to the left or the right or the dot collided
	if ((m_Box.x < 0) || (m_Box.x + DOT_WIDTH > LEVEL_WIDTH)/* || TouchesWall(m_Box, tiles)*/)
	{
		// Move back
		m_Box.x -= m_VelX;
	}

	// Move the dot up or down
	m_Box.y += m_VelY;

	// IF the dot went too far to the left or the right or the dot collided
	if ((m_Box.y < 0) || (m_Box.y + DOT_HEIGHT > LEVEL_HEIGHT)/* || TouchesWall(m_Box, tiles)*/)
	{
		// Move back
		m_Box.y -= m_VelY;
	}
}

void Dot::render(SDL_Rect& camera)
{
	// Show the dot
	gFooTexture.render(m_Box.x - camera.x, m_Box.y - camera.y);

}

LWindow::LWindow()
{
	m_pWindow = NULL;
	m_MouseFocus = m_KeyboardFocus = m_Fullscreen = m_Minimized = false;
	m_Width = m_Height = 0;
}

bool LWindow::init()
{
	//Create window
	m_pWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (m_pWindow != NULL)
	{
		m_MouseFocus = true;
		m_KeyboardFocus = true;
		m_Width = SCREEN_WIDTH;
		m_Height = SCREEN_HEIGHT;
	}
	return m_pWindow != NULL;
}

SDL_Renderer* LWindow::createRenderer()
{
	return SDL_CreateRenderer(m_pWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);;
}

void LWindow::handleEvent(SDL_Event& e)
{
	// Window event ocurred
	if (e.type == SDL_WINDOWEVENT)
	{
		// Caption update flag
		bool updateCaption = false;

		switch (e.window.event)
		{
			// Get new dimensions and repaint on window size change
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			m_Width = e.window.data1;
			m_Height = e.window.data2;
			SDL_RenderPresent(gRenderer);
			break;
			// Repaint on exposure
		case SDL_WINDOWEVENT_EXPOSED:
			SDL_RenderPresent(gRenderer);
			break;
			// Mouse entered window
		case SDL_WINDOWEVENT_ENTER:
			m_MouseFocus = true;
			updateCaption = true;
			break;
			// Mouse left window
		case SDL_WINDOWEVENT_LEAVE:
			m_MouseFocus = false;
			updateCaption = true;
			break;
			// Window has keyboard focus
		case SDL_WINDOWEVENT_FOCUS_GAINED:
			m_KeyboardFocus = true;
			updateCaption = true;
			break;
			// Window lost keyboard focus
		case SDL_WINDOWEVENT_FOCUS_LOST:
			m_KeyboardFocus = false;
			updateCaption = true;
			break;
			// Window minimized
		case SDL_WINDOWEVENT_MINIMIZED:
			m_Minimized = true;
			break;
			// Window maximized
		case SDL_WINDOWEVENT_MAXIMIZED:
			m_Minimized = false;
			break;
			// Window restored
		case SDL_WINDOWEVENT_RESTORED:
			m_Minimized = false;
			break;
		}
		if (updateCaption)
		{
			std::stringstream caption;
			caption << "SDL Tutorial - MouseFocus: " << ((m_MouseFocus) ? "On" : "Off") << " KeyboardFocus: " << ((m_KeyboardFocus) ? "On" : "Off");
			SDL_SetWindowTitle(m_pWindow, caption.str().c_str());
		}
	}
	// Enter exit full screen on return key
	else if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_RETURN)
	{
		if (m_Fullscreen)
		{
			SDL_SetWindowFullscreen(m_pWindow, SDL_FALSE);
			m_Fullscreen = false;
		}
		else
		{
			SDL_SetWindowFullscreen(m_pWindow, SDL_TRUE);
			m_Fullscreen = true;
			m_Minimized = false;
		}
	}

}

void LWindow::free()
{
	SDL_DestroyWindow(m_pWindow);

	m_pWindow = NULL;
	m_MouseFocus = m_KeyboardFocus = m_Fullscreen = m_Minimized = false;
	m_Width = m_Height = 0;
}

Particle::Particle(int x, int y)
{
	// Set offsets
	m_PosX = x - 5 + (rand() % 25);
	m_PosY = y - 5 + (rand() % 25);

	// Initialize animation
	m_Frame = rand() % 5;

	// Set type
	switch (rand() % 3)
	{
	case 0: m_pTexture = &gRedTexture; break;
	case 1: m_pTexture = &gGreenTexture; break;
	case 2: m_pTexture = &gBlueTexture; break;
	}
}

void Particle::render()
{
	// Show image
	m_pTexture->render(m_PosX, m_PosY);

	// Show shimer
	if (m_Frame % 2 == 0)
	{
		gShimmerTexture.render(m_PosX, m_PosY);
	}

	// Animate
	m_Frame++;
}

bool Particle::isDead()
{
	return m_Frame > 10;
}

Tile::Tile(int x, int y, int tileType) :
	m_Box{ x,y, TILE_WIDTH, TILE_HEIGHT },
	m_Type{ tileType }
{
}

void Tile::render(SDL_Rect& camera)
{
	// If the tiles is on the screen
	if (CheckCollision(camera, m_Box))
	{
		// Show the tile
		gTileTexture.render(m_Box.x - camera.x, m_Box.y - camera.y, &gTileClips[m_Type]);
	}
}

LBitmapFont::LBitmapFont() :
	m_pBitmap{ NULL },
	m_NewLine{ 0 },
	m_Space{ 0 }
{
}

bool LBitmapFont::buildFont(LTexture* bitmap)
{
	bool success = true;

	// Lock pixels for access
	if (!bitmap->lockTexture())
	{
		SDL_Log("Unable to lock bitmap font texture!\n");
		success = false;
	}
	else
	{
		// Set the background color
		Uint32 bgColor = bitmap->getPixel32(0, 0);

		// Set the cell dimensions
		int cellW = bitmap->getWidth() / 16;
		int cellH = bitmap->getHeight() / 16;

		// New line variables
		int top = cellH;
		int baseA = cellH;

		// The current character we're setting
		int currentChar = 0;

		// Go through the cell rows
		for (int rows = 0; rows < 16; rows++)
		{
			// Go through the cell columns
			for (int cols = 0; cols < 16; cols++)
			{
				// Set the character offset
				m_Chars[currentChar].x = cellW * cols;
				m_Chars[currentChar].y = cellH * rows;

				// Set the dimensions of the character
				m_Chars[currentChar].w = cellW;
				m_Chars[currentChar].h = cellH;

				// Find left side
				// Go through pixel columns
				for (int pCol = 0; pCol < 16; pCol++)
				{
					// Go through pixel rows
					for (int pRow = 0; pRow < 16; pRow++)
					{
						// Get the pixel offsets
						int pX = (cellW * cols) + pCol;
						int pY = (cellH * rows) + pRow;

						// if a non colorkey is found
						if (bitmap->getPixel32(pX, pY) != bgColor)
						{
							// Set the x offset
							m_Chars[currentChar].x = pX;

							// Break the loops
							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				// Find right side
				// Go through pixel columns
				for (int pColW = cellW - 1; pColW >= 0; pColW--)
				{
					// Go through pixel rows
					for (int pRowW = 0; pRowW < cellH; pRowW++)
					{
						// Get the pixel offsets
						int pX = (cellW * cols) + pColW;
						int pY = (cellH * rows) + pRowW;

						// if a non colorkey is found
						if (bitmap->getPixel32(pX, pY) != bgColor)
						{
							// Set the x offset
							m_Chars[currentChar].w = (pX - m_Chars[currentChar].x) + 1;

							// Break the loops
							pColW = -1;
							pRowW = cellH;
						}
					}
				}

				// Find top
				// Go through pixel rows
				for (int pRow = 0; pRow < cellH; pRow++)
				{
					// Go through pixel rows
					for (int pCol = 0; pCol < cellW; pCol++)
					{
						// Get the pixel offsets
						int pX = (cellW * cols) + pCol;
						int pY = (cellH * rows) + pRow;

						// if a non colorkey is found
						if (bitmap->getPixel32(pX, pY) != bgColor)
						{
							// if new top is found
							if (pRow < top)
							{
								top = pRow;
							}

							// Break the loops
							pCol = cellW;
							pRow = cellH;
						}
					}
				}

				// Find bottom of A
				if (currentChar == 'A')
				{
					// Go through pixel rows
					for (int pRow = cellH - 1; pRow >= 0; pRow--)
					{
						// Go through pixel rows
						for (int pCol = 0; pCol < cellW; pCol++)
						{
							// Get the pixel offsets
							int pX = (cellW * cols) + pCol;
							int pY = (cellH * rows) + pRow;

							// if a non colorkey is found
							if (bitmap->getPixel32(pX, pY) != bgColor)
							{
								baseA = pRow;

								// Break the loops
								pCol = cellW;
								pRow = -1;
							}
						}
					}
				}
				// Go to the next character
				currentChar++;
			}
		}

		// Calculate space
		m_Space = cellW / 2;

		// Calculate new line
		m_NewLine = baseA - top;

		// Loop off excess top pixels
		for (int i = 0; i < 256; i++)
		{
			m_Chars[i].y += top;
			m_Chars[i].h -= top;
		}

		bitmap->unlockTexture();
		m_pBitmap = bitmap;
	}

	return success;
}

void LBitmapFont::renderText(int x, int y, const char* pText)
{
	// If the font has been build
	if (m_pBitmap != NULL)
	{
		// Temp offsets
		int curX = x;
		int curY = y;

		// Go through the text
		for (int i = 0; i < strlen(pText); i++)
		{
			// if the current character is a space
			if (pText[i] == ' ')
			{
				// Move over
				curX += m_Space;
			}
			// If the current character is new line
			else if (pText[i] == '\n')
			{
				// Move down
				curY += m_NewLine;

				// Move back
				curX = x;
			}
			else
			{
				// Get the ASCII value of the character
				int ascii = (unsigned char)pText[i];

				// Show the character
				m_pBitmap->render(curX, curY, &m_Chars[ascii]);

				// Move over the width of the character with one pixel of padding
				curX += m_Chars[ascii].w + 1;
			}
		}
	}
}

DataStream::DataStream() :
	m_Images{ NULL, NULL, NULL, NULL },
	m_CurrentImage{ 0 },
	m_DelayFrames{ 4 }
{
}

bool DataStream::loadMedia()
{
	bool success = true;

	for (int i = 0; i < 4; ++i)
	{
		char path[64] = "";
		sprintf(path, "foo_walk_%d.png", i);

		SDL_Surface* loadedSurface = IMG_Load(path);
		if (loadedSurface == NULL)
		{
			printf("Unable to load %s! SDL_image error: %s\n", path, IMG_GetError());
			success = false;
		}
		else
		{
			m_Images[i] = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
		}

		SDL_FreeSurface(loadedSurface);
	}

	return success;
}

void DataStream::free()
{
	for (int i = 0; i < 4; ++i)
	{
		SDL_FreeSurface(m_Images[i]);
	}
}

void* DataStream::getBuffer()
{
	--m_DelayFrames;
	if (m_DelayFrames == 0)
	{
		++m_CurrentImage;
		m_DelayFrames = 4;
	}

	if (m_CurrentImage == 4)
	{
		m_CurrentImage = 0;
	}

	return m_Images[m_CurrentImage]->pixels;
}
