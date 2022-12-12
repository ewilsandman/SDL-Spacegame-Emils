//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
//#include <stdio.h>
#include <string>
#include <cmath>
#include <cstdlib>
#include <time.h>

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//player pos values
int PlayerSize = 50;
int PlayerX = SCREEN_WIDTH /2 - PlayerSize /2;
int PlayerY = SCREEN_HEIGHT - PlayerSize;

//player shot values
bool ShotActive = false;
int ShotX = 0;
int ShotY = SCREEN_HEIGHT - PlayerSize * 2;
int ShotLenght = 10;

//Wave settings
int MaxWaves = 2;
int CurrentWave = 0;
int EnemiesPerWave = 8;
bool WaveActive = false;
float TimeBetweenWaves; // might not use
int EnemySize = 20;

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

void spawnWave();
void playerShoot();

//shuts down SDL
void close();

//Loads individual image as texture
//SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer = NULL;

class Enemy
{
	private:
	int EnemyX;
	int EnemyY;
	int EnemySize;
	int Speed;
	bool Alive;
	int Movedelay; // in frames
	int FramesSinceMoved;
	public: 
	Enemy() // likely redundant
	{
		EnemyX = 0;
		EnemyY = 0;
		EnemySize = 50;
		Speed = 1;
		Alive = false;
		Movedelay = 6; // in frames, difficulty here??
		FramesSinceMoved = 0;
#if DEBUG
		char Buffer[50];
		sprintf_s(Buffer, "Helo ");
		SDL_LogInfo(0, Buffer);
#endif
	}
	void SetPos(int x, int y)
	{
		EnemyX = x;
		EnemyY = y;
	}
	int GetX()
	{
		return EnemyX;
	}
	int GetY()
	{
		return EnemyY;
	}
	void SetLife(bool input)
	{
		Alive = input;
	}
	bool GetLife()
	{
		return Alive;
	}
	void CheckAndPreformMove()
	{
		if (FramesSinceMoved >= Movedelay)
		{
			EnemyY= EnemyY + Speed;
			FramesSinceMoved = 0;
		}
		else
		{
			FramesSinceMoved++;
		}
	}
};

Enemy Enemies[10];

bool init()
{
	//Initialization flag
	bool success = true;
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
		gWindow = SDL_CreateWindow("Bootleg invaders", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

void playerShoot()
{
	if (ShotActive) // preventing player from shooting more than one shot
	{}
	else
	{
		ShotX = PlayerX + PlayerSize / 2;
		ShotY = SCREEN_HEIGHT - PlayerSize;
#if DEBUG
		char ShotBuffer[50];
		sprintf_s(ShotBuffer, "Firing shot %d", ShotY);
		SDL_LogInfo(0, ShotBuffer);
#endif
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
			srand(time(0));
			
			bool quit = false;
			
			SDL_Event e;
			//While application is running
			while (!quit)
			{
				Uint64 StartLoopTick = SDL_GetPerformanceCounter();
				//SDL_Delay(300);
				//Handle events on queue
				while (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT) // does not seem to work?
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
							spawnWave();
							break;

						case SDLK_LEFT:
							if (PlayerX - 2 < 0)
							{
								break;
							}
							PlayerX = PlayerX - 2;
							break;

						case SDLK_RIGHT:
							if (PlayerX + PlayerSize + 2 > SCREEN_HEIGHT)
							{
								break;
							}
							PlayerX = PlayerX + 2;
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
					ShotY = ShotY - 3;
					if (ShotY < 0)
					{
						ShotActive = false;
# if debug
						sprintf_s(buffer, "Disabling shot %d", ShotY);
						SDL_LogInfo(0, buffer);
#endif
					}
					else
					{
# if debug
						sprintf_s(buffer, "Shot traversing %d", ShotY);
						SDL_LogInfo(0, buffer);
#endif
					}
					SDL_SetRenderDrawColor(gRenderer, 100, 255, 255, 255);
					SDL_Rect shotRect = { ShotX, ShotY, ShotLenght / 2, ShotLenght};
					SDL_RenderFillRect(gRenderer, &shotRect);
				}
				bool WaveHasEnemies = false;

				//Draw enemies and handle collisions/hits
				for (int i = 0; i < EnemiesPerWave; i++)
				{
					int CurrentEnemyX = Enemies[i].GetX();
					int CurrentEnemyY = Enemies[i].GetY();
					if (Enemies[i].GetLife() == true)
					{
						SDL_Rect fillRect = { CurrentEnemyX,  CurrentEnemyY,  EnemySize,  EnemySize };
						SDL_SetRenderDrawColor(gRenderer, 100, 100, 255, 255);
						SDL_RenderFillRect(gRenderer, &fillRect);
						Enemies[i].CheckAndPreformMove();
						WaveHasEnemies = true;
					}
					if (ShotActive)
					{
						if (CurrentEnemyX - ShotLenght /3 < ShotX && ShotX < (CurrentEnemyX + EnemySize))
						{
#if debug
							sprintf_s(buffer, "X in range!");
							SDL_LogInfo(0, buffer);
#endif
							if (CurrentEnemyY < ShotY && ShotY < (CurrentEnemyY + EnemySize))
							{
								Enemies[i].SetLife(false);
								ShotActive = false;
#if debug
								sprintf_s(buffer, "Y in range!");
								SDL_LogInfo(0, buffer);
#endif
							}
						}
					}
					if(CurrentEnemyY == SCREEN_HEIGHT - PlayerSize)
					{
						quit = true;
					}
				}
				if (WaveHasEnemies)
				{ }
				else
				{
					WaveActive = false;
				}

				//Update screen
				SDL_RenderPresent(gRenderer);

				Uint64 EndLoopTick = SDL_GetPerformanceCounter();
				float elapsedMS = (EndLoopTick - StartLoopTick) / (float)SDL_GetPerformanceFrequency() * 1000.0f;

				// Cap to 60 FPS
				SDL_Delay(floor(16.666f - elapsedMS));
			}
			//Free resources and close SDL
			close();

			return 0;
		}
	}
	return 0;
}

void spawnWave()
{
	if (WaveActive)
	{

	}
	else
	{
		char buffer[50];
		if (CurrentWave < MaxWaves)
		{
			for (int i = 1; i < EnemiesPerWave + 1; i++)
			{
				Enemies[i - 1].SetLife(true);
				int randomness = log10(rand()) * 10;
				Enemies[i - 1].SetPos(i * EnemySize * 2 + randomness, 0);
#if debug
				sprintf_s(buffer, "Adding Enemy at %d", Enemies[i].GetX());
				SDL_LogInfo(0, buffer);
#endif
			}
		}
		else
		{
			sprintf_s(buffer, "You win!");
			SDL_LogInfo(0, buffer);
		}
		WaveActive = true;
		CurrentWave = CurrentWave + 1;
	}
}