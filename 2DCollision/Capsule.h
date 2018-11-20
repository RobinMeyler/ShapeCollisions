#pragma once

#include <SFML/Graphics.hpp>

class Capsule
{
public:
	Capsule();
	Capsule(sf::Vector2f t_startCirclePos, float t_endCirclePos, float t_radius, sf::Color t_colour);
	~Capsule();
	void update();
	void render(sf::RenderWindow & t_wind);
	void setPosition(sf::Vector2f t_pos);
	sf::Vector2f getPosition();

private:
	sf::CircleShape m_startCircle;
	sf::CircleShape m_endCircle;
	sf::RectangleShape m_capsuleBridge;
	float m_length;
	float m_radius;
	sf::Vector2f m_centre;
};

