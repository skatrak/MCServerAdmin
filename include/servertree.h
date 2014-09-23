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

protected:
  void changeEvent(QEvent* event);

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
