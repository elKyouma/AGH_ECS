#include <X11/extensions/randr.h>
#include <array>
#include <gtest/gtest.h>
#include "Component.hpp"
#include "System.hpp"
#include "ECS.hpp"
#include "Types.hpp"
#include <typeindex>
#include "ComponentManager.hpp"

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
    EXPECT_NO_THROW(comP.AddComponent(0));
    EXPECT_ANY_THROW(comP.AddComponent(0));
}

TEST_F(ComponentPoolTest, CheckIfMaxSizeWorksCorrectly)
{
    ComponentPool<Position> comp;
    for(ComponentId id = 0; id < MAX_ENTITY_COUNT; id++)
        EXPECT_NO_THROW(comp.AddComponent(id));

    EXPECT_ANY_THROW(comp.AddComponent(MAX_ENTITY_COUNT));

    ComponentPool<Position> comp2(25);
    for(ComponentId id = 0; id < 25; id++)
        EXPECT_NO_THROW(comp2.AddComponent(30 + id));

    EXPECT_ANY_THROW(comp2.AddComponent(3));

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
        void SetSignature(Signature&) override{}
    };

    class DummySys1 : public System
    {
    public:
        int EntitySize()
        {
            return entities.size();
        }
        
        void SetSignature(Signature& systemSignature) override
        {
            systemSignature.set(compManager->CompId<Position>());
        }
    };
    
    class DummySys2 : public System
    {
    public:
        int EntitySize()
        {
            return entities.size();
        }
        
        void SetSignature(Signature& systemSignature) override
        {
            systemSignature.set(compManager->CompId<Position>());
            systemSignature.set(compManager->CompId<Rotation>());
        }
    };

    ComponentManager compManager{};
    std::array<Signature, MAX_ENTITY_COUNT> signatures;
    std::unordered_map<std::type_index, uint32_t> typeToId;
};

TEST_F(SystemTest, SystemInitialization)
{
    EmptySys sys1;
    EXPECT_ANY_THROW(sys1.Init(signatures.data(), &compManager));
    DummySys2 sys2;
    sys2.Init(signatures.data(), &compManager);

    EXPECT_EQ(sys2.EntitySize(), 1) << "Wrong entity count";
    EXPECT_TRUE(sys2.CheckIfEntitySubscribed(0)) << "Entity with right signature unsubscribed";
    EXPECT_FALSE(sys2.CheckIfEntitySubscribed(1)) << "Entity with wrong signature subscribed";

}

TEST_F(SystemTest, ChangingEntitySignature)
{
    DummySys2 sys;
    sys.Init(signatures.data(), &compManager);

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
    sys.Init(signatures.data(), &compManager);
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
        void SetSignature(Signature& systemSignature) override
        {
            systemSignature.set(compManager->CompId<Position>());
        }

        void Update(float deltaTime) override 
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
        
        void SetSignature(Signature& systemSignature) override
        {
            systemSignature.set(compManager->CompId<Position>());
            systemSignature.set(compManager->CompId<Rotation>());
        }

        void Update(float deltaTime) override 
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

TEST_F(ECSTest, CustomPoolSizesTest)
{
    ECS ecs;
    ecs.RegisterComponentPool<Position>(2);
    ecs.RegisterComponentPool<Rotation>(3);
    
    EXPECT_NO_THROW(ecs.AddComponent<Position>(5));
    EXPECT_NO_THROW(ecs.AddComponent<Position>(6));
    EXPECT_ANY_THROW(ecs.AddComponent<Position>(1));

    EXPECT_NO_THROW(ecs.AddComponent<Rotation>(5));
    EXPECT_NO_THROW(ecs.AddComponent<Rotation>(6));
    EXPECT_NO_THROW(ecs.AddComponent<Rotation>(7));
    EXPECT_ANY_THROW(ecs.AddComponent<Rotation>(2));
}

TEST_F(ECSTest, CreateComponentWithArgs)
{
    ECS ecs;
    auto ent1 = ecs.CreateEntity();
    auto ent2 = ecs.CreateEntity();
    auto ent3 = ecs.CreateEntity();
    ecs.RegisterComponentPool<Position>();
    ecs.AddComponent<Position>(ent1, 2, 3);
    ecs.AddComponent<Position>(ent2);
    ecs.AddComponent<Position>(ent3, 5, 6);
    
    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent1).x, 2);
    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent1).y, 3);
    
    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent2).x, 0);
    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent2).y, 0);

    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent3).x, 5);
    EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent3).y, 6);
    
    auto ents = CreateEntitiesArray(ecs, 90);
    ecs.AddComponents<Position>(std::span(ents.begin(), ents.end()), 9, 7);

    for(const auto ent : ents)
    {
        EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent).x, 9);
        EXPECT_DOUBLE_EQ(ecs.GetComponent<Position>(ent).y, 7);
    }
}

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

    ecs.UpdateSystems(0.1);


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
    ecs.AddComponents<Position>(std::span<EntityId>(entities.begin(), 91));
    ecs.AddComponents<Rotation>(std::span<EntityId>(entities.begin() + 30, 51));
    
    //POS: 0-10; 40-90
    //ROT: 60-80
    ecs.DeleteComponents<Position>(std::span<EntityId>(entities.begin() + 10, 30));
    ecs.DeleteComponents<Rotation>(std::span<EntityId>(entities.begin() + 30, 30));
    
    //ENT: 0-59, 72-100
    //POS: 0-10; 40-59; 72-90
    //ROT: 71-80
    int numberOfDestroyedEnts = 12;
    ecs.DestroyEntities(std::span<EntityId>(entities.begin() + 60, numberOfDestroyedEnts));
    
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

