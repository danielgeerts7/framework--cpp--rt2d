/**
 * This file is part of a demo that shows how to use RT2D, a 2D OpenGL framework.
 * 
 * - Copyright 2015 Rik Teerling <rik@onandoffables.com>
 *     - Initial commit
 * - Copyright 2015 Your Name <you@yourhost.com>
 *     - What you did
 */

#ifndef SCENE00_H
#define SCENE00_H

#include <vector>
#include <rt2d/timer.h>
#include <rt2d/player.h>
#include "superscene.h"
#include "basicentity.h"

class Scene00: public SuperScene
{
	public:
		Scene00();
		virtual ~Scene00();

		virtual void update(float deltaTime);
	
	private:
		BasicEntity* background_entity;
		BasicEntity* player_entity;
		//BasicEntity* gun_enemie_entity;
		BasicEntity* gun_player_entity;

		Timer t;
		int delay;
		int counter;

		std::vector<BasicEntity*> enemies;
		std::vector<BasicEntity*> guns_enemies;
		std::vector<BasicEntity*> blocks;
		std::vector<BasicEntity*> player_bullets;

		void PlayerStayInScreen();
};

#endif /* SCENE00_H */ 
