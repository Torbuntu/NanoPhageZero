#pragma once

#include <Pokitto.h>
#include <Tilemap.hpp>

namespace Level{
    class LevelManager{
        public:
            static void setMap(Tilemap map);
            
            static void update();
            
            static void render(int x, int y);
            
        private:
        
    };
}