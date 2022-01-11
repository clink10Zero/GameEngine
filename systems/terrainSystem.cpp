#include "terrainSystem.hpp"

#include "../ecs/coordinator.hpp"

extern Coordinator gCoordinator;

void TerrainSystem::Init()
{
	for (auto go : mGameObject)
	{
		Terrain& terrain = gCoordinator.GetCompenent<Terrain>(go);
		
		Chunk c = CreateChunk(0, 0, terrain.Xsize, terrain.Ysize, terrain.Zsize, terrain.octave, terrain.persistance, terrain.lacunarity, terrain.modificateur, terrain.seed, terrain.seuil);

		GameObject newChunk = gCoordinator.CreateGameObject();
		Transform t{};

		t.rotation = glm::vec3{ 0.f, 0.f, 0.f };
		t.scale = glm::vec3{ 1.f, 1.f, 1.f };
		t.translation = glm::vec3{ 0.f, 0.f, 0.f };

		Graph g{};
		g.parent = go;

		Graph& gp = gCoordinator.GetCompenent<Graph>(go);
		gp.children.push_back(newChunk);

		terrain.data.push_back(c);

		gCoordinator.AddComponent<Chunk>(newChunk, c);
		gCoordinator.AddComponent<Transform>(newChunk, t);
		gCoordinator.AddComponent<Graph>(newChunk, g);
	}
}

void TerrainSystem::Update(float dt, LveDevice& lveDevice)
{
	for (auto go : mGameObject)
	{
		Terrain& terrain = gCoordinator.GetCompenent<Terrain>(go);
		int size = terrain.data.size();

		for (int i = 0; i < size; i++)
		{
			Chunk& c = terrain.data[i];
			if (!c.update)
			{
				Mesh mc{};
				LveModel::Builder modelBuilder{};

				int Xsize = c.Xsize;
				int Ysize = c.Ysize;
				int Zsize = c.Zsize;
				
				for (int x = 0; x < Xsize; x++)
				{
					for (int y = 0; y < Ysize; y++)
					{
						for (int z = 0; z < Zsize; z++)
						{
							if (c.data[x][y][z].sol)
								triangulationCube(modelBuilder, c, x, y, z);
						}
					}
				}
				mc.data = std::make_unique<LveModel>(lveDevice, modelBuilder);

				Graph g = gCoordinator.GetCompenent<Graph>(go);

				if (gCoordinator.HaveComponent<Mesh>(g.children[i]))
				{
					Mesh& old = gCoordinator.GetCompenent<Mesh>(g.children[i]);
					old.data = mc.data;
				}
				else {
					gCoordinator.AddComponent<Mesh>(g.children[i], mc);
				}
				c.update = true;
			}
		}

	}
}

Chunk TerrainSystem::CreateChunk(int x, int z, int Xsize, int Ysize, int Zsize, int octave, float persistance, float lacunarity, int modificateur, float seed, float seuil)
{
	Chunk c {};
	c.coordonner = std::pair<int, int>{ x, z };
	c.Xsize = Xsize;
	c.Ysize = Ysize;
	c.Zsize = Zsize;

	std::vector<std::vector<float>> heightMap{};

	perlin::Perlin map(1532512342);

	float frequency = 16;
	float fx = Xsize / frequency;
	float fy = Ysize / frequency;

	for (int x = 0; x < Xsize; x++)
	{
		heightMap.push_back(std::vector<float>{});
		for (int y = 0; y < Ysize; y++)
		{
			float p = map.accumulatedNoise2D(x / fx, y / fy, octave, lacunarity, persistance);
			heightMap[x].push_back(p * modificateur);
		}
	}

	c.height = heightMap;

	for (int x = 0; x < Xsize; x++)
	{
		c.data.push_back(std::vector<std::vector<Block>>{});
		for (int y = 0; y < Ysize; y++)
		{
			c.data[x].push_back(std::vector<Block>{});

			float level = heightMap[x][y];

			for (int z = 0; z < Zsize; z++)
			{
				Block b{};
				if (z < level)
				{
					b.sol = true;
				}
				else
				{
					b.sol = false;
				}
				c.data[x][y].push_back(b);
			}
		}
	}
	return c;
}

