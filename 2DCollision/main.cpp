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
	// Create the main window
	sf::RenderWindow window(sf::VideoMode(800, 600), "SFML window");
	enum CurrentShape
	{
		Circle,
		AABB,
		Ray
	};
	CurrentShape myPlayerShape = AABB;
	bool firstEntry = true;
	bool collided = false;
	sf::RectangleShape playerAABB;
	playerAABB.setFillColor(sf::Color::Green);
	playerAABB.setPosition(100,100);
	playerAABB.setSize(sf::Vector2f{ 20, 20 });

	sf::RectangleShape npc;
	npc.setFillColor(sf::Color::Blue);
	npc.setPosition(0, 0);
	npc.setSize(sf::Vector2f{ 100, 100 });

	//Setup NPC AABB
	c2AABB aabb_npc;
	aabb_npc.min = c2V(npc.getPosition().x, npc.getPosition().y);
	aabb_npc.max = c2V(
		npc.getPosition().x + npc.getGlobalBounds().width,
		npc.getPosition().y + npc.getGlobalBounds().height);

	//Setup Player AABB
	c2AABB aabb_player;
	aabb_player.min = c2V(playerAABB.getPosition().x, playerAABB.getPosition().y);
	aabb_player.max = c2V(playerAABB.getGlobalBounds().width, playerAABB.getGlobalBounds().width);


	c2Circle playerCircleC2;
	playerCircleC2.p = { -1000, -1000 };
	playerCircleC2.r = 30;
	sf::CircleShape playerCircle(playerCircleC2.r);
	playerCircle.setPosition(playerCircleC2.p.x, playerCircleC2.p.y);
	playerCircle.setFillColor(sf::Color::Green);
	playerCircle.setOrigin(playerCircleC2.r, playerCircleC2.r);

	c2Circle testCircle;
	testCircle.p = { 200, 200 };
	testCircle.r = 50;
	sf::CircleShape maCircle(testCircle.r);
	maCircle.setPosition(testCircle.p.x, testCircle.p.y);
	maCircle.setFillColor(sf::Color::Blue);
	maCircle.setOrigin(testCircle.r, testCircle.r);


	c2Capsule tinyCapsule;
	tinyCapsule.a = { 300, 100 };
	tinyCapsule.b = { 400, 100 };
	tinyCapsule.r = 30;

	Capsule npcCapule(sf::Vector2f{ tinyCapsule.a.x,tinyCapsule.a.y }, sf::Vector2f{ tinyCapsule.b.x,tinyCapsule.b.y }, tinyCapsule.r, sf::Color::Blue);

	c2Poly myPol;
	myPol.count = 3;
	myPol.verts[0] = c2V(200, 150);
	myPol.verts[1] = c2V(350, 180);
	myPol.verts[2] = c2V(350, 250);

	sf::ConvexShape tri;
	tri.setFillColor(sf::Color::Blue);
	tri.setOrigin(40, 40);
	tri.setPointCount(3);
	tri.setPosition(0, 0);
	tri.setOrigin(0, 0);
	

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

	// Direction of movement of NPC
	sf::Vector2f direction(0.01f, 0.02f);
	
	// Start the game loop
	while (window.isOpen())
	{
		vertexArray.clear();
		if (myPlayerShape == AABB)
		{
			
			if (firstEntry == true)
			{
				playerCircleC2.p.x = -1000;
				playerCircleC2.p.y = -1000;
				npcRay.p = c2V(-400, -500);
				npcRay.d = c2V(-400, -500);
				startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
				endofRay = sf::Vertex{ sf::Vector2f{npcRay.d.x, npcRay.d.y}, sf::Color::Green };
				// Reset
				myPol.verts[0] = c2V(200, 150);
				myPol.verts[1] = c2V(350, 180);
				myPol.verts[2] = c2V(350, 250);
				tinyCapsule.a = { 300, 100 };
				tinyCapsule.b = { 400, 100 };
				npc.setPosition(0, 0);
				testCircle.p.x = 200;
				testCircle.p.y = 200;
				firstEntry = false;

			}
			playerAABB.setPosition(window.mapPixelToCoords(sf::Mouse::getPosition(window)));

			//  AABB to AABB collision check 
			result = c2AABBtoAABB(aabb_player, aabb_npc);
			cout << ((result != 0) ? ("AABB to AABB - Collision ") : "") << endl;
			if (result) {
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Circle collision check 
			result = c2CircletoAABB(testCircle, aabb_player);
			cout << ((result != 0) ? ("AABB to Circle - Collision") : "") << endl;
			if (result) {
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Polygon Capsule check 
			result = c2AABBtoCapsule(aabb_player, tinyCapsule);
			cout << ((result != 0) ? ("AABB to Capsule - Collision") : "") << endl;
			if (result) {
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}

			//  AABB to Polygon collision check 
			result = c2AABBtoPoly(aabb_player, &myPol, NULL);
			cout << ((result != 0) ? ("AABB to Polygon - Collision") : "") << endl;
			if (result) {
				playerAABB.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerAABB.setFillColor(sf::Color(0, 255, 0));
			}


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
			collided = false;
		}
		else if (myPlayerShape == Circle)
		{
			if (firstEntry == true)
			{
				playerAABB.setPosition(-2000, -2000);
				npcRay.p = c2V(-400, -500);
				npcRay.d = c2V(-400, -500);
				startOfRay = sf::Vertex{ sf::Vector2f{npcRay.p.x, npcRay.p.y}, sf::Color::Green };
				endofRay = sf::Vertex{ sf::Vector2f{npcRay.d.x, npcRay.d.y}, sf::Color::Green };
				// Reset
				myPol.verts[0] = c2V(200, 150);
				myPol.verts[1] = c2V(350, 180);
				myPol.verts[2] = c2V(350, 250);
				tinyCapsule.a = { 300, 100 };
				tinyCapsule.b = { 400, 100 };
				npc.setPosition(0, 0);
				testCircle.p.x = 200;
				testCircle.p.y = 200;
				firstEntry = false;
			}
			playerCircleC2.p.x = window.mapPixelToCoords(sf::Mouse::getPosition(window)).x;
			playerCircleC2.p.y = window.mapPixelToCoords(sf::Mouse::getPosition(window)).y;

			// Circle to Capsule
			result = c2CircletoCapsule(playerCircleC2, tinyCapsule);
			cout << ((result != 0) ? ("Circle to Capsule - Collision") : "") << endl;
			if (result) {
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to Poly
			result = c2CircletoPoly(playerCircleC2, &myPol, NULL);
			cout << ((result != 0) ? ("Circle to Poly - Collision") : "") << endl;
			if (result) {
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to AABB
			result = c2CircletoAABB(playerCircleC2, aabb_npc);
			cout << ((result != 0) ? ("Circle to AABB - Collision") : "") << endl;
			if (result) {
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

			// Circle to Circle
			result = c2CircletoCircle(playerCircleC2, testCircle);
			cout << ((result != 0) ? ("Circle to Circle - Collision") : "") << endl;
			if (result) {
				playerCircle.setFillColor(sf::Color(255, 0, 0));
				collided = true;
			}
			else if (collided == false) {
				playerCircle.setFillColor(sf::Color(0, 255, 0));
			}

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

			collided = false;
		}
		else if (myPlayerShape == Ray)
		{
			if (firstEntry == true)
			{
				playerAABB.setPosition(-2000, -2000);
				playerCircleC2.p.x = -1000;
				playerCircleC2.p.y = -1000;
				npcRay.p = c2V(400, 500);
				npcRay.d = c2V(400, 500);
				
				// Reset
				myPol.verts[0] = c2V(200, 150);
				myPol.verts[1] = c2V(350, 180);
				myPol.verts[2] = c2V(350, 250);
				tinyCapsule.a = { 300, 100 };
				tinyCapsule.b = { 400, 100 };
				npc.setPosition(0, 0);
				testCircle.p.x = 200;
				testCircle.p.y = 200;
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
			result = c2RaytoAABB(npcRay, aabb_npc, &cast);
			cout << ((result != 0) ? ("Ray to AABB - Collision") : "") << endl;
			if (result) {
				startOfRay.color =  sf::Color::Red ;
				endofRay.color = sf::Color::Red;
			}
			else if (collided == false) {
				startOfRay.color = sf::Color::Green;
				endofRay.color = sf::Color::Green;
			}
			

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
		}

		// Circle
		testCircle.p.x += direction.x;
		testCircle.p.y += direction.y;

		maCircle.setPosition(testCircle.p.x, testCircle.p.y);
		playerCircle.setPosition(playerCircleC2.p.x, playerCircleC2.p.y);
		// Capsule
		tinyCapsule.a.x += direction.x;
		tinyCapsule.a.y += direction.y;
		tinyCapsule.b.x += direction.x;
		tinyCapsule.b.y += direction.y;


		myPol.verts[0] = c2V(myPol.verts[0].x + 0.01, myPol.verts[0].y + 0.01);
		myPol.verts[1] = c2V(myPol.verts[1].x + 0.01, myPol.verts[1].y + 0.01);
		myPol.verts[2] = c2V(myPol.verts[2].x + 0.01, myPol.verts[2].y + 0.01);
		tri.setPoint(0, sf::Vector2f{ myPol.verts[0].x, myPol.verts[0].y });
		tri.setPoint(1, sf::Vector2f{ myPol.verts[1].x, myPol.verts[1].y });
		tri.setPoint(2, sf::Vector2f{ myPol.verts[2].x, myPol.verts[2].y });

		npcCapule.setPosition(sf::Vector2f{ tinyCapsule.a.x, tinyCapsule.a.y });

		// Move The NPC
		sf::Vector2f move_to(npc.getPosition().x + direction.x, npc.getPosition().y + direction.y);
		if (move_to.x < 0) {
			direction.x *= -1;
			move_to.x = 0; 
		}
		else if (move_to.x + npc.getGlobalBounds().width >= 800) {
			direction.x *= -1;
			move_to.x = 800 - npc.getGlobalBounds().width;
		}
		else if (move_to.y < 0) {
			direction.y *= -1;
			move_to.y = 0; 
		}
		else if (move_to.y + npc.getGlobalBounds().height >= 600) {
			direction.y *= -1;
			move_to.y = 600 - npc.getGlobalBounds().height;
		}
		npc.setPosition(move_to);

		// Update NPC AABB set x and y
		aabb_npc.min = c2V(npc.getPosition().x, npc.getPosition().y);
		aabb_npc.max = c2V(npc.getPosition().x + npc.getGlobalBounds().width, npc.getPosition().y + npc.getGlobalBounds().height);

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
		window.draw(maCircle);
		// Draw the Players Current Animated Sprite
		
		npcCapule.render(window);
		// Draw the NPC's Current Animated Sprite
		window.draw(npc);
		window.draw(tri);
		window.draw(playerCircle);
		window.draw(playerAABB);
		window.draw(vertexArray);
		// Update the window
		window.display();
	}

	return EXIT_SUCCESS;
};



