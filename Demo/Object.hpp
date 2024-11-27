#pragma once

class Object
{
public:
    virtual void Init() =0;
    virtual void Update() =0;
    virtual void Clean() =0;
};
