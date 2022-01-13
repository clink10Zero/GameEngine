#pragma once

#include "Transform.hpp"
#include "terrain.hpp"

struct InterTerrain
{
	float distance;
	Terrain terrain;

	//resultat raycast
	glm::vec3 destroyed;
	glm::vec3 placeBloc;

	int indice;
	bool tileFound = false;
};