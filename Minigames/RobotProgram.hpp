#pragma once

#include <Pokitto.h>
#include "sprites.h"
#include "maps.h"

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
            
            static int getRobotX();
            
            static int getRobotY();
            
            static void setRobotX(int x);
            
            static void setRobotY(int y);
            
        private:
        
            static inline Sprite icons, robo;
            
            static inline int program[13];
            
            static inline int length, roboX, roboY, inventory, step, speed;
            
            static inline RoboState roboState = RoboState::READY;
            
            // init button state mechanism
            static inline int buttonsPreviousState = 0;
            
            static inline int buttonsJustPressed = 0;
            
            static inline bool end = false; 
        
    };
    
    
}