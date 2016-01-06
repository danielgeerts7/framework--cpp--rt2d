/**
* - Copyright 2015 Daniel Geerts <daniel.geerts@live.nl>
*     - Initial commit
*/

#include "scene00.h"

Scene00::Scene00() : SuperScene()
{
	// ###############################################################
	// Setting the variables
	// ###############################################################

	// Start Timer t
	t.start();

	mouseClickBulletDelay = 10;
	mouseClickBulletCounter = 0;

	playerReloadDelay = 100;
	playerReloadCounter = 0;

	enemieDelay = 10;
	enemieCounter = 0;

	pressedReloadingForThePlayer = false;
	playerCanShoot = false;

	playerRaduis = 24;

	maxAmmoInMagazine = 32;
	currentAmmoInMagazine = maxAmmoInMagazine;

	currentAmmoInBag = maxAmmoInMagazine * 2;

	text[0]->message("Scene00: shoot the enemies before they shoot you!");

	background_entity = new LoadTiles();
	layers[0]->addChild(background_entity);

	player_entity = new Player();
	player_entity->position = Point2(800, 350);
	player_entity->addSprite("assets/player.tga");
	player_entity->sprite()->color = GREEN;

	for (int e = 0; e < 5; e++) {
		Enemie* enemie = new Enemie();
		enemies.push_back(enemie);

		layers[2]->addChild(enemies[e]);

		BasicEntity* gun_enemie = new BasicEntity();
		gun_enemie->addSprite("assets/gun.tga");
		gun_enemie->sprite()->color = YELLOW;
		gun_enemie->position = Point2(30, 25);

		guns_enemies.push_back(gun_enemie);

		enemies[e]->addChild(gun_enemie);
	}
	
	enemies[0]->position = Point(100, 100);
	enemies[1]->position = Point(700, 600);
	enemies[2]->position = Point(750, 100);
	enemies[3]->position = Point(400, 400);
	enemies[4]->position = Point(200, 650);

	gun_player_entity = new BasicEntity();
	gun_player_entity->addSprite("assets/gun.tga");
	gun_player_entity->sprite()->color = YELLOW;
	gun_player_entity->position = Point2(30, 25);
	
	int amount = 10;

	for (int i = 0; i<amount; i++) {
		BasicEntity* b = new BasicEntity();
		b->addSprite("assets/block.tga");

		b->sprite()->color = BLUE;
		b->position.x = 250;
		b->position.y = rand() % 300 + 150;
		blocks.push_back(b);

		layers[2]->addChild(blocks[i]);
	}

	layers[2]->addChild(player_entity);
	player_entity->addChild(gun_player_entity);
}


Scene00::~Scene00()
{

	int s = blocks.size();
	for (int i = 0; i<s; i++) {
		layers[2]->removeChild(blocks[i]);
		delete blocks[i];
	}
	blocks.clear();

	int p_b = player_bullets.size();
	for (int j = 0; j<p_b; j++) {
		layers[1]->removeChild(player_bullets[j]);
		delete player_bullets[j];
	}
	player_bullets.clear();

	int e_b = enemies_bullets.size();
	for (int l = 0; l<e_b; l++) {
		layers[1]->removeChild(enemies_bullets[l]);
		delete enemies_bullets[l];
	}
	enemies_bullets.clear();

	int es = enemies.size();
	for (int k = 0; k < es; k++) {
		enemies[k]->removeChild(guns_enemies[k]);
		layers[1]->removeChild(enemies[k]);
		delete guns_enemies[k];
		delete enemies[k];
	}
	enemies.clear();

	int pp = particles.size();
	for (int a = 0; a < pp; a++) {
		layers[3]->removeChild(particles[a]);
		delete particles[a];
	}
	particles.clear();

	layers[0]->removeChild(background_entity);
	layers[2]->removeChild(player_entity);
	player_entity->removeChild(gun_player_entity);
	delete background_entity;
	delete player_entity;
	delete gun_player_entity;
}

