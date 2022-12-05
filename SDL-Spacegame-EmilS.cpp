//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <SDL_image.h>
#include <stdio.h>
#include <string>
#include <cmath>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//Start pos const

//player pos values
int PlayerSize = 50;
int PlayerX = SCREEN_WIDTH /2 - PlayerSize /2;
int PlayerY = SCREEN_HEIGHT - PlayerSize;

//player shot values
bool ShotActive = false;
int ShotX = 0;
int ShotY = SCREEN_HEIGHT - PlayerSize * 2;
int ShotLenght = 10;

//Enemy settings
int MaxWaves = 2;
int EnemiesPerWave = 3;
float TimeBetweenWaves;
int EnemySize;



//Store key state (unused)

enum KeyPress
{
	KEY_PRESS_DEFAULT,
	KEY_PRESS_UP,
	KEY_PRESS_DOWN,
	KEY_PRESS_LEFT,
	KEY_PRESS_RIGHT,
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Handles Enemies;
void handleEnemy(int);
///Ideas for handling enemies
/// Bool[], flip bool to false when hit, use relative distance between enemies, move in sync
/// Enemy[], similar to first but now enemies can be diffrent, operate independently
///
void playerShoot();

//shuts down SDL
void close();

//Loads individual image as texture
//SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

bool init()
{
	//Initialization flag
	bool success = true;
	auto test = SDL_IMAGE_COMPILEDVERSION;
	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		//Set texture filtering to linear
		if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
		{
			printf("Warning: Linear texture filtering not enabled!");
		}

		//Create window
		gWindow = SDL_CreateWindow("SDL Spacegame thing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Create renderer for window
			gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
			if (gRenderer == NULL)
			{
				printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
				success = false;
			}
			else
			{
				//Initialize renderer color	
				SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

				//Initialize PNG loading (currently unused)
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

bool loadMedia()
{
	//Loading success flag
	bool success = true;

	//Nothing to load
	return success;
}

void handleEnemy(int)
{

}

void playerShoot()
{
	char ShotBuffer[50];
	if (ShotActive) // preventing player from shooting more than one shot
	{

	}
	else
	{
		ShotX = PlayerX + PlayerSize / 2;
		ShotY = SCREEN_HEIGHT - PlayerSize;
		sprintf_s(ShotBuffer, "Firing shot %d", ShotY);
		SDL_LogInfo(0, ShotBuffer);
		ShotActive = true;
	}
}

void close()
{
	//Destroy window	
	SDL_DestroyRenderer(gRenderer);
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
	gRenderer = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
}

int main(int argc, char* args[])
{
	char buffer[50];
	//Start up SDL and create window
	if (!init())
	{
		printf("Failed to initialize!\n");
	}
	else
	{
		//Load media (unused)
		if (!loadMedia())
		{
			printf("Failed to load media!\n");
		}
		else
		{
			//Main loop flag
			bool quit = false;

			//Event handler
			SDL_Event e;

			//While application is running
			while (!quit)
			{
				//SDL_Delay(300);
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)
					{
						quit = true;
					}
					else if (e.type == SDL_KEYDOWN) // this part is quite hacky, might fix later
					{
						//Select surfaces based on key press
						switch (e.key.keysym.sym)
						{

						case SDLK_UP:
						playerShoot();
							break;
							
						case SDLK_DOWN:
							if (PlayerY + PlayerSize + 3 > SCREEN_WIDTH)
							{
								break;
							}
							PlayerY = PlayerY + 3;
							break;

						case SDLK_LEFT:
							if (PlayerX - 3 < 0)
							{
								break;
							}
							PlayerX = PlayerX - 3;
							break;

						case SDLK_RIGHT:
							if (PlayerX + PlayerSize + 3 > SCREEN_HEIGHT)
							{
								break;
							}
							PlayerX = PlayerX + 3;
							break;

						default:
							break;
						}
					}
				}
				//Clear screen
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 0, 0);
				SDL_RenderClear(gRenderer);

				//Draw player
				SDL_Rect fillRect = { PlayerX, PlayerY,  PlayerSize,  PlayerSize   };
				SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
				SDL_RenderFillRect(gRenderer, &fillRect);

				//shot go up
				if (ShotActive)
				{
					ShotY = ShotY - 1;
					if (ShotY < 0)
					{
						ShotActive = false;
						sprintf_s(buffer, "Disabling shot %d", ShotY);
						SDL_LogInfo(0, buffer);
					}
					else
					{
						sprintf_s(buffer, "Shot traversing %d", ShotY);
						SDL_LogInfo(0, buffer);
					}
					SDL_SetRenderDrawColor(gRenderer, 100, 255, 255, 255);
					SDL_Rect shotRect = { ShotX, ShotY, ShotLenght / 2, ShotLenght};
					SDL_RenderFillRect(gRenderer, &shotRect);
				}

				//Draw midscreen horisontal line
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
				SDL_RenderDrawLine(gRenderer, 0, SCREEN_HEIGHT / 2, SCREEN_WIDTH, SCREEN_HEIGHT / 2);

				//Draw midscreen vertical line
				SDL_SetRenderDrawColor(gRenderer, 0, 0, 255, 255);
				SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH / 2, 0, SCREEN_WIDTH / 2, SCREEN_HEIGHT);

				//Update screen
				SDL_RenderPresent(gRenderer);
			}
		}
	}

	//Free resources and close SDL
	close();

	return 0;
}















// Stuff that might be usefull later
/* SDL_Texture* loadTexture(std::string path)
{
	//The final texture
	SDL_Texture* newTexture = NULL;

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load(path.c_str());
	/
	if (loadedSurface == NULL)
	{
		printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
	}
	else
	{
		//Create texture from surface pixels
		newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
		if (newTexture == NULL)
		{
			printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
		}

		//Get rid of old loaded surface
		SDL_FreeSurface(loadedSurface);
	}

	return newTexture;
}*/