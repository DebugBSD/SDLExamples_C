/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>
#include <stdio.h>
#include <string>
#include <sstream>
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
	void handleEvent(SDL_Event &e);

	// Deallocates internals
	void free();

	// Window dimensions
	int getWidth() { return m_Width; }
	int getHeight() { return m_Height; }

	// Window focii
	bool hasMouseFocus() { return m_MouseFocus; }
	bool hasKeyboardFocus() { return m_KeyboardFocus; }
	bool isMinimized() { return m_Minimized; }
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

	// Takes key presses and adjusts the dot's velocity
	void handleEvent(SDL_Event& e);

	// Moves the dot and checks collision
	void move();

	// Shows the dot on the screen relative to the camera
	void render(int camX = 0, int camY = 0);

	// Position accessors
	int getPosX() { return m_PosX; }
	int getPosY() { return m_PosY; }


private:

	// Initialize the offsets
	int	m_PosX;
	int m_PosY;

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

// Font textures
LTexture gSceneTexture;

// Starts up SDL and creates the window
bool Init();

// Loads media
bool LoadMedia();

// Free media files and shut down SDL
void Close();

// Circle/Circle collision detection
bool CheckCollision(Circle& a, Circle& b);

// Circle/Box collision detection
bool CheckCollision(Circle& a, SDL_Rect& b);

// Calculate the distance squared between two points
double distanceSquared(int x1, int y1, int x2, int y2);


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

			// Current input point
			int currentData = 0;

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
					
					// Handle window events
					gWindow.handleEvent(e);

				}

				// Only draw when not minimized
				if (!gWindow.isMinimized())
				{
					// Render game
					// Clear Screen
					SDL_SetRenderDrawColor(gRenderer, 0xff, 0xff, 0xff, 0xff);
					SDL_RenderClear(gRenderer);

					// Render text textures
					gSceneTexture.render((gWindow.getWidth() - gSceneTexture.getWidth()) / 2, (gWindow.getHeight() - gSceneTexture.getHeight() ) / 2);

					// Draw HUD

					// Update screen
					SDL_RenderPresent(gRenderer);
				}
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

bool LoadMedia(void)
{
	// Loading success flag
	bool success = true;

	// loading texture
	if (!gSceneTexture.loadFromFile("window.png"))
	{
		success = false;
	}

	// Nothing to load
	return success;
}

void Close()
{
	gSceneTexture.free();

	//Destroy window
	SDL_DestroyRenderer(gRenderer);
	gWindow.free();

	//Quit SDL subsystems
	TTF_Quit();
	IMG_Quit();
	SDL_Quit();
}

bool CheckCollision(Circle& a, Circle& b)
{
	// Calculate total radius squared
	int totalRadiusSquared = a.r + b.r;
	totalRadiusSquared = totalRadiusSquared * totalRadiusSquared;

	// If the distance between the centers of the circles is less than the sum of ther radii
	if (distanceSquared(a.x, a.y, b.x, b.y) < (totalRadiusSquared))
	{
		//The circles have collided
		return true;
	}

	return false;
}

bool CheckCollision(Circle& a, SDL_Rect& b)
{
	// Closest point on collision box
	int cX, cY;

	// Find closest x offset
	if (a.x < b.x)
	{
		cX = b.x;
	}
	else if (a.x > b.x + b.w)
	{
		cX = b.x + b.w;
	}
	else
	{
		cX = a.x;
	}

	// Find closest y offset
	if (a.y < b.y)
	{
		cY = b.y;
	}
	else if (a.y > b.y + b.h)
	{
		cY = b.y + b.h;
	}
	else
	{
		cY = a.y;
	}

	// If the closest point is inside the circle
	if (distanceSquared(a.x, a.y, cX, cY) < a.r * a.r)
	{
		// This box and the circle have collided
		return true;
	}

	return false;
}

double distanceSquared(int x1, int y1, int x2, int y2)
{
	int deltaX = x2 - x1;
	int deltaY = y2 - y1;
	return deltaX * deltaX + deltaY * deltaY;
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

int LTexture::getWidth()
{
	return m_Width;
}

int LTexture::getHeight()
{
	return m_Height;
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
	m_PosX = x;  m_PosY = y;
	m_VelX = m_VelY = 0;

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

void Dot::move()
{
	// Move the dot left or right
	m_PosX += m_VelX;

	// IF the dot went too far to the left or the right or the dot collided
	if ((m_PosX < 0) || (m_PosX + DOT_WIDTH > LEVEL_WIDTH))
	{
		// Move back
		m_PosX -= m_VelX;
	}

	// Move the dot up or down
	m_PosY += m_VelY;

	// IF the dot went too far to the left or the right or the dot collided
	if ((m_PosY < 0) || (m_PosY + DOT_HEIGHT > LEVEL_HEIGHT))
	{
		// Move back
		m_PosY -= m_VelY;
	}
}

void Dot::render(int camX, int camY)
{
	// Show the dot
	//gDotTexture.render(m_PosX - camX, m_PosY - camY);
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
