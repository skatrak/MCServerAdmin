#ifndef SERVER_H
#define SERVER_H

#include <QDataStream>
#include <QSet>
#include <QString>

class Map;

class Server {
public:
  // Constructors and destructor
  Server(const QString name = "Unknown Server");
  Server(const Server& other);
  ~Server(); // Elimina también todos los mapas

  // Class methods
  void addMap(Map* map);
  void removeMap(Map* map);
  Server& operator= (const Server& other);

  // Accessors and mutators
  QSet<Map*> getMaps() const { return maps; }

  QString getName() const { return name; }
  void setName(QString value) {
    name = value;
  }

  QString getMessageOfTheDay() const { return motd; }
  void setMessageOfTheDay(const QString& value) {
    motd = value;
  }

  const quint8* getIPAddress() const { return address; }
  void setIPAddress(const quint8* value) {
    if (value) {
      address[0] = value[0];
      address[1] = value[1];
      address[2] = value[2];
      address[3] = value[3];
    }
  }

  quint16 getPort() const { return port; }
  void setPort(quint16 value) {
    port = value;
  }

  bool isOnline() const { return online; }
  void setOnline(bool value) {
    online = value;
  }

  bool areAchievementsAnnounced() const { return announce_achievements; }
  void setAchievementsAnnounced(bool value) {
    announce_achievements = value;
  }

  bool isSnooperEnabled() const { return snooper; }
  void setSnooperEnabled(bool value) {
    snooper = value;
  }

  bool hasWhiteList() const { return white_list; }
  void setWhiteList(bool value) {
    white_list = value;
  }

  int getOpPermissionLevel() const { return op_permission_level; }
  void setOpPermissionLevel(int value) {
    op_permission_level = value;
  }

  bool isQueryEnabled() const { return query; }
  void setQueryEnabled(bool value) {
    query = value;
  }

  quint16 getQueryPort() const { return query_port; }
  void setQueryPort(quint16 value) {
    query_port = value;
  }

  bool isRemoteConsoleEnabled() const { return rcon; }
  void setRemoteConsoleEnabled(bool value) {
    rcon = value;
  }

  QString getRconPassword() const { return rcon_password; }
  void setRconPassword(const QString& value) {
    rcon_password = value;
  }

  quint16 getRconPort() const { return rcon_port; }
  void setRconPort(quint16 value) {
    rcon_port = value;
  }

  static Server* fromServerProperties(const QMap<QString, QString>& properties);

protected:
  static const quint16 DEFAULT_PORT = 25565;
  static const bool DEFAULT_ONLINE = true;
  static const bool DEFAULT_ANNOUNCE_ACHIEVEMENTS = true;
  static const bool DEFAULT_SNOOPER = true;
  static const bool DEFAULT_WHITE_LIST = false;
  static const int DEFAULT_OP_PERMISSION_LEVEL = 4;
  static const bool DEFAULT_QUERY = false;
  static const quint16 DEFAULT_QUERY_PORT = 25565;
  static const bool DEFAULT_RCON = false;
  static const quint16 DEFAULT_RCON_PORT = 25575;

private:
  // List of maps associated to this server
  QSet<Map*> maps;

  // Server
  QString name;
  QString motd;
  quint8 address[4];
  quint16 port;
  bool online;
  bool announce_achievements;

  // Administration
  bool snooper;
  bool white_list;
  int op_permission_level;
  bool query;
  quint16 query_port;
  bool rcon;
  QString rcon_password;
  quint16 rcon_port;

};

// Methods for the class's serialization
QDataStream& operator<<(QDataStream&, const Server&);
QDataStream& operator>>(QDataStream&, Server&);

#endif // SERVER_H
