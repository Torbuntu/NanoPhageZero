#pragma once

#include <Pokitto.h>
#include <Tilemap.hpp>
#include <tasui>
#include <puits_UltimateUtopia.h>

#include "src/structs.h"

#include "src/Minigames/Sequence.hpp"
#include "src/Minigames/Bruteforce.hpp"
#include "src/Minigames/RobotProgram.hpp"
#include "src/Minigames/BossBattle.hpp"

#include "src/Level.hpp"
#include "src/Player.hpp"
#include "src/HackLog.hpp"
#include "src/ButtonMaps.h"

#include "sprites.h"
#include "maps.h"

// Pokitto utils
using Pokitto::Core;
using Pokitto::Display;
using Pokitto::Buttons;
using Pokitto::Sound;
using Pokitto::UI;

// Minigames
using Sequence::SeqHack;
using Bruteforce::BruteHack;
using RobotProgram::RoboHack;

using BossBattle::BossFight;

// Managers
using Level::LevelManager;
using Player::PlayerManager;
using HackLog::HackLogManager;