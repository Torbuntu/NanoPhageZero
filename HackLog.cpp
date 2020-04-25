#include "HackLog.hpp"
#include <string>
using namespace std; 


namespace HackLog {
    string HackLogManager::getLog(int id){
        if(unlocked[id]){
            return logs[id];
        }else {
            return "No log found...";
        }
    }
    void HackLogManager::unlockLog(int id){
        unlocked[id] = true;
    }
    
    void HackLogManager::init(){
        //logs
        logs[0] = "I tried to hack the exit. But they were watching... If I could only disable the watchers first.";
        
        for(int i = 0; i < 32; ++i){
            unlocked[i] = false;
        }
    }
        
}