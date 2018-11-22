#include <iostream>
#include <SFML/Graphics.hpp>
#define TINYC2_IMPL
#include <tinyc2.h>
#include <AnimatedSprite.h>
#include <GameObject.h>
#include <Player.h>
#include <NPC.h>
#include <Input.h>
#include <Debug.h>
#include "Capsule.h"

using namespace std;

int main()
{
	// Random seed 
	srand(time(nullptr));

	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");

	// States used to manage which object the player is usuing
	enum CurrentShape
	{
		Circle,
		AABB,
		Ray,
		CapsuleState
	};
	CurrentShape myPlayerShape = AABB;

	// Bools to keep track of information
	bool firstEntry = true;
	bool collided = false;

	// Shapes intialize ----------------------------------------------------------------------------------------------------------------------------
	// Player and Game recatangle-------------------------------------
	sf::RectangleShape playerAABB;
	playerAABB.setFillColor(sf::Color::Green);
	playerAABB.setPosition(100,100);
	playerAABB.setSize(sf::Vector2f{ 20, 20 });

	sf::RectangleShape npcAABB;
	npcAABB.setFillColor(sf::Color::Blue);
	npcAABB.setPosition(rand() % 600 + 100, rand() % 400 + 100);
	npcAABB.setSize(sf::Vector2f{ 100, 100 });

	//Setup NPC AABB
	c2AABB aabb_npc;
	aabb_npc.min = c2V(npcAABB.getPosition().x, npcAABB.getPosition().y);
	aabb_npc.max = c2V(
		npcAABB.getPosition().x + npcAABB.getGlobalBounds().width,
		npcAABB.getPosition().y + npcAABB.getGlobalBounds().height);

	//Setup Player AABB
	c2AABB aabb_player;
	aabb_player.min = c2V(playerAABB.getPosition().x, playerAABB.getPosition().y);
	aabb_player.max = c2V(playerAABB.getGlobalBounds().width, playerAABB.getGlobalBounds().width);

	// Player and Game Circle------------------------------------------
	c2Circle playerCircleC2;
	playerCircleC2.p = { -1000, -1000 };
	playerCircleC2.r = 30;
	sf::CircleShape playerCircle(playerCircleC2.r);
	playerCircle.setPosition(playerCircleC2.p.x, playerCircleC2.p.y);
	playerCircle.setFillColor(sf::Color::Green);
	playerCircle.setOrigin(playerCircleC2.r, playerCircleC2.r);

	c2Circle gameCircle;
	gameCircle.p = { rand() % 600 + 100 * 0.0f, rand() % 400 + 100 * 0.0f };
	gameCircle.r = 50;
	sf::CircleShape gameCircleShape(gameCircle.r);
	gameCircleShape.setPosition(gameCircle.p.x, gameCircle.p.y);
	gameCircleShape.setFillColor(sf::Color::Cyan);
	gameCircleShape.setOrigin(gameCircle.r, gameCircle.r);

	// Game Capsule -----------------------------------------------------
	c2Capsule gameCapsule;
	gameCapsule.a = { 300, 100 };
	gameCapsule.b = { 400, 100 };
	gameCapsule.r = 30;

	Capsule npcCapule(sf::Vector2f{ gameCapsule.a.x,gameCapsule.a.y }, sf::Vector2f{ gameCapsule.b.x,gameCapsule.b.y }, gameCapsule.r, sf::Color::Magenta);

	c2Capsule playerCapsule;
	playerCapsule.a = {  200, 100 };
	playerCapsule.b = {  250, 100 };
	playerCapsule.r = 30;

	Capsule playerCapShape(sf::Vector2f{ playerCapsule.a.x,playerCapsule.a.y }, sf::Vector2f{ playerCapsule.b.x,playerCapsule.b.y }, playerCapsule.r, sf::Color::Blue);

	// 2 Game Polygons ----------------------------------------------------------
	c2Poly gamePolyC2;
	gamePolyC2.count = 3;
	gamePolyC2.verts[0] = c2V(200, 150);
	gamePolyC2.verts[1] = c2V(350, 180);
	gamePolyC2.verts[2] = c2V(350, 250);
	c2Norms(gamePolyC2.verts, gamePolyC2.norms, gamePolyC2.count);
	
	sf::ConvexShape convexShapeTrianlge;
	convexShapeTrianlge.setFillColor(sf::Color::Black);
	convexShapeTrianlge.setOrigin(40, 40);
	convexShapeTrianlge.setPointCount(3);
	convexShapeTrianlge.setPosition(0, 0);
	convexShapeTrianlge.setOrigin(0, 0);

	c2Poly othergamePolyC2;
	othergamePolyC2.count = 3;
	othergamePolyC2.verts[0] = c2V(300, 250);
	othergamePolyC2.verts[1] = c2V(450, 280);
	othergamePolyC2.verts[2] = c2V(450, 350);
	c2Norms(othergamePolyC2.verts, othergamePolyC2.norms, othergamePolyC2.count);

	sf::ConvexShape otherconvexShapeTrianlge;
	otherconvexShapeTrianlge.setFillColor(sf::Color::Black);
	otherconvexShapeTrianlge.setPointCount(3);
	otherconvexShapeTrianlge.setPosition(0, 0);
	otherconvexShapeTrianlge.setOrigin(0, 0);
	
	// Player and Game Ray Cast ------------------------------------------
	c2Ray npcRay;
	npcRay.p = c2V(400, 500);
	npcRay.d = c2V(400, 500);
	npcRay.t = 10;

	sf::Vertex startOfRay;
	sf::Vertex endofRay;
	startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
	endofRay = sf::Vertex{ sf::Vector2f{npcRay.d.x, npcRay.d.y}, sf::Color::Green };
	npcRay.d = c2Norm(npcRay.d);
	sf::VertexArray vertexArray;
	vertexArray.setPrimitiveType(sf::Lines);

	// Collision result
	int result = 0;

	// Direction of movement of Objects
	sf::Vector2f direction(0.01f, 0.02f);
	sf::Vector2f circleMovement{ 0.02f, 0.01f };
	sf::Vector2f capMovement{ 0.01f, 0.03f };
	sf::Vector2f polyMovement{ 0.03f, 0.01f };
	sf::Vector2f otherpolyMovement{ 0.00f, 0.03f };
	
	// Start the game loop
	while (window.isOpen())
	{
		vertexArray.clear();
		if (myPlayerShape == AABB)
		{
			
			if (firstEntry == true)
			{
				// Moving other playerShapes off screen -------------------------------------
				playerCircleC2.p.x = -1000;
				playerCircleC2.p.y = -1000;
				npcRay.p = c2V(-400, -500);
				npcRay.d = c2V(-400, -500);
				playerCapShape.setPosition(sf::Vector2f{ -3000, -2000 });
				startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
				endofRay = sf::Vertex{ sf::Vector2f{npcRay.d.x, npcRay.d.y}, sf::Color::Green };

				// Reset --------------------------------------------------
				gamePolyC2.verts[0] = c2V(200, 150);
				gamePolyC2.verts[1] = c2V(350, 180);
				gamePolyC2.verts[2] = c2V(350, 250);
				gameCapsule.a = { 300, 100 };
				gameCapsule.b = { 400, 100 };
				npcAABB.setPosition(rand() % 600 + 100, rand() % 400 + 100);
				gameCircle.p.x = rand() % 600 + 100;
				gameCircle.p.y = rand() % 400 + 100;
				firstEntry = false;

			}
			playerAABB.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

			//  AABB to AABB collision check -------------------------------------------------------------
			result = c2AABBtoAABB(aabb_player, aabb_npc);
			cout << ((result != 0) ? ("AABB to AABB - Collision ") : "") << endl;
			if (result) 
			{
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Circle collision check -------------------------------------------------------------
			result = c2CircletoAABB(gameCircle, aabb_player);
			cout << ((result != 0) ? ("AABB to Circle - Collision") : "") << endl;
			if (result) 
			{
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Polygon Capsule check --------------------------------------------------------------
			result = c2AABBtoCapsule(aabb_player, gameCapsule);
			cout << ((result != 0) ? ("AABB to Capsule - Collision") : "") << endl;
			if (result) 
			{
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Polygon collision check ----------------------------------------------------------------
			result = c2AABBtoPoly(aabb_player, &gamePolyC2, NULL);
			cout << ((result != 0) ? ("AABB to Polygon - Collision") : "") << endl;
			if (result) 
			{
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			result = c2AABBtoPoly(aabb_player, &othergamePolyC2, NULL);
			cout << ((result != 0) ? ("AABB to Polygon - Collision") : "") << endl;
			if (result)
			{
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			// Input from Keyboard check ------------------------------------------------------------------------
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				myPlayerShape = Circle;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
			{
				myPlayerShape = Ray;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
			{
				myPlayerShape = CapsuleState;
				firstEntry = true;
			}
			collided = false;
		}
		else if (myPlayerShape == Circle)
		{
			if (firstEntry == true)
			{
				// Moving other playerShapes off screen -------------------------------------
				playerAABB.setPosition(-2000, -2000);
				npcRay.p = c2V(-400, -500);
				npcRay.d = c2V(-400, -500);
				startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
				endofRay = sf::Vertex{ sf::Vector2f{npcRay.d.x, npcRay.d.y}, sf::Color::Green };
				playerCapShape.setPosition(sf::Vector2f{ -3000, -2000 });

				// Reset---------------------------------------------
				gamePolyC2.verts[0] = c2V(200, 150);
				gamePolyC2.verts[1] = c2V(350, 180);
				gamePolyC2.verts[2] = c2V(350, 250);
				gameCapsule.a = { 300, 100 };
				gameCapsule.b = { 400, 100 };
				npcAABB.setPosition(rand() % 600 + 100, rand() % 400 + 100);
				gameCircle.p.x = rand() % 600 + 100;
				gameCircle.p.y = rand() % 400 + 100;
				firstEntry = false;
			}
			playerCircleC2.p.x = window.mapPixelToCoords(sf::Mouse::getPosition(window)).x;
			playerCircleC2.p.y = window.mapPixelToCoords(sf::Mouse::getPosition(window)).y;

			// Circle to Capsule ----------------------------------------------------------
			result = c2CircletoCapsule(playerCircleC2, gameCapsule);
			cout << ((result != 0) ? ("Circle to Capsule - Collision") : "") << endl;
			if (result) 
			{
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to Poly ----------------------------------------------------------------
			result = c2CircletoPoly(playerCircleC2, &gamePolyC2, NULL);
			cout << ((result != 0) ? ("Circle to Poly - Collision") : "") << endl;
			if (result) 
			{
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			result = c2CircletoPoly(playerCircleC2, &othergamePolyC2, NULL);
			cout << ((result != 0) ? ("Circle to Poly - Collision") : "") << endl;
			if (result)
			{
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to AABB ----------------------------------------------------------------
			result = c2CircletoAABB(playerCircleC2, aabb_npc);
			cout << ((result != 0) ? ("Circle to AABB - Collision") : "") << endl;
			if (result) 
			{
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to Circle ---------------------------------------------------------------
			result = c2CircletoCircle(playerCircleC2, gameCircle);
			cout << ((result != 0) ? ("Circle to Circle - Collision") : "") << endl;
			if (result) 
			{
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) 
			{
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Input from Keyboard Check ------------------------------------------------------
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
			{
				myPlayerShape = AABB;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
			{
				myPlayerShape = Ray;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
			{
				myPlayerShape = CapsuleState;
				firstEntry = true;
			}
			collided = false;
		}
		else if (myPlayerShape == Ray)
		{
			if (firstEntry == true)
			{ 
				// Moving other playerShapes off screen -------------------------------------
				playerAABB.setPosition(-2000, -2000);
				playerCircleC2.p.x = -1000;
				playerCircleC2.p.y = -1000;
				npcRay.p = c2V(400, 500);
				npcRay.d = c2V(400, 500);
				playerCapShape.setPosition(sf::Vector2f{ -3000, -2000 });
				
				// Reset-----------------------------------------------------------------------------
				gamePolyC2.verts[0] = c2V(200, 150);
				gamePolyC2.verts[1] = c2V(350, 180);
				gamePolyC2.verts[2] = c2V(350, 250);
				gameCapsule.a = { 300, 100 };
				gameCapsule.b = { 400, 100 };
				npcAABB.setPosition(rand() % 600 + 100, rand() % 400 + 100);
				gameCircle.p.x = rand() % 600 + 100;
				gameCircle.p.y = rand() % 400 + 100;
				firstEntry = false;
			}
			c2v mousePosition = { window.mapPixelToCoords(sf::Mouse::getPosition(window)).x, window.mapPixelToCoords(sf::Mouse::getPosition(window)).y };

			// Getting the length of the ray
			sf::Vector2f dist = { mousePosition.x - npcRay.p.x, mousePosition.y - npcRay.p.y };
			double len = std::sqrt(dist.x * dist.x + dist.y * dist.y);
			npcRay.t = len;

			npcRay.d.x = mousePosition.x - npcRay.p.x;
			npcRay.d.y = mousePosition.y - npcRay.p.y;
			endofRay = sf::Vertex{ sf::Vector2f{mousePosition.x, mousePosition.y}, sf::Color::Green };
			npcRay.d = c2Norm(npcRay.d);
			
			startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
			c2Raycast cast;

			// Ray to Objects Collisions ------------------------------------------------------------------------
			// Ray to AABB ---------------------------------------
			result = c2RaytoAABB(npcRay, aabb_npc, &cast);
			cout << ((result != 0) ? ("Ray to AABB - Collision") : "") << endl;
			if (result) {
				startOfRay.color =  sf::Color::Red ;
				endofRay.color = sf::Color::Red;
				collided = true;
			}
			else if (collided == false) 
			{
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}

			// Ray to Circle ---------------------------------------
			result = c2RaytoCircle(npcRay, gameCircle, &cast);
			cout << ((result != 0) ? ("Ray to Circle - Collision") : "") << endl;
			if (result) 
			{
				startOfRay.color = sf::Color::Red;
				endofRay.color = sf::Color::Red;
				collided = true;
			}
			else if (collided == false) 
			{
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}

			// Ray to Capsule ---------------------------------------
			result = c2RaytoCapsule(npcRay, gameCapsule, &cast);
			cout << ((result != 0) ? ("Ray to Capsule - Collision") : "") << endl;
			if (result) {
				startOfRay.color = sf::Color::Red;
				endofRay.color = sf::Color::Red;
				collided = true;
			}
			else if (collided == false) {
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}

			// Ray to Polygon ---------------------------------------
			result = c2RaytoPoly(npcRay, &gamePolyC2, NULL, &cast);
			cout << ((result != 0) ? ("Ray to Polygon - Collision") : "") << endl;
			if (result) {
				startOfRay.color = sf::Color::Red;
				endofRay.color = sf::Color::Red;
				collided = true;
			}
			else if (collided == false) {
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}

			result = c2RaytoPoly(npcRay, &othergamePolyC2, NULL, &cast);
			cout << ((result != 0) ? ("Ray to Polygon - Collision") : "") << endl;
			if (result) {
				startOfRay.color = sf::Color::Red;
				endofRay.color = sf::Color::Red;
				collided = true;
			}
			else if (collided == false) {
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}

			// Input from Keyboard Check ---------------------------------------
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				myPlayerShape = Circle;
				firstEntry = true;
			}

			if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
			{
				myPlayerShape = AABB;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::C))
			{
				myPlayerShape = CapsuleState;
				firstEntry = true;
			}
			collided = false;
		}
		else if (myPlayerShape == CapsuleState)
		{
			if (firstEntry == true)
			{
				// Moving other playerShapes off screen -------------------------------------
				playerAABB.setPosition(-2000, -2000);
				playerCircleC2.p.x = -1000;
				playerCircleC2.p.y = -1000;
				npcRay.p = c2V(-400, -500);
				npcRay.d = c2V(-400, -500);
				startOfRay = sf::Vertex{ sf::Vector2f{ npcRay.p.x, npcRay.p.y }, sf::Color::Green };
				endofRay = sf::Vertex{ sf::Vector2f{ npcRay.d.x, npcRay.d.y }, sf::Color::Green };

				// Reset-----------------------------------------------------------------------------
				gamePolyC2.verts[0] = c2V(200, 150);
				gamePolyC2.verts[1] = c2V(350, 180);
				gamePolyC2.verts[2] = c2V(350, 250);
				gameCapsule.a = { 300, 100 };
				gameCapsule.b = { 400, 100 };
				npcAABB.setPosition(rand() % 600 + 100, rand() % 400 + 100);
				gameCircle.p.x = rand() % 600 + 100;
				gameCircle.p.y = rand() % 400 + 100;
				firstEntry = false;
			}
			playerCapShape.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

			// Capsule to AABB ----------------------------------
			result = c2AABBtoCapsule(aabb_npc, playerCapsule);
			cout << ((result != 0) ? ("AABB to Capsule - Collision") : "") << endl;
			if (result)
			{
				playerCapShape.setColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCapShape.setColor(sf::Color(0, 255, 0));
			}

			// Capsule to Circle  -------------------------------
			result = c2CircletoCapsule(gameCircle, playerCapsule);
			cout << ((result != 0) ? ("Circle to Capsule - Collision") : "") << endl;
			if (result)
			{
				playerCapShape.setColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCapShape.setColor(sf::Color(0, 255, 0));
			}
			 // Capsule to Poly --------------------------------
			result = c2CapsuletoPoly(playerCapsule, &gamePolyC2, NULL);
			cout << ((result != 0) ? ("Poly to Capsule - Collision") : "") << endl;
			if (result)
			{
				playerCapShape.setColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCapShape.setColor(sf::Color(0, 255, 0));
			}

			result = c2CapsuletoPoly(playerCapsule, &othergamePolyC2, NULL);
			cout << ((result != 0) ? ("Poly to Capsule - Collision") : "") << endl;
			if (result)
			{
				playerCapShape.setColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCapShape.setColor(sf::Color(0, 255, 0));
			}

			// Capsule to Capsule --------------------------------
			result = c2CapsuletoCapsule(playerCapsule, gameCapsule);
			cout << ((result != 0) ? ("Capsule to Capsule - Collision") : "") << endl;
			if (result)
			{
				playerCapShape.setColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false)
			{
				playerCapShape.setColor(sf::Color(0, 255, 0));
			}

			// input from keyboard
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::B))
			{
				myPlayerShape = Circle;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
			{
				myPlayerShape = Ray;
				firstEntry = true;
			}
			if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
			{
				myPlayerShape = AABB;
				firstEntry = true;
			}
			collided = false;
		}






		// In game Shapes
		// Movement for each sape and update of the SF shapes

		// Circle----------------------------------------------------------------------------------------------------
		// Boundary
		if (gameCircle.p.x + gameCircleShape.getRadius() > 800)
		{
			circleMovement.x = -circleMovement.x;
		}
		if (gameCircle.p.x - gameCircleShape.getRadius() < 0)
		{
			circleMovement.x = -circleMovement.x;
		}
		if (gameCircle.p.y + gameCircleShape.getRadius() > 600)
		{
			circleMovement.y = -circleMovement.y;
		}
		if (gameCircle.p.y - gameCircleShape.getRadius() < 0)
		{
			circleMovement.y = -circleMovement.y;
		}

		// Movement
		gameCircle.p.x += 5*circleMovement.x;
		gameCircle.p.y += 5*circleMovement.y;
		// Assign
		gameCircleShape.setPosition(gameCircle.p.x, gameCircle.p.y);
		playerCircle.setPosition(playerCircleC2.p.x, playerCircleC2.p.y);

		// Capsule-----------------------------------------------------------------------------------------------------
		// Boundary
		if (gameCapsule.b.x + npcCapule.getRadius() > 800)
		{
			capMovement.x = -capMovement.x;
		}
		if (gameCapsule.a.x - npcCapule.getRadius() < 0)
		{
			capMovement.x = -capMovement.x;
		}
		if (gameCapsule.a.y + npcCapule.getRadius() > 600)
		{
			capMovement.y = -capMovement.y;
		}
		if (gameCapsule.a.y - npcCapule.getRadius() < 0)
		{
			capMovement.y = -capMovement.y;
		}
		// Movement
		gameCapsule.a.x += 5 * capMovement.x;
		gameCapsule.a.y += 5 * capMovement.y;
		gameCapsule.b.x += 5 * capMovement.x;
		gameCapsule.b.y += 5 * capMovement.y;

		playerCapsule.a.x = playerCapShape.getPosition().x;
		playerCapsule.a.y = playerCapShape.getPosition().y;
		
		playerCapsule.b.x = playerCapShape.getPositionEnd().x;
		playerCapsule.b.y = playerCapShape.getPositionEnd().y;
		
		
		// Assignment
		npcCapule.setPosition(sf::Vector2f{ gameCapsule.a.x, gameCapsule.a.y });
		playerCapShape.setPosition(sf::Vector2f{ playerCapsule.a.x, playerCapsule.a.y });
		// Polygon----------------------------------------------------------------------------------------------------------
		// Boundary Check
		if (gamePolyC2.verts[1].x > 800)
		{
			polyMovement.x = -polyMovement.x;
		}
		if (gamePolyC2.verts[0].x < 0)
		{
			polyMovement.x = -polyMovement.x;
		}
		if (gamePolyC2.verts[2].y > 600)
		{
			polyMovement.y = -polyMovement.y;
		}
		if (gamePolyC2.verts[0].y < 0)
		{
			polyMovement.y = -polyMovement.y;
		}

		// Movement
		gamePolyC2.verts[0] = c2V(gamePolyC2.verts[0].x + 5 * polyMovement.x, gamePolyC2.verts[0].y + 5 * polyMovement.y);
		gamePolyC2.verts[1] = c2V(gamePolyC2.verts[1].x + 5 * polyMovement.x, gamePolyC2.verts[1].y + 5 * polyMovement.y);
		gamePolyC2.verts[2] = c2V(gamePolyC2.verts[2].x + 5 * polyMovement.x, gamePolyC2.verts[2].y + 5 * polyMovement.y);
		// Assignment
		convexShapeTrianlge.setPoint(0, sf::Vector2f{ gamePolyC2.verts[0].x, gamePolyC2.verts[0].y });
		convexShapeTrianlge.setPoint(1, sf::Vector2f{ gamePolyC2.verts[1].x, gamePolyC2.verts[1].y });
		convexShapeTrianlge.setPoint(2, sf::Vector2f{ gamePolyC2.verts[2].x, gamePolyC2.verts[2].y });


		// Other game Poly ------------------------------------------------------------
		if (othergamePolyC2.verts[1].x > 800)
		{
			otherpolyMovement.x = -otherpolyMovement.x;
		}
		if (othergamePolyC2.verts[0].x < 0)
		{
			otherpolyMovement.x = -otherpolyMovement.x;
		}
		if (othergamePolyC2.verts[2].y > 600)
		{
			otherpolyMovement.y = -otherpolyMovement.y;
		}
		if (othergamePolyC2.verts[0].y < 0)
		{
			otherpolyMovement.y = -otherpolyMovement.y;
		}

		// Movement
		othergamePolyC2.verts[0] = c2V(othergamePolyC2.verts[0].x + 5 * otherpolyMovement.x, othergamePolyC2.verts[0].y + 5 * otherpolyMovement.y);
		othergamePolyC2.verts[1] = c2V(othergamePolyC2.verts[1].x + 5 * otherpolyMovement.x, othergamePolyC2.verts[1].y + 5 * otherpolyMovement.y);
		othergamePolyC2.verts[2] = c2V(othergamePolyC2.verts[2].x + 5 * otherpolyMovement.x, othergamePolyC2.verts[2].y + 5 * otherpolyMovement.y);
		// Assignment
		otherconvexShapeTrianlge.setPoint(0, sf::Vector2f{ othergamePolyC2.verts[0].x, othergamePolyC2.verts[0].y });
		otherconvexShapeTrianlge.setPoint(1, sf::Vector2f{ othergamePolyC2.verts[1].x, othergamePolyC2.verts[1].y });
		otherconvexShapeTrianlge.setPoint(2, sf::Vector2f{ othergamePolyC2.verts[2].x, othergamePolyC2.verts[2].y });

		// Poly to Poly collision --------------------------------------
		result = c2PolytoPoly(&gamePolyC2, NULL, &othergamePolyC2, NULL);
		cout << ((result != 0) ? ("Poly to Polygon - Collision") : "") << endl;
		if (result) {
			convexShapeTrianlge.setFillColor(sf::Color::Yellow);
			otherconvexShapeTrianlge.setFillColor(sf::Color::Yellow);
			collided = true;
		}
		else if (collided == false) {
			convexShapeTrianlge.setFillColor(sf::Color::Black);
			otherconvexShapeTrianlge.setFillColor(sf::Color::Black);
		}

		// Move The NPC AABB--------------------------------------------------------------------------------------------
		sf::Vector2f move_to(npcAABB.getPosition().x + 5*direction.x, npcAABB.getPosition().y + 5*direction.y);
		if (move_to.x < 0) {
			direction.x *= -1;
			move_to.x = 0; 
		}
		else if (move_to.x + npcAABB.getGlobalBounds().width >= 800) {
			direction.x *= -1;
			move_to.x = 800 - npcAABB.getGlobalBounds().width;
		}
		else if (move_to.y < 0) {
			direction.y *= -1;
			move_to.y = 0; 
		}
		else if (move_to.y + npcAABB.getGlobalBounds().height >= 600) {
			direction.y *= -1;
			move_to.y = 600 - npcAABB.getGlobalBounds().height;
		}
		npcAABB.setPosition(move_to);

		// Update NPC AABB set x and y
		aabb_npc.min = c2V(npcAABB.getPosition().x, npcAABB.getPosition().y);
		aabb_npc.max = c2V(npcAABB.getPosition().x + npcAABB.getGlobalBounds().width, npcAABB.getPosition().y + npcAABB.getGlobalBounds().height);

		// Update Player AABB
		aabb_player.min = c2V(playerAABB.getPosition().x, playerAABB.getPosition().y);
		aabb_player.max = c2V(playerAABB.getPosition().x + playerAABB.getGlobalBounds().width, playerAABB.getPosition().y + playerAABB.getGlobalBounds().height);

		// Process events
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
			case sf::Event::Closed:
				// Close window : exit
				window.close();
				break;
			case sf::Event::KeyPressed:

			default:
				break;
			}
		}
		
		vertexArray.append(startOfRay);												// Joining vertexes
		vertexArray.append(endofRay);

		// Clear screen
		window.clear(sf::Color::White);

		// Draw Objects
		window.draw(gameCircleShape);
		npcCapule.render(window);
		window.draw(npcAABB);
		window.draw(convexShapeTrianlge);
		window.draw(otherconvexShapeTrianlge);
		window.draw(playerCircle);
		window.draw(playerAABB);
		window.draw(vertexArray);
		playerCapShape.render(window);
		// Update the window
		window.display();
	}

	return EXIT_SUCCESS;
};



