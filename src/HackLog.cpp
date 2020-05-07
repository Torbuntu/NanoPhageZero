#include <Pokitto.h>

#include "HackLog.hpp"
#include "sprites.h"

#include "src/structs.h"
#include "src/ButtonMaps.h"

#include <tasui>
#include <puits_UltimateUtopia.h>

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

    void HackLogManager::init(){
        lock.play(lockIcon, LockIcon::idle);
        logging.play(logIcon, LogIcon::idle);
        cursorIcon.play(cursor, Cursor::idle);
        //logs
        logs[0] = "What happened?! My arm feels heavy and cold and my vision... I can't... see... What are these outlines? I need to get out of here...";
        logs[1] = "I feel drawn to these terminals. My arm... like a magnet... If I just touch them, it is like I can tell whem what I want them to do... I must... hack them.";
        logs[2] = "The minibot fields look strange. It seems they have been infected with some virus... I will have to be careful with how I program the minibot in order to bypass the wandering viruses.";
        logs[3] = "The sentry drones have been compromised. Every time I try to get by they force me back to the enterance. The drones used to be around to protect the facility merchandise. I wonder what happened to them.";
        logs[4] = "The conveyor belt brings in scraps for sorting. My job was to collect valuables and put them in the crates for delivery. Green Scraps go for the highest prices, how was I to know I would get infected...";
        logs[5] = "These minibots are used to run tasks on the processing facility servers. They aren't that useful yet, but I can program them for printing keycards to access locked terminals.";
        logs[6] = "The facility has very powerful sentry drones that gaurd certain floors. When I look at them now they are glowing red... They must be infected with some virus... I should hack the control system.";
        logs[7] = "The tower lobby... I see they use the same tech as the reclemation facility. My arm is starting to ache. The Green Scrap in use here must be much stronger than the scraps at the facility. I must be close.";
        
        for(int i = 0; i < 35; ++i){
            unlocked[i] = false;
        }
        unlockLog(0);
    }
    
    void HackLogManager::update(){
        using Pokitto::Buttons;
        Buttons::pollButtons();
        btnJustPressed = Buttons::buttons_state & (~btnPrevState);
        btnPrevState = Buttons::buttons_state;
        
        
        if(btnJustPressed == B_RIGHT){
            if(logCursorX < 6) logCursorX++;
        }
        if(btnJustPressed == B_LEFT){
            if(logCursorX > 0) logCursorX--;
        }
        if(btnJustPressed == B_DOWN){
            if(logCursorY < 4) logCursorY++;
        }
        if(btnJustPressed == B_UP){
            if(logCursorY > 0) logCursorY--;
        }
            
        cursorTimer++;
        if(cursorTimer > 15) {
            showCursor = !showCursor;
            cursorTimer = 0;
        }
        
        if(btnJustPressed == B_A){
            if(unlocked[logCursorX + logCursorY * 7]){
                showLog = true;
            }
        }
        
        if(!showLog && btnJustPressed == B_B){
            end = true;
        }
        
    }
    
    void HackLogManager::render(){
        using Pokitto::UI;
        UI::clear();
        UI::showTileMapUISprites();
        UI::drawBox(1, 1, 32, 24);
        UI::setCursorBoundingBox(2, 2, 32 - 1, 24 - 1);
        UI::setCursor(2, 2);
        UI::setCursorDelta(UIVariants::standard);
        UI::printString("Hack Log: ");
        
        if(!showLog){
            for(int x = 0; x < 7; ++x){
                for(int y = 0; y < 5; ++y){
                    if(unlocked[x + y * 7]){
                        logging.draw(23 + x * 24, 25 + y * 24);
                    }else {
                        lock.draw(20 + x * 24, 24 + y * 24);
                    }
                }
            }
            if(showCursor) cursorIcon.draw(17 + logCursorX * 24, 30 + logCursorY * 24);
        }else {
            if(btnJustPressed == B_B) showLog = false;
            UI::drawBox(4, 4, 28, 20);
            UI::setCursorBoundingBox(6, 6, 28 - 1, 20 - 1);
            UI::setCursor(6, 6);
            UI::setCursorDelta(UIVariants::standard);
            UI::printText(HackLogManager::getLog(logCursorX, logCursorY));
        }
    }
    
    void HackLogManager::setShouldEnd(bool should){
        logCursorX = 0;
        logCursorY = 0;
        end = should;
    }
    
    bool HackLogManager::shouldEnd(){
        return end;
    }
        
}