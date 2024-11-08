#include "gtest/gtest.h"
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
  comP.AddComponent(0);
  comP.DeleteComponent(0);
  EXPECT_FALSE(comP.TryGetComponent(0).has_value());

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

TEST_F(ComponentPoolTest, ManipulatingManyComponents)
{
  ComponentPool<Position> comP;
  for(int i=0; i<15; i++)
  {
    try {
      auto& pos = comP.AddComponent(i);
      pos.Set(i, i);
    }
    catch (int ex) {
      ASSERT_TRUE(false) << "Adding component resulted in failure";
    }
  }
  
  ASSERT_TRUE(comP.TryDeleteComponent(6)) << "You can't delete 6th component";
  ASSERT_TRUE(comP.TryDeleteComponent(9)) << "You can't delete 9th component";

  EXPECT_FALSE(comP.TryGetComponent(6).has_value()) << "You have access to non-existent component";
  
  try {
    auto& pos = comP.AddComponent(6);
    pos.Set(6.0, 9.0);
  }
  catch (int ex) {
    ASSERT_TRUE(false) << "Adding 6th component resulted in failure";
  }

  EXPECT_TRUE(comP.TryGetComponent(6).has_value()) << "You don't have access to 6th component";
  
  for(int i=0; i<15; i++)
  {
    if(comP.TryGetComponent(i).has_value())
    {
      auto& pos = comP.GetComponent(i);
      if(i != 6)
      {
        EXPECT_DOUBLE_EQ(pos.x, static_cast<double>(i));
        EXPECT_DOUBLE_EQ(pos.y, static_cast<double>(i));
      }
      else
      {
        EXPECT_DOUBLE_EQ(pos.x, 6.0);
        EXPECT_DOUBLE_EQ(pos.y, 9.0);
      }
    }
  }
}
