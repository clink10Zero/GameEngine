#pragma once

#include "system.hpp"
#include "../components/aabb.hpp"
#include "../components/terrain.hpp"
#include "../components/mesh.hpp"
#include "../ecs/types.hpp"

#include "../lve_model.hpp"
#include "../lve_device.hpp"

#include <iterator>
#include <algorithm>
#include <random>

using namespace lve;

class TerrainSystem : public System
{
	public:
		void Init();

		void Update(float dt, LveDevice& lveDevice);

	private:

		Chunk CreateChunk(int _x, int _z, int Xsize, int Ysize, int Zsize, int octave, float bias, int modificateur, unsigned int seed, float seuil);
		
		void triangulationCube(LveModel::Builder& builder, Chunk c, int x, int y, int z);

		void AddFace(LveModel::Builder& builder, LveModel::Vertex v1, LveModel::Vertex v2, LveModel::Vertex v3, LveModel::Vertex v4);

		std::vector<float> makeSeedTab(unsigned int seed, int nWidth, int nHeight) {
			srand(seed);
			std::vector<float> tabSeed{};
			for (int i = 0; i < nWidth * nHeight; i++) tabSeed.push_back((float)rand() / (float)RAND_MAX);

			return tabSeed;
		}

		std::vector<int> PerlinNoise2D(int nWidth, int nHeight, std::vector<float> fSeed, int nOctaves, float fBias, int modificateur)
		{
			std::vector<int> fOutput{};

			// Used 1D Perlin Noise
			for (int x = 0; x < nWidth; x++)
			{
				for (int y = 0; y < nHeight; y++)
				{
					float fNoise = 0.0f;
					float fScaleAcc = 0.0f;
					float fScale = 1.0f;

					for (int o = 0; o < nOctaves; o++)
					{
						int nPitch = nWidth >> o;
						int nSampleX1 = (x / nPitch) * nPitch;
						int nSampleY1 = (y / nPitch) * nPitch;

						int nSampleX2 = (nSampleX1 + nPitch) % nWidth;
						int nSampleY2 = (nSampleY1 + nPitch) % nWidth;

						float fBlendX = (float)(x - nSampleX1) / (float)nPitch;
						float fBlendY = (float)(y - nSampleY1) / (float)nPitch;

						float fSampleT = (1.0f - fBlendX) * fSeed[nSampleY1 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY1 * nWidth + nSampleX2];
						float fSampleB = (1.0f - fBlendX) * fSeed[nSampleY2 * nWidth + nSampleX1] + fBlendX * fSeed[nSampleY2 * nWidth + nSampleX2];

						fScaleAcc += fScale;
						fNoise += (fBlendY * (fSampleB - fSampleT) + fSampleT) * fScale;
						fScale = fScale / fBias;
					}

					// Scale to seed range
					fOutput.push_back((fNoise / fScaleAcc) * modificateur);
				}
			}

			return fOutput;
		}

};