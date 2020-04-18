
#include <Pokitto.h>
#include <ButtonMaps.h>
#include "sprites.h"
#include "Minigames/RobotProgram.hpp"

namespace RobotProgram{
    
    void RoboHack::init(int len, int robX, int robY){
        icons.play(hackIcons, HackIcons::aUp);
        robo.play(robot, Robot::idle);
        length = len;
        roboX = robX;
        roboY = robY;
        step = 0;
        inventory = 0;
        
        for(int i = 0; i < length; ++i){
            program[i] = 10;
        }
    }
    
    void RoboHack::update(){
        using Pokitto::Buttons;
        using Pokitto::Display;
        // Poll the button
        Buttons::pollButtons();
        buttonsJustPressed = Buttons::buttons_state & (~buttonsPreviousState);
        buttonsPreviousState = Buttons::buttons_state;
        
        if( buttonsJustPressed != 0 && step >= length ){
            roboState = RoboState::RUNNING;
        }
        
        
        switch(roboState){
            case READY:
                Display::print("Press C to program robot.");
                if( buttonsJustPressed == B_C ){
                    roboState = RoboState::PROGRAMMING;
                }
            
            break;
            case PROGRAMMING:
                if( buttonsJustPressed != 0 && step < length ){
                    program[step] = buttonsJustPressed;
                    step++;
                }
            break;
            case RUNNING:
            
            break;
            
            case COMPLETE:
            
            break;
        }
    }
    
    void RoboHack::render(){
        
        for(int i = 0; i < length; ++i){
            switch(program[i]){
                case B_A:
                    icons.play(hackIcons, HackIcons::aUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_B:
                    icons.play(hackIcons, HackIcons::bUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_C:
                    icons.play(hackIcons, HackIcons::cUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_UP:
                    icons.play(hackIcons, HackIcons::dUpUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_DOWN:
                    icons.play(hackIcons,  HackIcons::dDownUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_LEFT:
                    icons.play(hackIcons, HackIcons::dLeftUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                case B_RIGHT:
                    icons.play(hackIcons,  HackIcons::dRightUp);
                    icons.draw(8 + i * 16, 32);
                    break;
                default:
                    icons.play(hackIcons, HackIcons::empty);
                    icons.draw(8 + i * 16, 32);
            }
            
        }
        
        robo.draw(roboX, roboY);
    }
    
    
    
}