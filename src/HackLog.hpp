#pragma once
#include <string>
using namespace std; 

namespace HackLog {
    class HackLogManager{
        public:
            static void init();
            static string getLog(int id);
            static void unlockLog(int id);
        private:
            static inline string logs[32];
            static inline bool unlocked[32];
        
    };
}