void TerrainSystem::triangulationCube(LveModel::Builder& builder, Chunk c, int x, int y, int z)
{
	if (y < c.Ysize - 1 && !c.data[x][y + 1][z].sol)
	{
		LveModel::Vertex v1{};
		v1.position = glm::vec3{ x + 0.f, y + 1.f, z + 0.f };

		LveModel::Vertex v2{};
		v2.position = glm::vec3{ x + 0.f, y + 1.f, z + 1.f};

		LveModel::Vertex v3{};
		v3.position = glm::vec3{ x + 1.f, y + 1.f, z + 0.f};

		LveModel::Vertex v4{};
		v4.position = glm::vec3{ x + 1.f, y + 1.f, z + 1.f};

		AddFace(builder, v1, v2, v3, v4);
	}

	if (y > 0 && !c.data[x][y - 1][z].sol)
	{
		LveModel::Vertex v1{};
		v1.position = glm::vec3{ x + 1.f, y + 0.f, z + 1.f };

		LveModel::Vertex v2{};
		v2.position = glm::vec3{ x + 0.f, y + 0.f, z + 1.f };

		LveModel::Vertex v3{};
		v3.position = glm::vec3{ x + 1.f, y + 0.f, z + 0.f };

		LveModel::Vertex v4{};
		v4.position = glm::vec3{ x + 0.f, y + 0.f, z + 0.f };

		AddFace(builder, v1, v2, v3, v4);
	}

	if (z != c.Zsize - 1)
	{
		if (!c.data[x][y][z + 1].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 0.f, y + 0.f, z + 1.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, y + 0.f, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, y + 1.f, z + 1.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 1.f, y + 1.f, z + 1.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}
	
	if (z != 0)
	{
		if (!c.data[x][y][z - 1].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 1.f, y + 1.f, z + 0.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, y + 0.f, z + 0.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, y + 1.f, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 0.f, y + 0.f, z + 0.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}

	if (x != c.Xsize - 1)
	{
		if (!c.data[x + 1][y][z].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 1.f, y + 1.f, z + 0.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, y + 1.f, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 1.f, y + 0.f, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 1.f, y + 0.f, z + 1.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}

	if (x != 0)
	{
		if (!c.data[x - 1][y][z].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 0.f, y + 0.f, z + 1.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 0.f, y + 1.f, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, y + 0.f, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 0.f, y + 1.f, z + 0.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}
}

void TerrainSystem::AddFace(LveModel::Builder& builder, LveModel::Vertex v1, LveModel::Vertex v2, LveModel::Vertex v3, LveModel::Vertex v4)
{
	int index = builder.vertices.size();

	builder.vertices.push_back(v1);
	builder.vertices.push_back(v2);
	builder.vertices.push_back(v3);
	builder.vertices.push_back(v4);

	glm::uvec2 indexV1 = GetIndex(builder, index, v1);
	glm::uvec2 indexV2 = GetIndex(builder, indexV1.x, v2);
	glm::uvec2 indexV3 = GetIndex(builder, indexV2.x, v3);
	glm::uvec2 indexV4 = GetIndex(builder, indexV3.x, v4);

	builder.indices.push_back(indexV1.y);
	builder.indices.push_back(indexV2.y);
	builder.indices.push_back(indexV3.y);
	builder.indices.push_back(indexV2.y);
	builder.indices.push_back(indexV4.y);
	builder.indices.push_back(indexV3.y);
}

glm::uvec2 TerrainSystem::GetIndex(LveModel::Builder builder, int index, LveModel::Vertex v1)
{
	int indexV1 = index;
	int tmp = IndexOf(builder.vertices, v1);
	if (tmp != -1)
	{
		indexV1 = tmp;
	}
	else
	{
		builder.vertices.push_back(v1);
		index++;
	}
	return {index, indexV1};
}