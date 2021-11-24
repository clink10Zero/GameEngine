#pragma once

#include <bitset>
#include <cstdint>
#include <bitset>

//ECS
using GameObject = std::uint32_t;
const GameObject MAX_GAMEOBJECT = 5000;

using ComponentType = std::uint8_t;
const ComponentType MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;