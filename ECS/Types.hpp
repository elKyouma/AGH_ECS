#pragma once
#include <cstdint>
#include <bitset>
#include <cassert>

#ifdef IN_TEST
#define ASSERT(statement) if(!(statement)) throw(69);
#else 
#define ASSERT(statement) assert(statement); 
#endif // DEBUG

constexpr static uint32_t MAX_ENTITY_COUNT = 100000;
constexpr static uint32_t MAX_COMPONENT_COUNT = 100;
constexpr static uint32_t MAX_SYSTEM_COUNT = 100;

using EntityId = uint32_t;
using SystemId = uint32_t;    
using ComponentId = uint32_t;
using Signature = std::bitset<MAX_COMPONENT_COUNT>;
