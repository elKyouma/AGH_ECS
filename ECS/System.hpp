#include <bitset>
#include "Component.hpp"

class System
{
public:
    std::bitset<MAX_COMPONENT_COUNT> requiredComponents;
};
