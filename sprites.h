
// Generated file - DO NOT EDIT!

#pragma once

class Sprite {
public:
    using OnEndAnimationCallback = void (*)(Sprite*);

    OnEndAnimationCallback onEndAnimation = +[](Sprite *sprite){ sprite->frame = 0; };
    const uint8_t *data = nullptr;
    std::uint32_t frameTime = 0;
    std::uint16_t frame = 0;
    std::uint16_t animation = 0;

    Sprite() = default;
    virtual ~Sprite(){}
    
    template<typename SpriteType>
    void play(const SpriteType& data, typename SpriteType::Animation animation){ 
        this->animation = static_cast<std::uint16_t>(animation); 
        this->data = reinterpret_cast<const uint8_t *>(&data);
        frame = 0;
        frameTime = Pokitto::Core::getTime() + getFrameDuration(0);
    }
    
    std::uint8_t getAnimationFrameCount(){ return data[ 2 + animation ]; }

    std::uint8_t getFrameId(std::uint32_t frame){ return data[ 2 + animation + 1 + frame * 2]; }

    std::uint32_t getFrameDuration(std::uint32_t frame){ return data[ 2 + animation + 1 + frame * 2 + 1 ] * 10; }
    
    std::uint32_t getFrameWidth(){ return getFrameBitmap(getFrameId(frame))[2]; }
    
    std::uint32_t getFrameHeight(){ return getFrameBitmap(getFrameId(frame))[3]; }

    const uint8_t *getFrameBitmap(std::uint32_t frameId){ 
        std::uint32_t offset = 2 + data[0] + (static_cast<std::uint32_t>(data[1]) << 8);
        uint32_t shift;
        switch(Pokitto::Display::m_colordepth){
        case 8: shift = 0; break;
        case 4: shift = 1; break;
        case 2: shift = 2; break;
        case 1: shift = 3; break;
        default: shift = 0; break;
        }
        
        while(frameId--){
            std::uint32_t w = data[offset + 2];
            std::uint32_t h = data[offset + 3];
            std::uint32_t total = w*h >> shift;
            offset += total + 4;
        }
        
        return data + offset;
    }

    void draw(std::uint32_t x, std::uint32_t y, bool flip=false, bool mirror=false, std::uint32_t recolor=0){
        if(!data) 
            return;
            
        std::uint32_t now = Pokitto::Core::getTime();
        while(true) {
            int32_t delta = now - frameTime;
            if( delta < 0 ){
                const uint8_t *bmp = getFrameBitmap(getFrameId(frame));
                x += *bmp++;
                y += *bmp++;

                #if PROJ_SCREENMODE == TASMODE
                if(Pokitto::Display::m_colordepth == 8){
                    Pokitto::Display::drawSprite(x, y, bmp, flip, mirror, recolor);
                    return;
                }
                #endif

                if(flip) Pokitto::Display::drawBitmapYFlipped(x, y, bmp);
                else if(mirror)  Pokitto::Display::drawBitmapXFlipped(x, y, bmp);
                else  Pokitto::Display::drawBitmap(x, y, bmp);
                return;
            }
            frame++;
            if( frame == getAnimationFrameCount() )
                onEndAnimation(this);
            frameTime += getFrameDuration(frame);
        }
    }
};

class Door {
public:
    enum Animation : std::uint16_t {
		locked = 0,
		unlocked = 3,
		open = 6
	};
};

class EnemyProgFill {
public:
    enum Animation : std::uint16_t {
		play = 0
	};
};

class EnemyVirus {
public:
    enum Animation : std::uint16_t {
		play = 0
	};
};

class HackIcons {
public:
    enum Animation : std::uint16_t {
		bUp = 0,
		bDown = 3,
		aUp = 6,
		aDown = 9,
		cUp = 12,
		cDown = 15,
		dUpUp = 18,
		dUpDown = 21,
		dLeftUp = 24,
		dLeftDown = 27,
		dRightUp = 30,
		dRightDown = 33,
		dDownUp = 36,
		dDownDown = 39,
		empty = 42
	};
};

class Hackme {
public:
    enum Animation : std::uint16_t {
		show = 0
	};
};

class Hero {
public:
    enum Animation : std::uint16_t {
		walkSouth = 0,
		walkNorth = 9,
		walkWest = 18,
		walkEast = 27,
		idleSouth = 36,
		idleNorth = 39,
		idleWest = 42,
		idleEast = 45
	};
};

class MinibotField {
public:
    enum Animation : std::uint16_t {
		idle = 0
	};
};

class ProgBar {
public:
    enum Animation : std::uint16_t {
		start = 0
	};
};

class ProgFill {
public:
    enum Animation : std::uint16_t {
		play = 0
	};
};

class Robot {
public:
    enum Animation : std::uint16_t {
		idle = 0
	};
};

class SecurityDrone {
public:
    enum Animation : std::uint16_t {
		idle = 0
	};
};

class Sentinal {
public:
    enum Animation : std::uint16_t {
		scanning = 0,
		warning = 17
	};
};

class ThreatLevel {
public:
    enum Animation : std::uint16_t {
		zero = 0,
		one = 3,
		two = 6,
		three = 9,
		four = 12,
		five = 15
	};
};


// https://stackoverflow.com/a/19591902
extern "C" {
extern const Door door;
extern const EnemyProgFill enemyProgFill;
extern const EnemyVirus enemyVirus;
extern const HackIcons hackIcons;
extern const Hackme hackme;
extern const Hero hero;
extern const MinibotField minibotField;
extern const ProgBar progBar;
extern const ProgFill progFill;
extern const Robot robot;
extern const SecurityDrone securityDrone;
extern const Sentinal sentinal;
extern const ThreatLevel threatLevel;
}
