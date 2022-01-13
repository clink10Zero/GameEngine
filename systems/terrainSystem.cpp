#include "terrainSystem.hpp"

#include "../ecs/coordinator.hpp"

extern Coordinator gCoordinator;

void TerrainSystem::Init()
{
	for (auto go : mGameObject)
	{
		Terrain& terrain = gCoordinator.GetCompenent<Terrain>(go);
		
		Chunk c = CreateChunk(0, 0, terrain.Xsize, terrain.Ysize, terrain.Zsize, terrain.octave, terrain.scalingBiais, terrain.modificateur, terrain.seed, terrain.seuil);

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
					for (int z = 0; z < Zsize; z++)
					{
						for (int y = 0; y < Ysize; y++)
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

int TerrainSystem::getChunkAt(Terrain& terrain, int x, int z){
	for (int i = 0; i < terrain.data.size();i++){
		Chunk currentChunk = terrain.data[i];
		if (currentChunk.coordonner.first*(i+1) <= x && (currentChunk.coordonner.first + currentChunk.Xsize)*(i+1) >= x &&
			currentChunk.coordonner.second*(i+1) <= z && (currentChunk.coordonner.second + currentChunk.Zsize)*(i+1) >= z){
			return i;
		}
	}
	return -1;
}


bool TerrainSystem::getSolInChunkAt(Chunk& c, int x, int y, int z){
	return c.data[x%c.Xsize][-y][z%c.Zsize].sol;
}

Chunk TerrainSystem::CreateChunk(int _x, int _z, int Xsize, int Ysize, int Zsize, int octave, float bias, int modificateur, unsigned int seed, float seuil)
{
	Chunk c {};
	c.coordonner = std::pair<int, int>{ _x, _z };
	c.Xsize = Xsize;
	c.Ysize = Ysize;
	c.Zsize = Zsize;

	std::vector<int> heightMap = PerlinNoise2D(c.Xsize, c.Zsize, makeSeedTab(seed, c.Xsize, c.Zsize), octave, bias, modificateur);

	c.height = heightMap;

	for (int x = 0; x < Xsize; x++)
	{
		c.data.push_back(std::vector<std::vector<Block>>{});
		for (int y = 0; y < Ysize; y++)
		{
			c.data[x].push_back(std::vector<Block>{});

			for (int z = 0; z < Zsize; z++)
			{

				int level = heightMap[x * Xsize + z] + 1;

				Block b{};
				if (y < level)
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
		v1.position = glm::vec3{ x + 0.f, -y, z + 0.f };

		LveModel::Vertex v2{};
		v2.position = glm::vec3{ x + 0.f, -y, z + 1.f};

		LveModel::Vertex v3{};
		v3.position = glm::vec3{ x + 1.f, -y, z + 0.f};

		LveModel::Vertex v4{};
		v4.position = glm::vec3{ x + 1.f, -y, z + 1.f};

		AddFace(builder, v1, v2, v3, v4);
	}

	if (y > 0 && !c.data[x][y - 1][z].sol)
	{
		LveModel::Vertex v1{};
		v1.position = glm::vec3{ x + 1.f, -y + 1.f, z + 1.f };

		LveModel::Vertex v2{};
		v2.position = glm::vec3{ x + 0.f, -y + 1.f, z + 1.f };

		LveModel::Vertex v3{};
		v3.position = glm::vec3{ x + 1.f, -y + 1.f, z + 0.f };

		LveModel::Vertex v4{};
		v4.position = glm::vec3{ x + 0.f, -y + 1.f, z + 0.f };

		AddFace(builder, v1, v2, v3, v4);
	}

	if (z != c.Zsize - 1)
	{
		if (!c.data[x][y][z + 1].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 0.f, -y + 1.f, z + 1.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, -y + 1.f, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, -y, z + 1.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 1.f, -y, z + 1.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}
	
	if (z != 0)
	{
		if (!c.data[x][y][z - 1].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 1.f, -y, z + 0.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, -y + 1.f, z + 0.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, -y, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 0.f, -y + 1.f, z + 0.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}

	if (x != c.Xsize - 1)
	{
		if (!c.data[x + 1][y][z].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 1.f, -y, z + 0.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 1.f, -y, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 1.f, -y + 1.f, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 1.f, -y + 1.f, z + 1.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}

	if (x != 0)
	{
		if (!c.data[x - 1][y][z].sol)
		{
			LveModel::Vertex v1{};
			v1.position = glm::vec3{ x + 0.f, -y + 1.f, z + 1.f };

			LveModel::Vertex v2{};
			v2.position = glm::vec3{ x + 0.f, -y, z + 1.f };

			LveModel::Vertex v3{};
			v3.position = glm::vec3{ x + 0.f, -y + 1.f, z + 0.f };

			LveModel::Vertex v4{};
			v4.position = glm::vec3{ x + 0.f, -y, z + 0.f };

			AddFace(builder, v1, v2, v3, v4);
		}
	}
}

void TerrainSystem::AddFace(LveModel::Builder& builder, LveModel::Vertex v1, LveModel::Vertex v2, LveModel::Vertex v3, LveModel::Vertex v4)
{
	int index = builder.vertices.size();

	v1.color = glm::vec3{ 0.f, 0.f, 0.f };
	v2.color = glm::vec3{ 0.f, 0.f, 0.f };
	v3.color = glm::vec3{ 0.f, 0.f, 0.f };
	v4.color = glm::vec3{ 0.f, 0.f, 0.f };

	builder.vertices.push_back(v1);
	builder.vertices.push_back(v2);
	builder.vertices.push_back(v3);
	builder.vertices.push_back(v4);

	builder.indices.push_back(index);
	builder.indices.push_back(index + 1);
	builder.indices.push_back(index + 2);
	builder.indices.push_back(index + 1);
	builder.indices.push_back(index + 3);
	builder.indices.push_back(index + 2);

}