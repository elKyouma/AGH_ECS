#include <X11/extensions/randr.h>
#include <gtest/gtest.h>
#include "Component.hpp"
#include "System.hpp"
#include "ECS.cpp"
#include "Types.hpp"
#include <span>
#include <typeindex>

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

class SystemTest : public testing::Test
{
protected:
    void SetUp() override
    {
        compManager.RegisterComponentPool<Position>();
        compManager.RegisterComponentPool<Rotation>();
        
        compManager.AddComponent<Position>(0);
        compManager.AddComponent<Rotation>(0);
        signatures[0].set(compManager.CompId<Position>());
        signatures[0].set(compManager.CompId<Rotation>());
        
        signatures[1].set(compManager.CompId<Rotation>());
        compManager.AddComponent<Rotation>(1);
    }

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

    struct Rotation
    {
        double deg;
    };

    class EmptySys : public System
    {
    public:
        void SetSignature() override{}
        void Update() override {}
    };

    class DummySys1 : public System
    {
    public:
        int EntitySize()
        {
            return entities.size();
        }
        
        void SetSignature() override
        {
            systemSignature.set(compManager->CompId<Position>());
        }

        void Update() override {}
    };
    
    class DummySys2 : public System
    {
    public:
        int EntitySize()
        {
            return entities.size();
        }
        
        void SetSignature() override
        {
            systemSignature.set(compManager->CompId<Position>());
            systemSignature.set(compManager->CompId<Rotation>());
        }

        void Update() override {}
    };

    ComponentManager compManager{};
    std::array<Signature, MAX_ENTITY_COUNT> signatures;
    std::unordered_map<std::type_index, uint32_t> typeToId;
};

TEST_F(SystemTest, SystemInitialization)
{
    EmptySys sys1;
    EXPECT_ANY_THROW(sys1.Init(signatures, &compManager));
    DummySys2 sys2;
    sys2.Init(signatures, &compManager);

    EXPECT_EQ(sys2.EntitySize(), 1) << "Wrong entity count";
    EXPECT_TRUE(sys2.CheckIfEntitySubscribed(0)) << "Entity with right signature unsubscribed";
    EXPECT_FALSE(sys2.CheckIfEntitySubscribed(1)) << "Entity with wrong signature subscribed";

}

TEST_F(SystemTest, ChangingEntitySignature)
{
    DummySys2 sys;
    sys.Init(signatures, &compManager);

    signatures[1].set(typeToId[std::type_index(typeid(Position))]);
    sys.OnEntitySignatureChanged(1, signatures[1]);

    EXPECT_EQ(sys.EntitySize(), 2) << "Wrong entity count";
    EXPECT_TRUE(sys.CheckIfEntitySubscribed(1)) << "Entity with right signature unsubscribed";

    signatures[0].reset(typeToId[std::type_index(typeid(Position))]);
    sys.OnEntitySignatureChanged(0, signatures[0]);

    EXPECT_EQ(sys.EntitySize(), 1) << "Wrong entity count";
    EXPECT_FALSE(sys.CheckIfEntitySubscribed(0)) << "Entity with wrong signature subscribed";

}

TEST_F(SystemTest, DeletingEntity)
{
    DummySys2 sys;
    sys.Init(signatures, &compManager);
    EXPECT_ANY_THROW(sys.OnEntityDestroyed(2)) << "Deleted non-existent entity";

    sys.OnEntityDestroyed(0);
    EXPECT_EQ(sys.EntitySize(), 0) << "Wrong entity count";
    EXPECT_FALSE(sys.CheckIfEntitySubscribed(0)) << "Deleted entity is subscribed";
}

class ECSTest : public testing::Test
{
protected:
    std::vector<EntityId> CreateEntitiesArray(ECS& ecs, int N)
    {
        std::vector<EntityId> entities(N);
        for(EntityId i = 0; i < N; i++)
            entities[i] = ecs.CreateEntity();
        
        return entities;
    }

    template<typename Component>
    void AddComponents(ECS& ecs, std::span<EntityId> entities)
    {
        for(const auto ent : entities)
            ecs.AddComponent<Component>(ent);
    }

    template<typename Component>
    void DeleteComponents(ECS& ecs, std::span<EntityId> entities)
    {
        for(const auto ent : entities)
            ecs.DeleteComponent<Component>(ent);
    }

    void DestroyEntities(ECS& ecs, std::span<EntityId> entities)
    {
        for(const auto ent : entities)
            ecs.DestroyEntity(ent);
    }


    struct Position
    {
        double x = 0.0;
        double y = 0.0;
      
        void Set(double x, double y)
        {
            this->x = x;
            this->y = y;
        }
    };

    struct Rotation
    {
        double deg = 0.0;
    };

    class DummySys1 : public System
    {
    public:
        void SetSignature() override
        {
            systemSignature.set(compManager->CompId<Position>());
        }

        void Update() override 
        {
            for(EntityId ent : entities)
            {
                auto& pos = compManager->GetComponent<Position>(ent);

                pos.x += 1.0;
                pos.y += 1.0;
            }
        }
    };

    class DummySys2 : public System
    {
    public:
        
        void SetSignature() override
        {
            systemSignature.set(compManager->CompId<Position>());
            systemSignature.set(compManager->CompId<Rotation>());
        }

        void Update() override 
        {
            for(EntityId ent : entities)
            {
                auto& pos = compManager->GetComponent<Position>(ent);
                auto& rot = compManager->GetComponent<Rotation>(ent);

                pos.x += 2.0;
                pos.y += 2.0;
                rot.deg += 2.0;
            }
        }
    };
};

