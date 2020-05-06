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
            static bool checkUnlocked(int x, int y);
            
            static bool shouldEnd();
            static void setShouldEnd(bool should);
            
        private:
            static inline char* logs[35];
            static inline bool unlocked[35];
            static inline Sprite lock, logging, cursorIcon;
            static inline int logCursorX, logCursorY, cursorTimer = 0;
            
            static inline bool showLog = false, end = false, showCursor = true;
            
            // init button state mechanism
            static inline int btnPrevState = 0, btnJustPressed = 0;
            
        
    };
}