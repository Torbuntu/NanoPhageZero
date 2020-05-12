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
    
    bool HackLogManager::checkUnlocked(int id){
        return id <= 28 ? unlocked[id] : false;
    }
    
    void HackLogManager::unlockLog(int id){
        unlocked[id] = true;
    }

    void HackLogManager::init(){
        lock.play(lockIcon, LockIcon::idle);
        logging.play(logIcon, LogIcon::idle);
        cursorIcon.play(cursor, Cursor::idle);
        //BEGIN Intro Level Logs
        logs[0] = "What happened?! My arm feels heavy and cold. My vision... I can't... see... What are these outlines? Everything is a light shape... I need to get out of here... I need to find the exit.";
        logs[1] = "The conveyor belt brings in scraps for sorting. My job was to collect valuables and put them in the crates for delivery. Green Scraps go for the highest prices, but this one... it exploded? I think?";
        logs[2] = "I feel drawn to these terminals. My arm... like a magnet... If I just touch them, I can tell them what I want them to do... I can hack them to find the escape... But I feel something else...";
        logs[3] = "These are the chips we process here at the facility. Normal chips like this go for a decent price, but the Green Scraps from the City Center are much more expensive. Nobody knows why though.";
        logs[4] = "These terminals are accessed by KEYCARD only. Usually we request keycards at a Minibot Field. These Fields are servers with micro robots that are programmed to run tasks for us.";
        logs[5] = "The minibot fields look strange. It seems they have been infected with the same virus as me... I will have to be careful how I program the minibots in order to bypass the wandering viruses.";
        logs[6] = "Each hack feels more intuitive than the last. Each time more enjoyable. My arm feels heavier however, and I can feel a slight pinch at my shoulder where it is growing heavier...";
        logs[7] = "The chips aren't supposed to leave the packaging room. This one is 3 levels outside of security clearance. Someone must have been up to something. This isn't right...";
        logs[8] = "This Minibot Field has remnants of code left over by a malicious hacker. What they were after though I cannot tell. The Minibot is still in here. Is this where they planted the virus?";
        
        logs[9] = "The facility has very powerful sentry drones that gaurd certain floors. When I look at them now they are glowing red... They must be infected with some virus... If I get close enough maybe I can hack them.";
        logs[10] = "The sentry drones have been compromised. Every time I try to get by they force me back to the enterance. The drones used to be around to protect the facility merchandise. I wonder what happened to them.";

        logs[11] = "These computer systems were typically run in a text terminal. It looks like these ones were all upgraded with a fancy GUI Desktop system. It doesn't change the underlying systems however, I can still hack.";
        logs[12] = "The tower lobby... I see they use the same tech as the reclemation facility. My arm is starting to ache. The Green Scrap in use here must be much stronger than the scraps at the facility. I must be close.";
        logs[13] = "This chip... it glows with an ominous red haze. My arm aches as I stare at it... Were they experimenting with the virus I found in the reclamation facility? Did they create the virus? What's going on?";
        
        logs[14] = "These Minibot Fields are the same as the ones in the reclamation facility. I didn't think the City Center used the same tech as we did. Unless This is where the tech comes from. I must be in the right place.";
        
        logs[15] = "";
        logs[16] = "";
        logs[17] = "";
        logs[18] = "";
        logs[19] = "";
        logs[20] = "";
        logs[21] = "";
        // logs[9] = "Someone must have been experimenting in here with some of the reclaimed electronics. Could that have been the cause of the Minibot Fields being infected?";
        // // END Intro Level Logs
        
        // //Lobby Logs
        // logs[7] = 
        // logs[8] = 
        // logs[10] = 
        // //END Lobby Logs
        
        // //LVL1 Logs
        // logs[11] = "It is weird that there aren't any employees around. So far all I've run into are some infected sentry drones... I wonder what has happened to everyone. Everything is locked down, so someone must be here.";
        // logs[12] = "Each hack feels more and more natural. I am starting to enjoy it... The pain in my arm is starting to feel like a comfort. I still don't quite understand what is happening to me...";
        // logs[13] = "This system has encrypted files talking about a strange VIRUS... or PHAGE... The logs mention Green Scraps. but it seems like someone has been trying to tamper with or delete these files... strange.";

        // logs[14] = "I keep finding computer chips that I don't understand. I've never seen these before at the facility. The scrap tech in me is very confused, but for some reason my humanity is drawn to these chips...";
        // logs[15] = "I can feel someone watching me on the other side of these network connections. Some... evil is trying to make sure that I know they are there. Is this... the virus? Is it guiding me?";
        // logs[16] = "Looking at these two chips I can feel a struggle between them. It is as if they aren't even actually computer chips, but something... more familiar. The infected chip glows violently as I approach it.";
        
        // logs[17] = "These systems aren't designed to be interfaced this way. Something has been changing their source remotely. They feel as if they are... alive...";
        // logs[18] = "";
        // logs[19] = "";
        // logs[20] = "";
        // logs[21] = "";
        
        
        // logs[22] = "";
        // logs[23] = "";
        // logs[24] = "";
        // logs[25] = "";
        // logs[26] = "";
        // logs[27] = "";
        // logs[28] = "";

        for(int i = 0; i < 28; ++i){
            unlocked[i] = false;
        }
    }
    
    void HackLogManager::update(){
        using Pokitto::Buttons;
        Buttons::pollButtons();
        btnJustPressed = Buttons::buttons_state & (~btnPrevState);
        btnPrevState = Buttons::buttons_state;

        if(logCursorY < 3){
            if(btnJustPressed == B_RIGHT){
                if(logCursorX < 6) logCursorX++;
            }
            if(btnJustPressed == B_LEFT){
                if(logCursorX > 0) logCursorX--;
            }
        }
        if(btnJustPressed == B_DOWN){
            if(logCursorY < 3) logCursorY++;
        }
        if(btnJustPressed == B_UP){
            if(logCursorY > 0) logCursorY--;
        }
        
        if(logCursorY == 3){
            if(btnJustPressed == B_LEFT){
                if(speed == 15) speed = 5;
                else if(speed == 25) speed = 15;
            }
            if(btnJustPressed == B_RIGHT){
                if(speed == 5) speed = 15;
                else if(speed == 15) speed = 25;
            }
        }
        
            
        cursorTimer++;
        if(cursorTimer > 15) {
            showCursor = !showCursor;
            cursorTimer = 0;
        }
        
        if(btnJustPressed == B_A && logCursorY < 3){
            if(unlocked[logCursorX + logCursorY * 7]){
                showLog = true;
            }
        }
        
        if(!showLog && btnJustPressed == B_B){
            end = true;
        }
        
        if(enterCooldown > 0)enterCooldown--;
        if(btnJustPressed == B_C && enterCooldown == 0){
            playMusic = !playMusic;
        }
    }
    
    void HackLogManager::render(){
        using Pokitto::UI;
        UI::clear();
        UI::showTileMapUISprites();
        UI::drawBox(1, 1, 32, 24);
        UI::setCursorBoundingBox(2, 2, 31, 23);
        UI::setCursor(2, 2);
        
        UI::printText("Hack Log: ");
        
        UI::setCursorBoundingBox(2, 32-16, 31, 23);
        UI::setCursor(2, 32-12);
        
        if(speed == 25) UI::printText("  <> Minibot Speed: Slow");
        if(speed == 15) UI::printText("  <> Minibot Speed: Default");
        if(speed == 5) UI::printText("  <> Minibot Speed: Fast");
        
        UI::setCursorBoundingBox(2, 32-10, 31, 23);
        UI::setCursor(2, 32-8);
        UI::printText("> A read, B close/exit\n");
        
        if(playMusic)UI::printText("> C to toggle music: true");
        else UI::printText("> C to toggle music: false");
        if(!showLog){
            for(int x = 0; x < width; ++x){
                for(int y = 0; y < height; ++y){
                    if(unlocked[x + y * 7]){
                        logging.draw(23 + x * 24, 25 + y * 24);
                    }else {
                        lock.draw(20 + x * 24, 24 + y * 24);
                    }
                }
            }
            if(showCursor) {
                if(logCursorY < 3) cursorIcon.draw(17 + logCursorX * 24, 30 + logCursorY * 24);
                else cursorIcon.draw(13 + logCursorX * 24, 48 + logCursorY * 24);
            }
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
        enterCooldown = 5;
        end = should;
    }
    
    bool HackLogManager::shouldEnd(){
        return end;
    }
    
    bool HackLogManager::getPlayMusic(){
        return playMusic;
    }
    
    void HackLogManager::animateLog(int pos){
        logging.draw(16, pos);
    }
    
    bool HackLogManager::cleared(){
        int full = 0;
        for(int i = 0; i < 28; ++i){
            if(unlocked[i])full++;
        }
        return full == 28;
    }
    
    int HackLogManager::getSpeed(){
        return speed;
    }
        
}