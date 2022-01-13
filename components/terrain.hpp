#pragma once

#include <glm\glm.hpp>
#include <vector>

struct Block
{
	bool sol;
};

struct Chunk
{
	bool update = false;
	std::pair<int, int> coordonner;

	std::vector<std::vector<std::vector<Block>>> data{};
	std::vector<int> height{};

	//for print data
	int niveau = 0;

	int Xsize = 0;
	int Ysize = 0;
	int Zsize = 0;
};

struct Terrain
{
	int seed;

	int Xsize = 16;
	int Ysize = 64;
	int Zsize = 16;

	int octave = 4;
	float scalingBiais = 2.f;

	float seuil = 0.7f;

	int modificateur = 32;
	
	GameObject player;

	std::vector<Chunk> data;
};
