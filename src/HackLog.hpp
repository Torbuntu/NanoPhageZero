#pragma once
#include "Pokitto.h"
#include "sprites.h"

using namespace std; 

namespace HackLog {
    class HackLogManager{
        public:
            static void init();
            static void update();
            static void render();
            
            static char* getLog(int id);
            static char* getLog(int x, int y);
            static void unlockLog(int id);
            static bool checkUnlocked(int id);
            
            static bool shouldEnd();
            static void setShouldEnd(bool should);
            
            static bool getPlayMusic();
            static void animateLog(int pos);
            
            static bool cleared();
            
            static int getSpeed();
            
        private:
            static inline char* logs[22];
            static inline bool unlocked[22];
            static inline Sprite lock, logging, cursorIcon;
            static inline int logCursorX, logCursorY, cursorTimer = 0, enterCooldown = 5;
            static inline int width = 7, height = 3, speed = 15;
            
            static inline bool showLog = false, end = false, showCursor = true, playMusic = true;
            
            // init button state mechanism
            static inline int btnPrevState = 0, btnJustPressed = 0;
            
    };
}