#include "Capsule.h"



Capsule::Capsule()
{
}

Capsule::Capsule(sf::Vector2f t_startCirclePos, float t_endCirclePos, float t_radius, sf::Color t_colour)  :
	m_startCircle(t_radius),
	m_endCircle(t_radius),
	m_radius(t_radius),
	m_length(t_endCirclePos)

{
	m_startCircle.setOrigin(t_radius, t_radius);
	m_endCircle.setOrigin(t_radius, t_radius);

	m_startCircle.setPosition(t_startCirclePos);
	m_endCircle.setPosition(t_startCirclePos.x + t_endCirclePos, t_startCirclePos.y);
	m_capsuleBridge.setPosition(m_startCircle.getPosition().x, m_startCircle.getPosition().y - t_radius);
	m_capsuleBridge.setSize(sf::Vector2f{t_endCirclePos, t_radius * 2 });

	m_startCircle.setFillColor(t_colour);
	m_endCircle.setFillColor(t_colour);
	m_capsuleBridge.setFillColor(t_colour);
	m_centre = { t_startCirclePos.x + (t_endCirclePos / 2) , t_startCirclePos.y  };
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
	m_capsuleBridge.setPosition(t_pos.x, t_pos.y - m_radius);
	m_endCircle.setPosition(t_pos.x + m_length, t_pos.y);
}

sf::Vector2f Capsule::getPosition()
{
	return m_startCircle.getPosition();
}
