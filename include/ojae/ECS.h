#pragma once

// https://www.youtube.com/watch?v=XsvI8Sng6dk&t=1042s

#include <vector>
#include <memory>
#include <algorithm>
#include <bitset>
#include <array>
#include <map>
#include <string>

#include "Character.h"

struct Component;
class Entity;

using ComponentID = std::size_t;

inline ComponentID getComponentTypeID()
{
    static ComponentID lastID = 0;
    return lastID++;
}

template<typename T> inline ComponentID getComponentTypeID() noexcept
{
    static ComponentID typeID = getComponentTypeID();
    return typeID;
}

constexpr std::size_t maxComponents = 32;

using ComponentBitSet = std::bitset<maxComponents>;
using ComponentArray = std::array<Component*, maxComponents>;

struct Component
{

Component() {}
virtual ~Component() {}

Entity* entity;

// virtual void init() {}
virtual void update() {}
// virtual void draw() {}

};

class EntityHandler;

class Entity
{

private:

    std::string name; 
    std::vector<std::string> tags;

    // bool active = true;
    std::vector<Component*> components;

    ComponentArray componentArray;
    ComponentBitSet componentBitSet;

public:

    EntityHandler* entity_handler;

    Entity() {}
    
    Entity(std::string _name)
    {
        name = _name;
    }

    ~Entity() {}

    std::vector<std::string>& get_tags() { return tags; }

    void update()
    {
        for(auto& c : components) c->update();
        // for(auto& c : components) c->draw();
    }

    void add_tag(std::string tag)
    {
        tags.push_back(tag);
    }

    bool remove_tag(std::string tag)
    {
        for(int i = 0; i < tags.size(); i++)
        {
            if(tags.at(i) == tag)
            {
                tags.erase(tags.begin() + i);
                return true;
            }
        }

        return false;
    }

    template <typename T> bool hasComponent() const
    {
        return componentBitSet[getComponentTypeID<T>()];
    }

    template <typename T, typename... TArgs>
    T& addComponent(TArgs&&... mArgs)
    {
        T* c(new T(std::forward<TArgs>(mArgs)...));
        c->entity = this;
        components.emplace_back(c);

        componentArray[getComponentTypeID<T>()] = c;
        componentBitSet[getComponentTypeID<T>()] = true;

        // c->init();
        return *c;
    }

    template <typename T> T* getComponent() const 
    {
        return static_cast<T*>(componentArray[getComponentTypeID<T>()]);
    }
};

class EntityHandler
{

private:

    // std::vector<Entity*> entities;
    std::map<std::pair<int, int>, std::vector<Entity*>> entity_positions;

    int width;
    int height;

public:

    EntityHandler() {}

    ~EntityHandler() {}

    void update()
    {
        for(std::map<std::pair<int, int>, std::vector<Entity*>>::iterator it =
            entity_positions.begin(); it != entity_positions.end(); it++ ) 
        {
            for(Entity* e : it->second) e->update();
        }
    }
        
    std::map<std::pair<int, int>, std::vector<Entity*>> get_entity_positions()
    {
        return entity_positions;
    }

    void add_entity(Entity* e, int x, int y)
    {
        // entities.push_back(e);
        e->entity_handler = this;
        entity_positions[{x, y}].push_back(e);
    }

    bool remove_entity(Entity* e, int x, int y)
    {

        for(int i = 0; i < entity_positions[{x, y}].size(); i++)
        {
            if(entity_positions[{x, y}].at(i) == e)
            {
                entity_positions[{x, y}].erase(
                    entity_positions[{x, y}].begin() + i);
                return true;
            }
        }
        return false;
    }
};
