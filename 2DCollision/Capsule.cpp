#include "Capsule.h"



Capsule::Capsule()
{
}

Capsule::Capsule(sf::Vector2f t_startCirclePos, sf::Vector2f t_endCirclePos, float t_radius, sf::Color t_colour)  :
	m_startCircle(t_radius),
	m_endCircle(t_radius),
	m_radius(t_radius),
	m_length(t_endCirclePos.x - t_startCirclePos.x)

{
	m_startCircle.setOrigin(t_radius, t_radius);
	m_endCircle.setOrigin(t_radius, t_radius);
	m_capsuleBridge.setOrigin(m_length / 2, t_radius);

	m_startCircle.setPosition(t_startCirclePos);
	m_endCircle.setPosition(t_endCirclePos.x, t_endCirclePos.y);

	
	m_capsuleBridge.setPosition(m_startCircle.getPosition().x + (m_length / 2), m_startCircle.getPosition().y);
	m_capsuleBridge.setSize(sf::Vector2f{ m_length, t_radius * 2 });
	

	m_startCircle.setFillColor(t_colour);
	m_endCircle.setFillColor(t_colour);
	m_capsuleBridge.setFillColor(t_colour);

}


Capsule::~Capsule()
{
}

void Capsule::update()
{
}

void Capsule::render(sf::RenderWindow & t_wind)
{
	t_wind.draw(m_startCircle);
	t_wind.draw(m_endCircle);
	t_wind.draw(m_capsuleBridge);
}

void Capsule::setPosition(sf::Vector2f t_pos)
{
	m_startCircle.setPosition(t_pos);
	m_capsuleBridge.setPosition(t_pos.x + m_length / 2, t_pos.y);
	m_endCircle.setPosition(t_pos.x + m_length, t_pos.y);
}

void Capsule::adjustBridge()
{
	sf::Vector2f dist = m_endCircle.getPosition() - m_startCircle.getPosition();
	double m_radians = atan2(dist.y, dist.x);		// Inverse tan of y/x for direction in radians
	double degrees = m_radians * (180 / 3.1415926535897932384);		// Degrees
	m_capsuleBridge.setRotation(degrees);
}

sf::Vector2f Capsule::getPosition()
{
	return m_startCircle.getPosition();
}

sf::Vector2f Capsule::getPositionEnd()
{
	return m_endCircle.getPosition();
}

void Capsule::setColor(sf::Color t)
{
	m_startCircle.setFillColor(t);
	m_endCircle.setFillColor(t);
	m_capsuleBridge.setFillColor(t);
}

float Capsule::getRadius()
{
	return m_radius;
}
