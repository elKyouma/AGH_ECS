#include <gtest/gtest.h>
#include "../ECS/Component.hpp"

class ComponentPoolTest : public testing::Test
{
  protected:

  struct Position
  {
    double x;
    double y;
  };
  ComponentPool<Position> comP;
};

TEST_F(ComponentPoolTest, AddingComponent) {
  ASSERT_TRUE(true);
  
}