TEST_F(ECSTest, EntityManipulation)
{
    ECS ecs;
    EntityId ent = ecs.CreateEntity();
    EXPECT_EQ(ent, 0) << "1";
    ent = ecs.CreateEntity();
    EXPECT_EQ(ent, 1) << "2";
    ent = ecs.CreateEntity();
    EXPECT_EQ(ent, 2) << "3";

    ecs.DestroyEntity(1);

    ent = ecs.CreateEntity();
    EXPECT_EQ(ent, 1) << "4";
}

TEST_F(ECSTest, ComponentManipulation)
{
    ECS ecs;
    EntityId ent = ecs.CreateEntity();
    EntityId ent2 = ecs.CreateEntity();

    ASSERT_NO_THROW(ecs.RegisterComponentPool<Position>()) << "Can not register component pool (Position)";
    EXPECT_ANY_THROW(ecs.RegisterComponentPool<Position>()) << "Registered already existing component pool";
    ASSERT_NO_THROW(ecs.RegisterComponentPool<Rotation>()) << "Can not register component pool (Rotation)";

    ASSERT_NO_THROW(auto& pos = ecs.AddComponent<Position>(ent)) << "Can not add component to entity signature (Position)";
    EXPECT_ANY_THROW(ecs.AddComponent<Position>(ent)) << "Added already existing component in entity signature";
    ASSERT_NO_THROW(auto& pos = ecs.AddComponent<Rotation>(ent)) << "Can not add component to entity signature (Rotation)";
    

    EXPECT_ANY_THROW(ecs.GetComponent<Position>(ent2)) << "Getting access to non-existent component";    
    ASSERT_NO_THROW(ecs.GetComponent<Position>(ent)) << "Can not get access to an existing component";
   
    EXPECT_NO_THROW(ecs.TryDeleteComponent<Position>(ent)) << "Can not delete an existing component (TryDeleteComponent)";
    EXPECT_ANY_THROW(ecs.TryDeleteComponent<Position>(ent)) << "Deleted non-existent component (TryDeleteComponent)";
    
    EXPECT_NO_THROW(ecs.DeleteComponent<Rotation>(ent)) << "Can not delete an existing component (DeleteComponent)";
    EXPECT_ANY_THROW(ecs.DeleteComponent<Rotation>(ent)) << "Deleted non-existent component (DeleteComponent)";

    EXPECT_ANY_THROW(ecs.GetComponent<Position>(ent)) << "Getting access to deleted component";

    EXPECT_NO_THROW(ecs.AddComponent<Position>(ent));
    ecs.DestroyEntity(ent);
    EXPECT_ANY_THROW(ecs.GetComponent<Position>(ent)) << "Getting access to destroyed entity component";
}

TEST_F(ECSTest, SystemManipulation)
{
    ECS ecs;
    EntityId ent1 = ecs.CreateEntity();
    EntityId ent2 = ecs.CreateEntity();

    ecs.RegisterComponentPool<Position>();
    ecs.RegisterComponentPool<Rotation>();

    auto& pos1 = ecs.AddComponent<Position>(ent1);
    auto& pos2 = ecs.AddComponent<Position>(ent2);
    auto& rot2 = ecs.AddComponent<Rotation>(ent2);

    EXPECT_NO_THROW(ecs.RegisterSystem<DummySys1>()) << "Can not register system (DummySys1)";
    EXPECT_ANY_THROW(ecs.RegisterSystem<DummySys1>()) << "Registered already existing system";
    EXPECT_NO_THROW(ecs.RegisterSystem<DummySys2>()) << "Can not register component pool (DummySys2)";

    ecs.UpdateSystems();


    EXPECT_DOUBLE_EQ(pos1.x, 1.0) << "1";
    EXPECT_DOUBLE_EQ(pos2.x, 3.0) << "2";
    EXPECT_DOUBLE_EQ(rot2.deg, 2.0) << "3";
}

TEST_F(ECSTest, MultipleDeletionHandling)
{
    ECS ecs;
    auto entities = CreateEntitiesArray(ecs, 100);

    ecs.RegisterComponentPool<Position>();
    ecs.RegisterComponentPool<Rotation>();
    
    
    //POS: 0-90
    //ROT: 30-80 
    AddComponents<Position>(ecs, std::span<EntityId>(entities.begin(), 91));
    AddComponents<Rotation>(ecs, std::span<EntityId>(entities.begin() + 30, 51));
    
    //POS: 0-10; 40-90
    //ROT: 60-80
    DeleteComponents<Position>(ecs, std::span<EntityId>(entities.begin() + 10, 30));
    DeleteComponents<Rotation>(ecs, std::span<EntityId>(entities.begin() + 30, 30));
    
    //ENT: 0-59, 72-100
    //POS: 0-10; 40-59; 72-90
    //ROT: 71-80
    int numberOfDestroyedEnts = 12;
    DestroyEntities(ecs, std::span<EntityId>(entities.begin() + 60, numberOfDestroyedEnts));
    
    for(EntityId id = 40; id <= 90; id++)
        if(id >= 60 && id <= 71)
            EXPECT_ANY_THROW(ecs.GetComponent<Position>(id)); //non existing ent
        else
            EXPECT_NO_THROW(ecs.GetComponent<Position>(id)); 

    for(EntityId id = 0; id < 100; id++)
        if(id >= 72 && id <= 80)
            EXPECT_NO_THROW(ecs.GetComponent<Rotation>(id));
        else
            EXPECT_ANY_THROW(ecs.GetComponent<Rotation>(id)); 

    
    
    auto newEnts = CreateEntitiesArray(ecs, 30);
    for(int i = 0; i < 30; i++)
        if(i < numberOfDestroyedEnts)
            EXPECT_EQ(newEnts[i], 71 - i);
        else
            EXPECT_EQ(newEnts[i], 100 + i - numberOfDestroyedEnts);
}

