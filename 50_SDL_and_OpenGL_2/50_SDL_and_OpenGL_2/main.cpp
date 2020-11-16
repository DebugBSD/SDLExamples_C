/*This source code copyrighted by Lazy Foo' Productions (2004-2020)
and may not be redistributed without written permission.*/

//Using SDL and standard IO
#include <SDL.h>
#include <SDL_opengl.h>
#include <gl\GLU.h>
#include <stdio.h>
#include <string>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

// Starts up SDL, creates the window and initializes OpenGL
bool init();

// Initializes matrices and clear color
bool initGL();

// Input handler
void handleKeys(unsigned char key, int x, int y);

// Per frame update
void update();

// Renders quad to the screen 
void render();

// Frees media and shutdown SDL
void close();

// The window we'll be rendering to
SDL_Window* gWindow = NULL;

// OpenGL context
SDL_GLContext gContext;

// Render flag
bool gRenderQuad = true;

int main( int argc, char* args[] )
{
	init();

	// Enable text input
	SDL_StartTextInput();

	SDL_Event e;

	bool quit = false;
	// While application is running
	while (!quit)
	{
		while (SDL_PollEvent(&e) != 0)
		{
			if (e.type == SDL_QUIT)
			{
				quit = true;
			}
			else if (e.type == SDL_TEXTINPUT)
			{
				int x, y; 
				SDL_GetMouseState(&x , &y);
				handleKeys(e.text.text[0], x, y);
			}
				 
			// Render quad
			render();

			// Update screen
			SDL_GL_SwapWindow(gWindow);
		}
	}

	close();
	//Quit SDL subsystems
	SDL_Quit();

	return 0;
}

bool init()
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
		// Use OpenGL 2.1
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			// Create context
			gContext = SDL_GL_CreateContext(gWindow);
			if (gContext == NULL)
			{
				printf("OpenGL context could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				// Use vsync
				if (SDL_GL_SetSwapInterval(1) < 0)
				{
					printf("warning: Unable to set Vsync! SDL Error: %s\n", SDL_GetError());
				}

				// Initializes OpenGL
				if (!initGL())
				{
					printf("Unable to initialize OpenGL!\n");
					success = false;
				}
			}
		}
	}

	return success;
}

bool initGL()
{
	bool success = true;
	GLenum error = GL_NO_ERROR;

	// Initialize Projection Matrix
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity(); 

	// Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	// Initialize ModelView Matrix
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	// Initialize clear color
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	// Check for error
	error = glGetError();
	if (error != GL_NO_ERROR)
	{
		printf("Error initializing OpenGL! %s\n", gluErrorString(error));
		success = false;
	}

	return success;
}

void handleKeys(unsigned char key, int x, int y)
{
	// Toggle quad
	if (key == 'q')
	{
		gRenderQuad = !gRenderQuad;
	}
}

void update()
{
}

void render()
{
	// Clear Color buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Render quad
	if (gRenderQuad)
	{
		glBegin(GL_QUADS);
		glVertex2f(-0.5f, -0.5f);
		glVertex2f( 0.5f, -0.5f);
		glVertex2f( 0.5f,  0.5f);
		glVertex2f(-0.5f,  0.5f);
		glEnd();
	}
}

void close()
{
}
