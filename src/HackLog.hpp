#pragma once
#include "Pokitto.h"
#include "sprites.h"

using namespace std; 

namespace HackLog {
    class HackLogManager{
        public:
            static void init();
            static char* getLog(int id);
            static char* getLog(int x, int y);
            static void unlockLog(int id);
            static bool checkUnlocked(int x, int y);
            static void render(int x, int y);
        private:
            static inline char* logs[35];
            static inline bool unlocked[35];
            static inline Sprite lock, logging;
        
    };
}