#pragma once
#include <cstdint>
#include <bitset>

constexpr static uint32_t MAX_ENTITY_COUNT = 100000;
constexpr static uint32_t MAX_COMPONENT_COUNT = 100;

using EntityId = uint32_t;
using Signature = std::bitset<MAX_COMPONENT_COUNT>;
