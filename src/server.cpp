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

#include "server.h"

#include "map.h"

#include <QStringList>

Server::Server(const QString nm)
  : name(nm), motd("A Minecraft Server"), port(DEFAULT_PORT), online(DEFAULT_ONLINE),
    announce_achievements(DEFAULT_ANNOUNCE_ACHIEVEMENTS), snooper(DEFAULT_SNOOPER),
    white_list(DEFAULT_WHITE_LIST), op_permission_level(DEFAULT_OP_PERMISSION_LEVEL),
    query(DEFAULT_QUERY), query_port(DEFAULT_QUERY_PORT), rcon(DEFAULT_RCON),
    rcon_port(DEFAULT_RCON_PORT) {
  address[0] = address[1] = address[2] = address[3] = 0;
}

Server::Server(const Server& other) {
  name = other.name;
  motd = other.motd;
  setIPAddress(other.address);
  port = other.port;
  online = other.online;
  announce_achievements = other.announce_achievements;
  snooper = other.snooper;
  white_list = other.white_list;
  op_permission_level = other.op_permission_level;
  query = other.query;
  query_port = other.query_port;
  rcon = other.rcon;
  rcon_password = other.rcon_password;
  rcon_port = other.rcon_port;
}

Server::~Server() {
  QSet<Map*>::iterator i = maps.begin();
   while (i != maps.end()) {
     delete *i; // El destructor invoca a removeMap()
     i = maps.begin();
   }
}

void Server::addMap(Map* map) {
  if (map)
    maps.insert(map);
}

void Server::removeMap(Map* map) {
  maps.remove(map);
}

Server& Server::operator= (const Server& other) {
  name = other.name;
  motd = other.motd;
  setIPAddress(other.address);
  port = other.port;
  online = other.online;
  announce_achievements = other.announce_achievements;
  snooper = other.snooper;
  white_list = other.white_list;
  op_permission_level = other.op_permission_level;
  query = other.query;
  query_port = other.query_port;
  rcon = other.rcon;
  rcon_password = other.rcon_password;
  rcon_port = other.rcon_port;
  return *this;
}

QDataStream& operator<<(QDataStream& ds, const Server& s) {
  const quint8* address = s.getIPAddress();
  ds << s.getName() << s.getMessageOfTheDay() << address[0] << address[1] << address[2]
     << address[3] << s.getPort() << s.isOnline() << s.areAchievementsAnnounced()
     << s.isSnooperEnabled() << s.hasWhiteList() << s.getOpPermissionLevel()
     << s.isQueryEnabled() << s.getQueryPort() << s.isRemoteConsoleEnabled()
     << s.getRconPassword() << s.getRconPort();

  QSet<Map*> maps = s.getMaps();
  ds << maps.size();
  for (QSet<Map*>::const_iterator i = maps.begin(); i != maps.end(); ++i)
    ds << **i;

  return ds;
}

QDataStream& operator>>(QDataStream& ds, Server& s) {
  QString sBuffer;
  bool bBuffer;
  int op_permission;
  quint8 address[4];
  quint16 port;

  ds >> sBuffer;
  s.setName(sBuffer);
  ds >> sBuffer;
  s.setMessageOfTheDay(sBuffer);

  for (int i = 0; i < 4; ++i)
    ds >> address[i];
  s.setIPAddress(address);
  ds >> port;
  s.setPort(port);

  ds >> bBuffer;
  s.setOnline(bBuffer);
  ds >> bBuffer;
  s.setAchievementsAnnounced(bBuffer);
  ds >> bBuffer;
  s.setSnooperEnabled(bBuffer);
  ds >> bBuffer;
  s.setWhiteList(bBuffer);
  ds >> op_permission;
  s.setOpPermissionLevel(op_permission);

  ds >> bBuffer;
  s.setQueryEnabled(bBuffer);
  ds >> port;
  s.setQueryPort(port);
  ds >> bBuffer;
  s.setRemoteConsoleEnabled(bBuffer);
  ds >> sBuffer;
  s.setRconPassword(sBuffer);
  ds >> port;
  s.setRconPort(port);

  int maps;
  ds >> maps;
  for (int i = 0; i < maps; ++i) {
    Map* map = new Map(&s);
    ds >> *map;
  }

  return ds;
}

// Utility function to avoid writing duplicate code
inline bool stringToBool(QString str, bool* format_ok) {
  bool val;
  if (str == "true")
    val = true;
  else if (str == "false")
    val = false;
  else {
    if (format_ok) *format_ok = false;
    return false;
  }

  if (format_ok) *format_ok = true;
  return val;
}

