#pragma once

#include "../lve_model.hpp"

using namespace lve;

struct Mesh
{
	std::string path;
	std::shared_ptr<LveModel> data;
	int lod = 0;
};