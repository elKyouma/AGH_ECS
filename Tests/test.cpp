#include <gtest/gtest.h>
#include "Component.hpp"
class ComponentPoolTest : public testing::Test
{
  protected:

  struct Position
  {
    double x;
    double y;
  };
};

TEST_F(ComponentPoolTest, AddingComponent) {
  ComponentPool<Position> comP;
  comP.AddComponent(0);
  EXPECT_ANY_THROW(comP.AddComponent(0));
}
