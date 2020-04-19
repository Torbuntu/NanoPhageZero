
#include <Pokitto.h>
#include <Tilemap.hpp>
#include <ButtonMaps.h>
#include "sprites.h"
#include "Minigames/RobotProgram.hpp"
#include "maps.h"

namespace RobotProgram{
    
    void RoboHack::restart(){
        roboX = -102;
        roboY = -80;
        step = 0;
        
        renderX = LCDWIDTH/2-8;
        renderY = LCDHEIGHT/2-8;
        inventory = 0;
        for(int i = 0; i < length; ++i){
            program[i] = 10;
        }
    }

    void RoboHack::init(int len){
        icons.play(hackIcons, HackIcons::aUp);
        robo.play(robot, Robot::idle);
        length = len;
        end = false;
        speed = 15;
        
        //0 = 14, 1 = 11, suburb+2 = mapPixelData
        tilemap.set(minibotfield[0], minibotfield[1], minibotfield+2);
        for(int i=0; i<sizeof(tiles)/(POK_TILE_W*POK_TILE_H); i++){
            tilemap.setTile(i, POK_TILE_W, POK_TILE_H, tiles+i*POK_TILE_W*POK_TILE_H);
        }
        
        RoboHack::restart();
        roboState = RoboState::READY;
    }
    
    void RoboHack::update(){
        using Pokitto::Buttons;
        using Pokitto::Display;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        int tileX = (roboX + renderX + PROJ_TILE_W/2) / PROJ_TILE_W;
        int tileY = (roboY + renderY) / PROJ_TILE_H;
        auto tile = minibotfieldEnum(tileX, tileY);
        
        
        switch(roboState){
            case READY:
                Display::print("> Press C to program robot");
                if( buttonsJustPressed == B_C ){
                    roboState = RoboState::PROGRAMMING;
                }
            
            break;
            case PROGRAMMING:
                Display::print("> Programming...");
                if( buttonsJustPressed != 0 && step < length ){
                    program[step] = buttonsJustPressed;
                    step++;
                }
                
                if(step == length){
                    step = 0;
                    roboState = RoboState::RUNNING;
                }
            break;
            case RUNNING:
                Display::print("> Running...");
                speed--;
                
                if(speed == 0){
                    speed = 15;
                    if(step < length){
                        switch(program[step]){
                            case B_LEFT:
                                roboX -= 16;
                                if(roboX < -102){
                                    roboX = -102;
                                }
                            break;
                            case B_RIGHT:
                                roboX += 16;
                            break;
                            case B_UP:
                                roboY -= 16;
                                if(roboY < -80){
                                    roboY = -80;
                                }
                            break;
                            case B_DOWN:
                                roboY += 16;
                                if(roboY > 80){
                                    roboY = 80;
                                }
                            break;
                        }
                        step++;
                    }
                }
                
                if(step == length){
                    step = 0;
                    roboState = RoboState::COMPLETE;
                }
            
            break;
            case COMPLETE:
                Display::print("> C to restart. B to end");
                if( buttonsJustPressed == B_B ){
                    if(tile == Button){
                        end = true;
                    }
                }
                
                if( buttonsJustPressed == B_C){
                    RoboHack::restart();
                    roboState = RoboState::PROGRAMMING;
                }
            break;
        }
        
        
    }
    
    void RoboHack::render(){
        for(int i = 0; i < length; ++i){
            switch(program[i]){
                case B_A:
                    icons.play(hackIcons, HackIcons::aUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_B:
                    icons.play(hackIcons, HackIcons::bUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_C:
                    icons.play(hackIcons, HackIcons::cUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_UP:
                    icons.play(hackIcons, HackIcons::dUpUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_DOWN:
                    icons.play(hackIcons,  HackIcons::dDownUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_LEFT:
                    icons.play(hackIcons, HackIcons::dLeftUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                case B_RIGHT:
                    icons.play(hackIcons,  HackIcons::dRightUp);
                    icons.draw(8 + i * 16, 16);
                    break;
                default:
                    icons.play(hackIcons, HackIcons::empty);
                    icons.draw(8 + i * 16, 16);
            }
        }
        if(roboState == RoboState::RUNNING){
            robo.draw(8+step*16, 32);
        }
        
        
        
        robo.draw(renderX, renderY);
    }
    
    bool RoboHack::complete(){
        return end;   
    }
    
    int RoboHack::getRobotX(){
        return roboX;
    }
    
    int RoboHack::getRobotY(){
        return roboY;
    }
    
    void RoboHack::setRobotX(int x){
        roboX = x;
    }
    
    void RoboHack::setRobotY(int y){
        roboY = y;
    }
    
}