## Instructions:
Player moves with left and right arrow, 
shoots with up arrow,
Spawns wave of enemies with down arrow

SDL-Spacegame-EmilS.zip contains standalone .exe and needed .dll
# Data oriented Design

The development of this game can be split in to three parts;
First I had to learn and experiment with SDL, starting out with a experimental build of SDL3 before moving to SDL2 stable.
Then I studied Data oriented Design and experimented with my own data handling system that was not particularily efficient.
Finally I looked into the ECS pattern, created a ECS (largely inspired by https://austinmorlan.com/posts/entity_component_system/.) and moved my enemies there.

In order to comply with Data oriented design the enemies in the game use a Entity Component System.
The ECS pattern stores values in memory without leaving gaps, greatly reducing cache misses.

The Entity in this case is a individual enemy, comprising of just a unique ID.

The Components are in this case the data used to describe a unique enemy, here they are all structs.
```
struct PositionComponent
{
	int x = 0;
	int y = 0;
	bool alive = true; // moved alive check here in order to reduce needed components
};

struct SpeedComponent
{
	int speed = 0;
};

struct LifeComponent // not used
{
	bool alive = false;
};
```

The systems used in this game handle movement and collision, systems are the "active" parts of an ECS structure.
```
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
			x = position.x;
			y = position.y;
			EnemyPositions[entity] = { x, y, EnemySize, EnemySize };
			i++;
		}
	}
}

void CollisionSystem::Update()
{
	for (auto& entity : Enemies)
	{
		auto& position = gCoordinator.GetComponent<PositionComponent>(entity);
		if (position.x < ShotX && ShotX < (position.x + EnemySize))
		{
			if (position.y < ShotY && ShotY < (position.y + EnemySize))
			{
				position.alive = false;
			}
		}
		else if (position.y == SCREEN_HEIGHT)
		{
			close(); // ends game
		}
	}
}
```
