//Using SDL, SDL_image, standard IO, math, and strings
#include <SDL.h>
#include <string>
#include <cmath>
#include <cstdlib>
#include <time.h> // used for random generation
#include "ECS.h"

//Screen dimension constants
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 800;

//player pos values
int PlayerSize = 50;
int PlayerX = SCREEN_WIDTH /2 - PlayerSize /2;
int PlayerY = SCREEN_HEIGHT - PlayerSize;

//shot values
bool ShotActive = false;
int ShotX = 0;
int ShotY = SCREEN_HEIGHT - PlayerSize * 2;
int ShotLenght = 10;

//Wave settings
int MaxWaves = 2;
int CurrentWave = 0;
const int EnemiesPerWave = 8;
bool WaveActive = false;
float TimeBetweenWaves; // might not use
int Movedelay = 6; // in frames
int FramesSinceMoved = 0;
int EnemySize = 20;
bool WaveHasEnemies = false;

//Starts up SDL and creates window
bool init();

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

//Does ECS magic
Coordinator gCoordinator;

SDL_Rect EnemyPositions[EnemiesPerWave];
SDL_Color ColoursToDraw[12];
//int CurrentRenderObject = 0;
bool AliveArray[EnemiesPerWave];

std::vector<Entity> Enemies(EnemiesPerWave);
#if 0
struct Enemy
{
	private:
	int EnemyX = 0;
	int EnemyY = 0;
	int EnemySize = 50;
	int Speed = 1;
	bool Alive = false;
	Enemy() // likely redundant
	{
#if 0
		char Buffer[50];
		sprintf_s(Buffer, "Helo ");
		SDL_LogInfo(0, Buffer);
#endif
	}
	/*void CheckAndPreformMove()
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
	}*/
};

//Enemy Enemies[10];
#endif
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
		gWindow = SDL_CreateWindow("Bootleg invaders: Now using ECS", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
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

void playerShoot()
{
	if (ShotActive) // preventing player from shooting more than one shot
	{}
	else
	{
		ShotX = PlayerX + PlayerSize / 2;
		ShotY = SCREEN_HEIGHT - PlayerSize;
#if 0
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
		gCoordinator.Init();

		gCoordinator.RegisterComponent<SpeedComponent>();
		gCoordinator.RegisterComponent<PositionComponent>();
		gCoordinator.RegisterComponent<LifeComponent>();


		auto movementSystem = gCoordinator.RegisterSystem<MovementSystem>();
		auto collisionSystem = gCoordinator.RegisterSystem<CollisionSystem>();

		Signature signature;
		signature.set(gCoordinator.GetComponentType<SpeedComponent>());
		signature.set(gCoordinator.GetComponentType<PositionComponent>());
		signature.set(gCoordinator.GetComponentType<LifeComponent>());

		gCoordinator.SetSystemSignature<MovementSystem>(signature);

		//Enemies.reserve(8);

			srand((unsigned int)time(0));
			bool quit = false;
			SDL_Event e;
			//While application is running
			while (!quit)
			{
				Uint64 StartLoopTick = SDL_GetPerformanceCounter();
				//SDL_Delay(300);
				//Handle events on queue
				if (SDL_PollEvent(&e) != 0)
				{
					//User requests quit
					if (e.type == SDL_QUIT)  
					{
						close();
						return 0;
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
				//CurrentRenderObject = 0;

				//Draw player
				SDL_Rect PositionToDraw = {PlayerX, PlayerY,  PlayerSize,  PlayerSize};
				//ColoursToDraw[0] = { 255, 0, 0, 255 };
				SDL_SetRenderDrawColor(gRenderer, 255, 0, 0, 255);
				SDL_RenderFillRect(gRenderer, &PositionToDraw);

				//shot go up
				if (ShotActive)
				{
					ShotY = ShotY - 6;
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
				WaveHasEnemies = false;
				// from this point using ECS
				if (WaveActive)
				{
					collisionSystem->Update();
					if (FramesSinceMoved > Movedelay)
					{
						movementSystem->Update();
						FramesSinceMoved = 0;
					}
					else
					{
						FramesSinceMoved++;
					}

					SDL_SetRenderDrawColor(gRenderer, 100, 255, 255, 255);
					int i = 0;
					for (auto& entity :Enemies)
					{
						auto& position = gCoordinator.GetComponent<PositionComponent>(entity);
						//sprintf_s(buffer, "Rendering Enemy %d ", EnemyPositions[i].x);
						//SDL_LogInfo(0, buffer);
						//SDL_RenderFillRect(gRenderer, &EnemyPositions[i]);
						if (position.alive)
						{
							SDL_LogInfo(0, "postion.alive entity %d i %d", entity, i);
							SDL_RenderFillRect(gRenderer, &EnemyPositions[entity]);
							WaveHasEnemies = true;
						}
						i++;
					}
				}
				if (!WaveHasEnemies) // breaking my normal pattern here
				{
					if (WaveActive)
					{
						for (auto& entity : Enemies)
						{
							gCoordinator.DestroyEntity(entity);
						}
					}
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
	return 0;
}

void spawnWave()
{
	if (WaveActive)
	{

	}
	else
	{
		int EnemyCount = 0;
		EnemyCount = 0;
		char buffer[50];
		if (CurrentWave < MaxWaves)
		{

				for (auto& entity : Enemies)
				{
					int xPosition = PlayerSize / 2 + EnemyCount * EnemySize * 3;
					SDL_LogInfo(0, "creating enemy, EnemyCount %d position %d",  EnemyCount, xPosition);
					entity = gCoordinator.CreateEntity();

					gCoordinator.AddComponent(
						entity,
						PositionComponent{ xPosition, 0 });

					gCoordinator.AddComponent(
						entity,
						SpeedComponent{ 1 });

					gCoordinator.AddComponent(
						entity,
						LifeComponent{ true });
					//AliveArray[EnemyCount] = true;
					EnemyCount++;
				}
				WaveActive = true;

		}
		else
		{
			sprintf_s(buffer, "You win!");
			SDL_LogInfo(0, buffer);
		}
		//WaveActive = true;
		CurrentWave = CurrentWave + 1;
	}
}

void MovementSystem::Update()
{
	int x = 0;
	int y = 0;
	int i = 0;
	for (auto& entity : Enemies)
	{
	
		auto& position = gCoordinator.GetComponent<PositionComponent>(entity);
		if (position.alive)
		{
			auto& speed = gCoordinator.GetComponent<SpeedComponent>(entity);
			position.y += speed.speed;
			//SDL_LogInfo(0, "Movement system updating, entity %p speed %d", entity, speed);
			x = position.x;
			y = position.y;
			EnemyPositions[entity] = { x, y, EnemySize, EnemySize };
			SDL_LogInfo(0, "MovementSystem::Update %d i %d", entity, i);
			i++;
		}
	}
}
void CollisionSystem::Update()
{
	for (auto& entity : Enemies)
	{
		auto& position = gCoordinator.GetComponent<PositionComponent>(entity);
		//SDL_LogWarn(0, "CollisionSystem::Update %d position.x %d position.y %d", entity, position.x, position.y);
		if (position.x < ShotX && ShotX < (position.x + EnemySize))
		{
			if (position.y < ShotY && ShotY < (position.y + EnemySize))
			{
				//SDL_LogWarn(0, "gCoordinator.DestroyEntity %d ", entity);
				//gCoordinator.DestroyEntity(entity);
				position.alive = false;
			}
		}
		else if (position.y == SCREEN_HEIGHT)
		{
			close();
		}
	}
}