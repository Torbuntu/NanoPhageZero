#include "HackLog.hpp"
#include "sprites.h"

using namespace std; 


namespace HackLog {
    char* HackLogManager::getLog(int id){
        if(unlocked[id]){
            return logs[id];
        }else {
            return "You have not unlocked this log.";
        }
    }
    
    char* HackLogManager::getLog(int x, int y){
        return getLog(x + y * 7);
    }
    
    void HackLogManager::unlockLog(int id){
        unlocked[id] = true;
    }
    
    bool HackLogManager::checkUnlocked(int x, int y){
        return unlocked[x + y * 7];
    }
    
    void HackLogManager::init(){
        lock.play(lockIcon, LockIcon::idle);
        logging.play(logIcon, LogIcon::idle);
        //logs
        logs[0] = "What happened?! My arm feels heavy and cold and my vision... I can't... see... What are these outlines? I need to get out of here...";
        
        for(int i = 0; i < 35; ++i){
            unlocked[i] = false;
        }
        unlockLog(0);
    }
    
    void HackLogManager::render(int x, int y){
        if(unlocked[x + y * 7]){
            logging.draw(23 + x * 24, 25 + y * 24);
        }else {
            lock.draw(20 + x * 24, 24 + y * 24);
        }
    }
        
}