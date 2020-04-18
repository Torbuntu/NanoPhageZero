#pragma once

#include <Pokitto.h>
#include "sprites.h"
namespace RobotProgram{
    enum RoboState{
        READY, PROGRAMMING, RUNNING, COMPLETE
    };
    
    class RoboHack{
        public:
            static void init(int len, int robX, int robY);
            
            static void update();
            
            static void render();
            
            static bool complete();
            
            static bool fail();
            
        private:
        
            static inline Sprite icons, robo;
            
            static inline int program[13];
            
            static inline int length, roboX, roboY, inventory, step;
            
            static inline RoboState roboState = RoboState::READY;
            
            // init button state mechanism
            static inline int buttonsPreviousState = 0;
            
            static inline int buttonsJustPressed = 0;
        
    };
    
    
}