Server* Server::fromServerProperties(const QMap<QString, QString>& properties) {
  Server* server = new Server;
  Map* map = new Map(server);

  for (QMap<QString, QString>::const_iterator i = properties.cbegin(); i != properties.cend(); ++i) {
    QString l_key = i.key().toLower();
    QString l_value = i.value().toLower();
    bool format_ok, bool_val;
    int int_val;

    if (l_key == "allow-flight") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setFlightAllowed(bool_val);
    }
    else if (l_key == "allow-nether") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setNetherAllowed(bool_val);
    }
    else if (l_key == "announce-player-achievements") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setAchievementsAnnounced(bool_val);
    }
    else if (l_key == "difficulty") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok && int_val >= PEACEFUL && int_val <= HARD)
        map->setDifficulty((GameDifficulty) int_val);
    }
    else if (l_key == "enable-query") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setQueryEnabled(bool_val);
    }
    else if (l_key == "enable-rcon") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setRemoteConsoleEnabled(bool_val);
    }
    else if (l_key == "enable-command-block") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setCommandBlock(bool_val);
    }
    else if (l_key == "force-gamemode") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setGameModeForced(bool_val);
    }
    else if (l_key == "gamemode") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok && int_val >= SURVIVAL && int_val <= SPECTATOR)
        map->setGameMode((GameMode) int_val);
    }
    else if (l_key == "generate-structures") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setStructuresGenerate(bool_val);
    }
    else if (l_key == "generator-settings")
      ; // TODO generator settings
    else if (l_key == "hardcore") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setHardcore(bool_val);
    }
    else if (l_key == "level-name")
      map->setName(i.value());
    else if (l_key == "level-seed")
      map->setSeed(i.value());
    else if (l_key == "level-type") {
      if (l_value == "default")
        map->setLevelType(DEFAULT);
      else if (l_value == "superflat")
        map->setLevelType(SUPERFLAT);
      else if (l_value == "largebiomes")
        map->setLevelType(LARGE_BIOMES);
      else if (l_value == "amplified")
        map->setLevelType(AMPLIFIED);
    }
    else if (l_key == "max-build-height") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        map->setMaxBuildHeight(int_val);
    }
    else if (l_key == "max-players") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        map->setMaxPlayers(int_val);
    }
    else if (l_key == "motd")
      server->setMessageOfTheDay(i.value());
    else if (l_key == "online-mode") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setOnline(bool_val);
    }
    else if (l_key == "op-permission-level") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        server->setOpPermissionLevel(int_val);
    }
    else if (l_key == "player-idle-timeout") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        map->setIdleTimeout(int_val);
    }
    else if (l_key == "pvp") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setPVP(bool_val);
    }
    else if (l_key == "query.port") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        server->setQueryPort(int_val);
    }
    else if (l_key == "rcon.password")
      server->setRconPassword(i.value());
    else if (l_key == "rcon.port") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        server->setRconPort(int_val);
    }
    else if (l_key == "resource-pack")
      map->setResourcePack(i.value());
    else if (l_key == "server-ip") {
      QStringList parts = i.value().split('.', QString::SkipEmptyParts);
      if (parts.size() == 4) {
        quint8 ip[4];
        for (int i = 0; i < 4; ++i) {
          ip[i] = (quint8) parts[i].toUShort(&format_ok);
          if (!format_ok)
            ip[i] = 0;
        }
        server->setIPAddress(ip);
      }
    }
    else if (l_key == "server-name")
      server->setName(i.value());
    else if (l_key == "server-port") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        server->setPort(int_val);
    }
    else if (l_key == "snooper-enabled") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setSnooperEnabled(bool_val);
    }
    else if (l_key == "spawn-animals") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setAnimalsSpawn(bool_val);
    }
    else if (l_key == "spawn-monsters") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setMonstersSpawn(bool_val);
    }
    else if (l_key == "spawn-npcs") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        map->setNpcsSpawn(bool_val);
    }
    else if (l_key == "spawn-protection") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        map->setSpawnProtectionRadius(int_val);
    }
    else if (l_key == "view-distance") {
      int_val = i.value().toInt(&format_ok);
      if (format_ok)
        map->setViewDistance(int_val);
    }
    else if (l_key == "white-list") {
      bool_val = stringToBool(l_value, &format_ok);
      if (format_ok)
        server->setWhiteList(bool_val);
    }
  }

  return server;
}
