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
	std::vector<std::vector<float>> height{};

	//for print data
	int niveau = 0;

	int Xsize = 0;
	int Ysize = 0;
	int Zsize = 0;
};

struct Terrain
{
	float seed;

	int Xsize = 16;
	int Ysize = 16;
	int Zsize = 64;

	int octave = 8;
	float persistance = 0.25f;
	float lacunarity = 2.f;

	float seuil = 0.7f;

	int modificateur = 32;
	
	GameObject player;

	std::vector<Chunk> data;
};
