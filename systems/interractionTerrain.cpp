#include "interractionTerrain.hpp"

extern Coordinator gCoordinator;
extern std::shared_ptr<GraphSystem> graphSystem;

void InterractionTerrain::init(GLFWwindow* glfw_window)
{
	window = glfw_window;
}

void InterractionTerrain::update(float dt)
{
	for (const auto& go : mGameObject)
	{
		InterTerrain& it = gCoordinator.GetCompenent<InterTerrain>(go);
		Transform t = graphSystem->getWorldPosition(go);
		glm::vec3 direction{ 0.f, 0.f, 1.f };
		direction = direction * t.rotation;
		raycast(t.translation, direction, it);

		if (it.tileFound && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			glm::vec3 destroyed = it.destroyed;
			it.terrain.data[it.indice].data[destroyed.x][destroyed.y][destroyed.z];
			it.terrain.data[it.indice].update = false;
		}
		if (it.tileFound && glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS)
		{
			glm::vec3 placeBloc = it.placeBloc;
			it.terrain.data[it.indice].data[placeBloc.x][placeBloc.y][placeBloc.z];
			it.terrain.data[it.indice].update = false;
		}
	}
}

void InterractionTerrain::raycast(glm::vec3 start, glm::vec3 direction, InterTerrain& it)
{
	glm::vec3 unitStepSize{};
	glm::vec3 rayLenght1D{};
	glm::vec3 step{};
	glm::vec3 mapCheck = start;

	unitStepSize.x = (unitStepSize / unitStepSize.x).length();
	unitStepSize.y = (unitStepSize / unitStepSize.y).length();
	unitStepSize.z = (unitStepSize / unitStepSize.z).length();

	float distanceMax = it.distance;
	Terrain t = it.terrain;

	Chunk c;

	for (int i = 0; i < t.data.size(); i++)
	{
		if (t.data[i].coordonner.first >= start.x / 16 && t.data[i].coordonner.first + 1 <= start.x / 16 &&
			t.data[i].coordonner.second >= start.z / 16 && t.data[i].coordonner.second + 1 <= start.z / 16)
		{
			c = t.data[i];
			it.indice = i;
		}
	}

	if (direction.x < 0.f)
	{
		step.x = -1.f;
		rayLenght1D.x = (start.x - float(mapCheck.x)) * rayLenght1D.x;
	}
	else
	{
		step.x = 1.f;
		rayLenght1D.x = (float(mapCheck.x + 1) - start.x) * rayLenght1D.x;
	}

	if (direction.y < 0.f)
	{
		step.y = -1.f;
		rayLenght1D.y = (start.y - float(mapCheck.y)) * rayLenght1D.y;
	}
	else
	{
		step.y = 1.f;
		rayLenght1D.y = (float(mapCheck.y + 1) - start.y) * rayLenght1D.y;
	}

	if (direction.z < 0.f)
	{
		step.z = -1.f;
		rayLenght1D.z = (start.z - float(mapCheck.z)) * rayLenght1D.z;
	}
	else
	{
		step.z = 1.f;
		rayLenght1D.z = (float(mapCheck.z) - start.z) * rayLenght1D.z;
	}

	it.tileFound = false;
	float distance = 0.f;

	glm::vec3 destroyed = start;
	glm::vec3 placeBloc = start;

	while (!it.tileFound && distance < distanceMax)
	{
		if (rayLenght1D.x < rayLenght1D.y)
		{
			if (rayLenght1D.x < rayLenght1D.z)
			{
				mapCheck.x += step.x;
				distance = rayLenght1D.x;
				rayLenght1D.x += unitStepSize.x;
			}
			else
			{
				mapCheck.z += step.z;
				distance = rayLenght1D.z;
				rayLenght1D.z += unitStepSize.z;
			}
		}
		else
		{
			mapCheck.y += step.y;
			distance = rayLenght1D.y;
			rayLenght1D.y += unitStepSize.y;
		}

		if (mapCheck.x >= 0 && mapCheck.x < c.Xsize && mapCheck.y >= 0 && mapCheck.y < c.Ysize && mapCheck.z >= 0 && mapCheck.z < c.Zsize)
		{
			if (c.data[mapCheck.x][mapCheck.y][mapCheck.z].sol)
			{
				it.tileFound = true;
			}

			placeBloc = destroyed;
			destroyed = mapCheck;
		}
	}
	if (it.tileFound)
	{
		it.destroyed = destroyed;
		it.placeBloc = placeBloc;
	}

}