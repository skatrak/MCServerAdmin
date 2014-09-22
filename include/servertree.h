#ifndef SERVERTREE_H
#define SERVERTREE_H

#include <QDataStream>
#include <QMap>
#include <QString>
#include <QTreeWidget>

class Map;
class Server;

enum DataRole {
  SavedData = Qt::UserRole,
  ModifiedData
};

enum ItemStatus {
  SavedStatus,
  ModifiedStatus,
  ActiveStatus,
  InvalidStatus
};

Q_DECLARE_METATYPE(ItemStatus)

static QMap<ItemStatus, QString> ITEM_STATUS_NAMES {
  { SavedStatus,    "Saved"    },
  { ModifiedStatus, "Modified" },
  { ActiveStatus,   "Active"   },
  { InvalidStatus,  "Unknown"  }
};

class ServerTree: public QTreeWidget {
  Q_OBJECT

public:
  explicit ServerTree(QWidget* parent = 0);
  ~ServerTree();

  bool importServerProperties(QString file_name);

  QTreeWidgetItem* getServer(QString server_name) const;
  QTreeWidgetItem* getMap(QTreeWidgetItem* server, QString map_name) const;

  void deleteItem(QTreeWidgetItem* item);
  void discardModifications(QTreeWidgetItem* item);
  void saveModifications(QTreeWidgetItem* item);
  void changeItemName(QTreeWidgetItem* item, QString name);

  void addServer(QString server_name, Server* server_data = 0);
  void addMap(QTreeWidgetItem* server, QString map_name, Map* map_data = 0);

  bool isServerItem(QTreeWidgetItem* item) const;
  bool isMapItem(QTreeWidgetItem* item) const;
  QString findSuitableServerName(QString desired) const;
  QString findSuitableMapName(QTreeWidgetItem* server, QString desired) const;

  Server* extractServerData(QTreeWidgetItem* item, DataRole role) const;
  Map* extractMapData(QTreeWidgetItem* item, DataRole role) const;
  ItemStatus extractStatusData(QTreeWidgetItem* item) const;

  void setServerData(QTreeWidgetItem* item, DataRole role, Server* data);
  void setMapData(QTreeWidgetItem* item, DataRole role, Map* data);
  void setStatusData(QTreeWidgetItem* item, ItemStatus status);
  void deleteData(QTreeWidgetItem* item, DataRole role);

public slots:
  void onContextMenuRequested(QPoint pos);

protected slots:
  void onItemNameChanged(QTreeWidgetItem* item, int col);

signals:
  void dataModified();
  void updateServerProperties(Map*);
  void itemNameChanged(QString);
  void customEditMenuRequested(QPoint pos);
  void globalEditMenuRequested(QPoint pos);
  void serverCreated(QString server);
  void serverRenamed(QString prev, QString act);
  void mapCreated(QString server, QString map);
  void mapRenamed(QString server, QString prev, QString act);

};

QDataStream& operator<<(QDataStream&, const ServerTree&);
QDataStream& operator>>(QDataStream&, ServerTree&);

#endif // SERVERTREE_H
