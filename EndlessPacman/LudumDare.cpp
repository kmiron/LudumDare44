// LudumDare.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "Components.h"
#include "GameConst.h"
#include "Game.h"
#include "Systems.h"

std::shared_ptr<Entity> g_singletonEntity;
Game g_game;

void RegisterSystems() 
{	
	g_game.RegisterSystem(std::make_shared<EntityRenderSystem>());
	g_game.RegisterSystem(std::make_shared<CombatSystem>());

	// At end to clear inputs
	g_game.RegisterSystem(std::make_shared<InputSystem>());

	// Cleanup 
	g_game.RegisterSystem(std::make_shared<EntityCleanupSystem>());
}

void CreatePlayer()
{
	std::shared_ptr<Entity> player = g_game.CreateEntity();
	std::shared_ptr<HealthComponent> healthCmp = player->AddComponent<HealthComponent>();
	std::shared_ptr<PositionComponent> positionCmp = player->AddComponent<PositionComponent>();
	std::shared_ptr<LocalPlayerComponent> localPlayerCmp = player->AddComponent<LocalPlayerComponent>();
	teREF(localPlayerCmp);

	healthCmp->m_health = 600;
	healthCmp->m_maxHealth = 600;
	positionCmp->x = 640;
	positionCmp->y = 360;
}

void CreateInjuredPlayers(std::vector<std::shared_ptr<Entity>>& injuredList)
{
	static const int numInjured = 8;
	for (int i = 0; i < numInjured; i++)
	{
		std::shared_ptr<Entity> injuredPlayer = g_game.CreateEntity();
		std::shared_ptr<HealthComponent> healthCmp = injuredPlayer->AddComponent<HealthComponent>();
		std::shared_ptr<PositionComponent> positionCmp = injuredPlayer->AddComponent<PositionComponent>();
		
		healthCmp->m_health = rand() % 40 + 10;
		healthCmp->m_maxHealth = 100;
		positionCmp->x = (float)(1280 / numInjured * i + (640 / numInjured) - 60);
		positionCmp->y = 160;
		positionCmp->index = i;

		injuredList.push_back(injuredPlayer);
	}

}
int main()
{
	RegisterSystems();

	g_singletonEntity = g_game.CreateEntity();
	g_singletonEntity->AddComponent<GlobalFontComponent>();
	g_singletonEntity->AddComponent<GlobalInputComponent>();

	std::vector<std::shared_ptr<Entity>> enemies;

	CreatePlayer();
	CreateInjuredPlayers(enemies);
	

	sf::RenderWindow window(sf::VideoMode(1280, 720), "Ludum Dare - 44");	
	sf::Text healthText;
	float dt = 0.0f;

	sf::Clock clock;
	while (window.isOpen())
	{
		dt = clock.restart().asSeconds();
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				window.close();
			if (event.type == sf::Event::KeyReleased)
			{
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_releasedKeys.insert(event.key.code);
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_heldKeys.erase(event.key.code);
			}
			if (event.type == sf::Event::KeyPressed)
			{
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_heldKeys.insert(event.key.code);
				g_singletonEntity->GetComponent<GlobalInputComponent>()->m_pressedKeys.insert(event.key.code);
			}
		}
				
		for (std::vector<std::shared_ptr<System>>::iterator it = g_game.m_systems.begin(); it != g_game.m_systems.end(); it++)
		{
			(*it)->Update(dt);
		}		
		
		// Do Render
		window.clear();
		for (std::vector<std::shared_ptr<System>>::iterator it = g_game.m_systems.begin(); it != g_game.m_systems.end(); it++)
		{
			(*it)->Draw(dt, window);
		}

		window.display();		
	}

	g_singletonEntity->Destroy();

	return 0;
}