class ComponentManagerTest : public testing::Test
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

    struct Rotation
    {
        double deg;
    };
};

TEST_F(ComponentManagerTest, ComponentPoolRegistration)
{
    ComponentManager compM;
    EXPECT_NO_THROW(compM.RegisterComponentPool<Position>()) << "Can not register component pool (1st compPool)";
    EXPECT_ANY_THROW(compM.RegisterComponentPool<Position>()) << "Registered already existing component pool";

    EXPECT_ANY_THROW(compM.GetComponentPool<Rotation>()) << "Getting access to non-existent ComponentPool";

    EXPECT_NO_THROW(compM.RegisterComponentPool<Rotation>()) << "Can not register component pool (2nd compPool)";
    
    EXPECT_NO_THROW(compM.GetComponentPool<Position>()) << "Can not get access to an existing component pool (1st compPool)";
    EXPECT_NO_THROW(compM.GetComponentPool<Rotation>()) << "Can not get access to an existing component pool (2st compPool)";
}

TEST_F(ComponentManagerTest, ComponentManipulation)
{
    ComponentManager compM;
    compM.RegisterComponentPool<Position>();
    auto& compPool = compM.GetComponentPool<Position>();

    
    auto& pos = compM.AddComponent<Position>(0);
    EXPECT_TRUE(compPool.TryGetComponent(0)) << "Can not add component to an entity";    
    pos.Set(2.1, 3.7);
    
    auto& pos2 = compM.GetComponent<Position>(0);
    EXPECT_DOUBLE_EQ(pos.x, pos2.x) << "Can not get access to component";
    EXPECT_DOUBLE_EQ(pos.y, pos2.y) << "Can not get access to component";
}

TEST_F(ComponentManagerTest, ComponentDestruction)
{
    ComponentManager compM;
    compM.RegisterComponentPool<Position>();
    compM.RegisterComponentPool<Rotation>();
    auto& PosCompPool = compM.GetComponentPool<Position>();
    auto& RotCompPool = compM.GetComponentPool<Rotation>();
    
    
    compM.AddComponent<Position>(0);
    compM.AddComponent<Rotation>(0);
    
    compM.TryDeleteComponent<Position>(0);
    EXPECT_FALSE(PosCompPool.TryGetComponent(0)) << "Getting access to the removed component";

    compM.DeleteComponent<Rotation>(0); 
    EXPECT_FALSE(RotCompPool.TryGetComponent(0)) << "Getting access to the removed component";
    
    compM.AddComponent<Position>(0);
    compM.AddComponent<Rotation>(0);

    compM.DestroyAllComponents(0);
    EXPECT_FALSE(PosCompPool.TryGetComponent(0)) << "Getting access to the removed component";
    EXPECT_FALSE(RotCompPool.TryGetComponent(0)) << "Getting access to the removed component";
}

TEST_F(ComponentManagerTest, DifferentPoolSizes)
{
    ComponentManager compM;
    compM.RegisterComponentPool<Position>(20);
    compM.RegisterComponentPool<Rotation>(10);
    

    for(ComponentId id = 0; id < 20; id++)
    {
        EXPECT_NO_THROW(compM.AddComponent<Position>(id * 10));
        if(id % 2 == 0)
            EXPECT_NO_THROW(compM.AddComponent<Rotation>(id * 3));
    }

    EXPECT_ANY_THROW(compM.AddComponent<Position>(2));
    EXPECT_ANY_THROW(compM.AddComponent<Rotation>(1));
}

TEST_F(ComponentManagerTest, CreateComponentWithArgs)
{
    ComponentManager compM;
    compM.RegisterComponentPool<Position>();
    compM.AddComponent<Position>(0, 2, 3);
    compM.AddComponent<Position>(1);
    compM.AddComponent<Position>(2, 5, 6);
    
    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(0).x, 2);
    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(0).y, 3);
    
    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(1).x, 0);
    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(1).y, 0);

    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(2).x, 5);
    EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(2).y, 6);

    std::array<EntityId, 90> ents;
    for(int id = 0; id < 90; id++)
        ents[id] = id + 4;

    compM.AddComponents<Position>(std::span(ents.begin(), ents.end()), 9, 7);
    for(EntityId ent = 4; ent < 94; ent++)
    {
        EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(ent).x, 9);
        EXPECT_DOUBLE_EQ(compM.GetComponent<Position>(ent).y, 7);
    }
}
