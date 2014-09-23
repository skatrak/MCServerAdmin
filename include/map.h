/*
 * This file is part of MCServerAdmin.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Copyright (c) 2014 Sergio M. Afonso Fumero <theSkatrak@gmail.com>
 */

#ifndef MAP_H
#define MAP_H

#include <QDataStream>
#include <QIODevice>
#include <QMap>
#include <QString>

#include "server.h"

enum GameDifficulty {
  PEACEFUL = 0,
  EASY     = 1,
  NORMAL   = 2,
  HARD     = 3
};

enum GameMode {
  SURVIVAL  = 0,
  CREATIVE  = 1,
  ADVENTURE = 2,
  SPECTATOR = 3
};

enum LevelType {
  DEFAULT,
  SUPERFLAT,
  LARGE_BIOMES,
  AMPLIFIED
};

const QMap<QString, GameDifficulty> DIFFICULTY_NAMES = {
  { "0. " + QObject::tr("Peaceful"), PEACEFUL },
  { "1. " + QObject::tr("Easy"),     EASY     },
  { "2. " + QObject::tr("Normal"),   NORMAL   },
  { "3. " + QObject::tr("Hard"),     HARD     }
};

const QMap<QString, GameMode> GAME_MODE_NAMES {
  { "0. " + QObject::tr("Survival"),  SURVIVAL  },
  { "1. " + QObject::tr("Creative"),  CREATIVE  },
  { "2. " + QObject::tr("Adventure"), ADVENTURE },
  { "3. " + QObject::tr("Spectator"), SPECTATOR }
};

const QMap<QString, LevelType> LEVEL_TYPE_NAMES {
  { "0. " + QObject::tr("Default"),      DEFAULT      },
  { "1. " + QObject::tr("Superflat"),    SUPERFLAT    },
  { "2. " + QObject::tr("Large biomes"), LARGE_BIOMES },
  { "3. " + QObject::tr("Amplified"),    AMPLIFIED    }
};

class Map {
public:
  // Constructors and destructor
  Map(Server* server, QString name = "world");
  Map(const Map& m);
  ~Map();

  // Methods of the class
  bool createServerProperties(QIODevice& file) const;
  Map& operator= (const Map& other);

  // Accessors and mutators
  Server* getServer() const { return server; }
  void setServer(Server* value) {
    if (server)
      server->removeMap(this);
    server = value;
    if (server)
      server->addMap(this);
  }

  QString getName() const { return name; }
  void setName(const QString &value) {
    name = value;
  }

  QString getResourcePack() const { return resource_pack; }
  void setResourcePack(const QString &value) {
    resource_pack = value;
  }

  unsigned getMaxPlayers() const { return max_players; }
  void setMaxPlayers(unsigned value) {
    max_players = value;
  }

  bool getCommandBlock() const { return command_block; }
  void setCommandBlock(bool value) {
    command_block = value;
  }

  GameDifficulty getDifficulty() const { return difficulty; }
  void setDifficulty(GameDifficulty value) {
    difficulty = value;
  }

  GameMode getGameMode() const { return game_mode; }
  void setGameMode(GameMode value) {
    game_mode = value;
  }

  bool isGameModeForced() const { return force_game_mode; }
  void setGameModeForced(bool value) {
    force_game_mode = value;
  }

  bool isHardcore() const { return hardcore; }
  void setHardcore(bool value) {
    hardcore = value;
  }

  int getIdleTimeout() const { return idle_timeout; }
  void setIdleTimeout(int value) {
    idle_timeout = value;
  }

  int getViewDistance() const { return view_distance; }
  void setViewDistance(int value) {
    view_distance = value;
  }

  bool isFlightAllowed() const { return allow_flight; }
  void setFlightAllowed(bool value) {
    allow_flight = value;
  }

  bool isNetherAllowed() const { return allow_nether; }
  void setNetherAllowed(bool value) {
    allow_nether = value;
  }

  bool isPVP() const { return pvp; }
  void setPVP(bool value) {
    pvp = value;
  }

  QString getSeed() const { return seed; }
  void setSeed(const QString &value) {
    seed = value;
  }

  LevelType getLevelType() const { return level_type; }
  void setLevelType(LevelType value) {
    level_type = value;
  }

  int getMaxBuildHeight() const { return max_build_height; }
  void setMaxBuildHeight(int value) {
    max_build_height = value;
  }

  bool doStructuresGenerate() const { return generate_structures; }
  void setStructuresGenerate(bool value) {
    generate_structures = value;
  }

  int getSpawnProtectionRadius() const { return spawn_protection_radius; }
  void setSpawnProtectionRadius(int value) {
    spawn_protection_radius = value;
  }

  bool doAnimalsSpawn() const { return spawn_animals; }
  void setAnimalsSpawn(bool value) {
    spawn_animals = value;
  }

  bool doMonstersSpawn() const { return spawn_monsters; }
  void setMonstersSpawn(bool value) {
    spawn_monsters = value;
  }

  bool doNpcsSpawn() const { return spawn_npcs; }
  void setNpcsSpawn(bool value) {
    spawn_npcs = value;
  }

protected:
  static const unsigned DEFAULT_MAX_PLAYERS = 20;
  static const bool DEFAULT_COMMAND_BLOCK = false;
  static const GameDifficulty DEFAULT_DIFFICULTY = EASY;
  static const GameMode DEFAULT_GAME_MODE = SURVIVAL;
  static const bool DEFAULT_FORCE_GAME_MODE = false;
  static const bool DEFAULT_HARDCORE = false;
  static const int DEFAULT_IDLE_TIMEOUT = 0;
  static const int DEFAULT_VIEW_DISTANCE = 10;
  static const bool DEFAULT_FLIGHT = false;
  static const bool DEFAULT_NETHER = true;
  static const bool DEFAULT_PVP = true;
  static const LevelType DEFAULT_LEVEL_TYPE = DEFAULT;
  static const int DEFAULT_MAX_BUILD_HEIGHT = 256;
  static const bool DEFAULT_GENERATE_STRUCTURES = true;
  static const int DEFAULT_SPAWN_PROTECTION_RADIUS = 16;
  static const bool DEFAULT_SPAWN_ANIMALS = true;
  static const bool DEFAULT_SPAWN_MONSTERS = true;
  static const bool DEFAULT_SPAWN_NPCS = true;

private:
  // Reference to its server
  Server* server;

  // General
  QString name;
  QString resource_pack;
  unsigned max_players;
  bool command_block;

  // Gameplay
  GameDifficulty difficulty;
  GameMode game_mode;
  bool force_game_mode;
  bool hardcore;
  int idle_timeout;
  int view_distance;
  bool allow_flight;
  bool allow_nether;
  bool pvp;

  // Map generation
  QString seed;
  LevelType level_type;
  /* TODO Superflat generator settings */
  int max_build_height;
  bool generate_structures;
  int spawn_protection_radius;
  bool spawn_animals;
  bool spawn_monsters;
  bool spawn_npcs;

};

// Methods for the class's serialization
QDataStream& operator<<(QDataStream&, const Map&);
QDataStream& operator>>(QDataStream&, Map&);

#endif // MAP_H