void Scene00::update(float deltaTime)
{
	// ###############################################################
	// Make SuperScene do what it needs to do (Escape key stops Scene)
	// ###############################################################
	SuperScene::update(deltaTime);

	// ###############################################################
	// Moving the player_entity
	// ###############################################################
	float speed = 150.0f; // 150 units / second

	Vector2 playerRight = Vector2(1, 0);
	Vector2 playerUp = Vector2(0, 1);

	// Move up
	if (input()->getKey(GLFW_KEY_W)) {
		player_entity->position -= playerUp * deltaTime * speed;
	}
	// Move down
	if (input()->getKey(GLFW_KEY_S)) {
		player_entity->position += playerUp * deltaTime * speed;
	}
	// move left
	if (input()->getKey(GLFW_KEY_A)) {
		player_entity->position -= playerRight * deltaTime * speed;
	}
	// move right
	if (input()->getKey(GLFW_KEY_D)) {
		player_entity->position += playerRight * deltaTime * speed;
	}

	// ###############################################################
	// Camera follow the player_entity
	// ###############################################################
	camera()->position.x = player_entity->position.x;
	camera()->position.y = player_entity->position.y;

	// ###############################################################
	// Rotate player_entity to the position of the mouse
	// ###############################################################
	int mousex = input()->getMouseX() + camera()->position.x - SWIDTH / 2;
	int mousey = input()->getMouseY() + camera()->position.y - SHEIGHT / 2;
	Point2 mousepos = Point2(mousex, mousey);
	Vector2 delta = Vector2(player_entity->position, mousepos);
	float angle = delta.getAngle();
	player_entity->rotation = angle;

	// ###############################################################
	// Updating the enemieCounter and checking if the enemieCounter is greater then the enemieDelay
	// ###############################################################
	enemieCounter += 0.5;
	if (enemieCounter >= enemieDelay) {
		enemieCounter = enemieDelay;
	}

	// ###############################################################
	// Giving the new Bullet(s)* b the rotation of the enemies[a]
	// ###############################################################
	for (int a = 0; a < enemies.size(); a++) {
		enemies[a]->checkForPlayerIfWalkingInFieldOfView(player_entity);
		if (enemies[a]->checkIfPlayerIsInFieldOfView && enemieCounter >= enemieDelay) {
			Bullet* b = new Bullet();
			b->setPositionAndRotation(enemies[a]);
			layers[1]->addChild(b);
			enemies_bullets.push_back(b);

			enemieCounter = 0;
		}
	}

	// ###############################################################
	// Player gets hit by the enemies bullets
	// ###############################################################
	std::vector<BasicEntity*>::iterator toremoveEB = enemies_bullets.begin();
	while (toremoveEB != enemies_bullets.end()) {
		if (player_entity->gettingHitByEnemieBullets((*toremoveEB)) == 1 && player_entity->getPlayerHealth() > 0 && (*toremoveEB) > 0) {
			layers[1]->removeChild((*toremoveEB));
			delete (*toremoveEB);
			toremoveEB = enemies_bullets.erase(toremoveEB);
		}
		else {
			++toremoveEB;
		}
	}

	// ###############################################################
	// Enemies gets hit by the player bullets
	// ###############################################################
	std::vector<BasicEntity*>::iterator toremovePB = player_bullets.begin();
	while (toremovePB != player_bullets.end()) {
		for (int i = 0; enemies.size(); i++) {

			bool temp = enemies[i]->gettingHitByPlayerBullets((*toremovePB)) == 1;

			if (temp && enemies[i]->getEnemieHealth() > 0 && (*toremovePB) > 0) {
				layers[1]->removeChild((*toremovePB));
				delete (*toremovePB);
				toremovePB = player_bullets.erase(toremovePB);

				/*
				p = new ParticleSystem();
				p->addParticleToParent((*enemiesIT), *toremovePB);

				layers[3]->addChild(p);
				particles.push_back(p);
				*/
			}
			else {
				++toremovePB;
			}
		}
	}

	// ###############################################################
	// Deleting particles when isDead() is true
	// ###############################################################
	std::vector<ParticleSystem*>::iterator toremovePart = particles.begin();
	while (toremovePart != particles.end()) {
		if ((*toremovePart)->isDead()) {
			layers[3]->removeChild((*toremovePart));
			delete (*toremovePart);
			toremovePart = particles.erase(toremovePart);
		} else {
			++toremovePart;
		}
	}

	std::string pp = "particles: ";
	pp.append(std::to_string(particles.size()));
	text[12]->message(pp);

	// ###############################################################
	// Checking if the enemie_bullets go out of the stage, then remove them
	// ###############################################################
	std::vector<BasicEntity*>::iterator toremoveEB2 = enemies_bullets.begin();
	while (toremoveEB2 != enemies_bullets.end()) {
		if ((((*toremoveEB2)->position.x < SWIDTH - SWIDTH) ||
			((*toremoveEB2)->position.x > SWIDTH) ||
			((*toremoveEB2)->position.y < SHEIGHT - SHEIGHT) ||
			((*toremoveEB2)->position.y > SHEIGHT)) &&
			(*toremoveEB2) > 0) {

			layers[1]->removeChild((*toremoveEB2));
			delete (*toremoveEB2);
			toremoveEB2 = enemies_bullets.erase(toremoveEB2);
		} else {
			++toremoveEB2;
		}
	}

	// ###############################################################
	// Checking if the player_bullets go out of the stage, then remove them
	// ###############################################################
	std::vector<BasicEntity*>::iterator toremove2 = player_bullets.begin();
	if (player_bullets.size() > 0) {
		while (toremove2 != player_bullets.end()) {
			if ((*toremove2)->position.x < SWIDTH - SWIDTH ||
				(*toremove2)->position.x > SWIDTH ||
				(*toremove2)->position.y < SHEIGHT - SHEIGHT ||
				(*toremove2)->position.y > SHEIGHT) {

				layers[1]->removeChild((*toremove2));
				delete (*toremove2);
				toremove2 = player_bullets.erase(toremove2);
			} else {
				++toremove2;
			}
		}
	}

	// ###############################################################
	// Calling the playerCollidWithBlock function to every block that is on stage
	// But that function doens't do anything yet
	// ###############################################################
	for (int i = 0; i < blocks.size(); i++) {
		player_entity->playerCollidWithBlock(player_entity, blocks[i], 32, 27);
	}
	
	// ###############################################################
	// Updating mouseClickBulletCounter and checking if the mouseClickBulletCounter is greater then the mouseClickBulletDelay
	// ###############################################################
	mouseClickBulletCounter += 0.5;
	if (mouseClickBulletCounter >= mouseClickBulletDelay) {
		mouseClickBulletCounter = mouseClickBulletDelay;
	}
	
	// ###############################################################
	// Creating Bullet* on the position of the player_entity when left mouse button is clicked
	// ###############################################################
	if (input()->getMouse(GLFW_MOUSE_BUTTON_1) && mouseClickBulletCounter >= mouseClickBulletDelay && currentAmmoInMagazine > 0 && playerCanShoot) {
		Bullet* b = new Bullet();
		b->setPositionAndRotation(player_entity);
		layers[1]->addChild(b);
		player_bullets.push_back(b);

		mouseClickBulletCounter = 0;
		currentAmmoInMagazine--;
		playerCanShoot = false;
	}

	// ###############################################################
	// When R pressed and currentAmmoInBag is greater then 0
	// Then, pressedReloadingForThePlayer is true
	// ###############################################################
	if (input()->getKeyDown(GLFW_KEY_R) && currentAmmoInBag > 0) {
		pressedReloadingForThePlayer = true;
	}
	
	// ###############################################################
	// When pressedReloadingForThePlayer is true
	// playerCanShoot is false, because in the reloading time the player can't shoot
	// Then the counter gets activated and while the counter is going that is the time that the player can shoot
	// Then the player reloads and after that he can shoot again.
	// ###############################################################
	if (pressedReloadingForThePlayer) {
		playerCanShoot = false;
		playerReloadCounter += 0.5;
		if (playerReloadCounter >= playerReloadDelay) {
			playerReloadCounter = playerReloadDelay;

			int checkIfCanCarry = currentAmmoInMagazine + currentAmmoInBag;

			if (checkIfCanCarry > 32) {
				currentAmmoInBag -= maxAmmoInMagazine - currentAmmoInMagazine;
				currentAmmoInMagazine = maxAmmoInMagazine;
			}
			else if (checkIfCanCarry > 0) {
				currentAmmoInMagazine += currentAmmoInBag;
				currentAmmoInBag -= currentAmmoInBag;
			}
			playerReloadCounter = 0;
			pressedReloadingForThePlayer = false;
		}
	} else if (!pressedReloadingForThePlayer) {
		playerCanShoot = true;
	}

	// ###############################################################
	// When the ammo in magazine equals or less than 0 then it stays 0
	// ###############################################################
	if (currentAmmoInMagazine <= 0) {
		currentAmmoInMagazine = 0;
	}
	// ###############################################################
	// When the ammo in youre bag equals or less than 0 then it stays 0
	// ###############################################################
	if (currentAmmoInBag <= 0) {
		currentAmmoInBag = 0;
	}
	// ###############################################################
	// When the ammo in magazine equals to 0 and the ammo in your bag equals to 0
	// Then you get a message that youre out of ammo,
	// ###############################################################
	if (currentAmmoInMagazine == 0 && currentAmmoInBag == 0) {
		text[6]->message("You're out of ammo, find a ammunition box to continuing shooting");
	}

	// ###############################################################
	// Logging current ammo and current mags
	// ###############################################################
	std::string AmmoLeftToUseText = "Ammo: ";
	AmmoLeftToUseText.append(std::to_string(currentAmmoInMagazine));
	AmmoLeftToUseText.append("/");
	AmmoLeftToUseText.append(std::to_string(currentAmmoInBag));
	text[4]->message(AmmoLeftToUseText);

	// ###############################################################
	// Logging current health and max health
	// ###############################################################
	std::string playerHealthToText = "Health: ";
	playerHealthToText.append(std::to_string(player_entity->getPlayerHealth()));
	playerHealthToText.append("/");
	playerHealthToText.append("100");
	text[5]->message(playerHealthToText);

	// ###############################################################
	// Setting the borders for the player_entity
	// ###############################################################
	if (player_entity->position.x < 0 + playerRaduis) {
		player_entity->position.x = 0 + playerRaduis;
	}
	if (player_entity->position.x > SWIDTH - playerRaduis) {
		player_entity->position.x = SWIDTH - playerRaduis;
	}
	if (player_entity->position.y < 0 + playerRaduis) {
		player_entity->position.y = 0 + playerRaduis;
	}
	if (player_entity->position.y > SHEIGHT - playerRaduis) {
		player_entity->position.y = SHEIGHT - playerRaduis;
	}
}
