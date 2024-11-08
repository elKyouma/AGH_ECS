#include <gtest/gtest.h>
#include "Component.hpp"
class ComponentPoolTest : public testing::Test
{
  protected:

  struct Position
  {
    double x;
    double y;
    
    void Set(double x, double y)
    {
      this->x = x;
      this->y = y;
    }
  };
  

};

TEST_F(ComponentPoolTest, AddingComponent) {
  ComponentPool<Position> comP;
  comP.AddComponent(0);
  EXPECT_ANY_THROW(comP.AddComponent(0));
}

TEST_F(ComponentPoolTest, DeletingComponent)
{
  ComponentPool<Position> comP;
  EXPECT_FALSE(comP.TryDeleteComponent(0)) << "You can delete non-existent component";
  EXPECT_ANY_THROW(comP.DeleteComponent(0)) << "You deleted non-existent component";
}

TEST_F(ComponentPoolTest, GettingComponent)
{
  ComponentPool<Position> comP;
  EXPECT_FALSE(comP.TryGetComponent(0).has_value()) << "You have access to non-existent component";
  auto& pos = comP.AddComponent(0);
  EXPECT_TRUE(comP.TryGetComponent(0).has_value()) << "You don't have access to existing component";
  pos.Set(6.0, 9.0);
  auto& pos2 = comP.GetComponent(0);
  EXPECT_DOUBLE_EQ(pos.x, pos2.x);
  EXPECT_DOUBLE_EQ(pos.y, pos2.y);
}
