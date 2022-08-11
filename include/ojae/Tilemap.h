#pragma once

#include <string>
#include <vector>
#include <map>

#include "Tile.h"
#include "Entity.h"
#include "Item.h"
#include "Player.h"

class Tilemap
{

private:

    std::map<std::pair<int,int>, std::vector<Tile*>> tiles; // List of tiles to their 
        // coordinates
    std::map<std::pair<int,int>, std::vector<Entity*>> entities; // List of entities to
        // their coordinates

    std::vector<char> display; // Stores the top priority characters for 
        // rendering our tilemap

    int width; // Width of the tilemap
    int height; // Height of the tilemap

    Player* player; // Instance of the Player

    TextFunnel* text_funnel; // Instance of the TextFunnel

public:

    bool keep_tiles; // If the tilemap keeps the tiles in heap memory when deleted
    bool keep_entities;// If the tilemap keeps the entities in heap memory when deleted

    Tilemap();
    Tilemap(int width, int height);
    Tilemap(TextFunnel* _text_funnel, int _width, int _height);
    Tilemap(Player* player, int width, int height);
    Tilemap(TextFunnel* _text_funnel, Player* _player, int _width, int _height);
    ~Tilemap();

    std::vector<char> get_display();

    bool bound_check(int x, int y);

    void update_player();
    void update_all_entities();
    void fill_tilemap(Tile* tile);
    void assemble_tilemap();
    void add(Tile* tile, int x, int y);
    void add(Entity* entity, int x, int y);
    void move(Tile* tile, int x, int y);
    void move(Entity* entity, int x, int y);
    void remove(Tile* tile, bool deconstruct = false);
    void remove(Entity* entity, bool deconstruct = false);
    void move_player();
    void move_all_entities();
};