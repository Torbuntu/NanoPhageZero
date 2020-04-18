#pragma once

#include <Pokitto.h>
#include "sprites.h"
namespace RobotProgram{
    class RoboHack{
        public:
            static void init();
            
            static void update();
            
            static void render();
            
            static bool complete();
            
            static bool fail();
            
        private:
        
            static inline Sprite icons;
            
            static inline int program[13];
        
    };
}