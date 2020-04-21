#pragma once

#include <Pokitto.h>
#include <Tilemap.hpp>

namespace Level{
    class LevelManager{
        public:
            static void setMap(const uint8_t* map);
            
            static void setMap();
            
            static void addLvl();
            
            static void setLvl(int lvl);
            
            static int getLvl();
            
            static void init();
            
            static void update();
            
            static void render(int x, int y);
            
        private:
        
            static inline Tilemap tilemap;
            
            static inline int level;
        
    };
}