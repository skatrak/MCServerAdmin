#include "servertree.h"

#include "mainwindow.h"
#include "map.h"
#include "servertreedelegate.h"
#include "server.h"

#include <QFile>
#include <QMessageBox>
#include <QMenu>
#include <QTextStream>

ServerTree::ServerTree(QWidget* parent): QTreeWidget(parent) {
  setIndentation(10);
  setEditTriggers(SelectedClicked | EditKeyPressed);
  setContextMenuPolicy(Qt::CustomContextMenu);
  setHeaderItem(new QTreeWidgetItem(QStringList() = {tr("Name"), tr("Status")}));
  setItemDelegate(new ServerTreeDelegate(this));

  connect(this, SIGNAL(customContextMenuRequested(QPoint)),
          this, SLOT(onContextMenuRequested(QPoint)));
  connect(this, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
          this, SLOT(onItemNameChanged(QTreeWidgetItem*,int)));

  connect(this, SIGNAL(serverCreated(QString)), this, SIGNAL(dataModified()));
  connect(this, SIGNAL(serverRenamed(QString,QString)), this, SIGNAL(dataModified()));
  connect(this, SIGNAL(mapCreated(QString,QString)), this, SIGNAL(dataModified()));
  connect(this, SIGNAL(mapRenamed(QString,QString,QString)), this, SIGNAL(dataModified()));
}

ServerTree::~ServerTree() {
  while (topLevelItemCount() > 0)
    deleteItem(topLevelItem(0));
}

bool ServerTree::importServerProperties(QString file_name) {
  QFile file(file_name);
  if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    return false;

  QTextStream in(&file);
  QMap<QString, QString> server_properties;
  while (!in.atEnd()) {
    QString line = in.readLine().trimmed();
    if (line.length() == 0 || line.at(0) == '#')
      continue;

    QStringList words = line.split('=', QString::KeepEmptyParts);
    if (words.size() == 1)
      continue;

    QString property = words.takeFirst();
    QString value = words.join('=');
    server_properties[property] = value;
  }
  file.close();

  Server* server = Server::fromServerProperties(server_properties);
  addServer(server->getName(), server);

  return true;
}

QTreeWidgetItem* ServerTree::getServer(QString server_name) const {
  for (int i = 0, sz = topLevelItemCount(); i < sz; ++i) {
    QTreeWidgetItem* server = topLevelItem(i);
    if (server->text(0).toLower() == server_name.toLower())
      return server;
  }
  return 0;
}

QTreeWidgetItem* ServerTree::getMap(QTreeWidgetItem* server, QString map_name) const {
  if (isServerItem(server)) {
    for (int i = 0, sz = server->childCount(); i < sz; ++i) {
      QTreeWidgetItem* map = server->child(i);
      if (map->text(0).toLower() == map_name.toLower())
        return map;
    }
  }
  return 0;
}

void ServerTree::deleteItem(QTreeWidgetItem* item) {
  if (item) {
    if (isServerItem(item)) {
      while (item->childCount() > 0)
        deleteItem(item->child(0));

      deleteData(item, SavedData);
      deleteData(item, ModifiedData);
    }
    else {
      deleteData(item, SavedData);
      deleteData(item, ModifiedData);
      item->parent()->removeChild(item);
    }

    emit(dataModified());
    delete item;
  }
}

void ServerTree::discardModifications(QTreeWidgetItem* item) {
  if (item) {
    deleteData(item, ModifiedData);
    setStatusData(item, SavedStatus);
  }
}

void ServerTree::saveModifications(QTreeWidgetItem* item) {
  if (isServerItem(item)) {
    Server* mod = extractServerData(item, ModifiedData);
    if (mod) {
      Server* saved = extractServerData(item, SavedData);
      if (saved) {
        QSet<Map*> maps = saved->getMaps();
        for (QSet<Map*>::iterator i = maps.begin(); i != maps.end(); ++i)
          (*i)->setServer(mod);
      }

      deleteData(item, SavedData);
      setServerData(item, SavedData, mod);
      setServerData(item, ModifiedData, 0);
    }
    setStatusData(item, SavedStatus);
    emit(dataModified());
  }
  else if (isMapItem(item)) {
    Map* mod = extractMapData(item, ModifiedData);
    if (mod) {
      mod->setServer(extractMapData(item, SavedData)->getServer());
      deleteData(item, SavedData);
      setMapData(item, SavedData, mod);
      setMapData(item, ModifiedData, 0);
    }
    setStatusData(item, SavedStatus);
    emit(dataModified());
  }
}

void ServerTree::changeItemName(QTreeWidgetItem* item, QString name) {
  if (item && item->text(0) != name) {
    QString final_name;
    if (isServerItem(item)) {
      final_name = findSuitableServerName(name);

      Server* server = extractServerData(item, SavedData);
      if (server) server->setName(final_name);
      server = extractServerData(item, ModifiedData);
      if (server) server->setName(final_name);

      emit(serverRenamed(item->text(0), final_name));
    }
    else if (isMapItem(item)) {
      final_name = findSuitableMapName(item->parent(), name);

      Map* map = extractMapData(item, SavedData);
      if (map) map->setName(final_name);
      map = extractMapData(item, ModifiedData);
      if (map) map->setName(final_name);

      emit(mapRenamed(item->parent()->text(0), item->text(0), final_name));
    }

    item->setText(0, final_name);
  }
}

void ServerTree::addServer(QString server_name, Server* server_data) {
  server_name = findSuitableServerName(server_name);

  QTreeWidgetItem* item = new QTreeWidgetItem(this, QStringList(server_name));
  item->setFlags(item->flags() | Qt::ItemIsEditable);
  setStatusData(item, SavedStatus);
  addTopLevelItem(item);

  if (server_data) {
    server_data->setName(server_name);

    setServerData(item, SavedData, server_data);
    QSet<Map*> maps = server_data->getMaps();
    for (QSet<Map*>::iterator i = maps.begin(); i != maps.end(); ++i)
      addMap(item, (*i)->getName(), *i);
  }
  else {
    server_data = new Server(server_name);
    setServerData(item, SavedData, server_data);
  }

  setServerData(item, ModifiedData, 0);

  emit(serverCreated(server_name));
}

void ServerTree::addMap(QTreeWidgetItem* server, QString map_name, Map* map_data) {
  if (isServerItem(server)) {
    map_name = findSuitableMapName(server, map_name);

    QTreeWidgetItem* item = new QTreeWidgetItem(server, QStringList(map_name));
    item->setFlags(item->flags() | Qt::ItemIsEditable);
    setStatusData(item, SavedStatus);
    server->addChild(item);

    Server* server_data = extractServerData(server, SavedData);
    if (map_data) {
      map_data->setServer(server_data);
      map_data->setName(map_name);
    }
    else
      map_data = new Map(server_data, map_name);

    setMapData(item, SavedData, map_data);
    setServerData(item, ModifiedData, 0);

    emit(mapCreated(server->text(0), map_name));
  }
}

bool ServerTree::isServerItem(QTreeWidgetItem* item) const {
  return item && !item->parent();
}

bool ServerTree::isMapItem(QTreeWidgetItem* item) const {
  return item && item->parent();
}

QString ServerTree::findSuitableServerName(QString desired) const {
  QString name = desired;
  int i = 0;
  while (getServer(name))
    name = desired + QString::number(++i);
  return name;
}

QString ServerTree::findSuitableMapName(QTreeWidgetItem* server, QString desired) const {
  QString name = desired;

  if (isServerItem(server)) {
    int i = 0;
    while (getMap(server, name))
      name = desired + QString::number(++i);
  }

  return name;
}

Server* ServerTree::extractServerData(QTreeWidgetItem* item, DataRole role) const {
  if (isServerItem(item))
    return reinterpret_cast<Server*>(item->data(0, role).value<void*>());
  return 0;
}

Map* ServerTree::extractMapData(QTreeWidgetItem* item, DataRole role) const {
  if (isMapItem(item))
    return reinterpret_cast<Map*>(item->data(0, role).value<void*>());
  return 0;
}

ItemStatus ServerTree::extractStatusData(QTreeWidgetItem* item) const {
  if (item)
    return item->data(1, Qt::UserRole).value<ItemStatus>();
  return InvalidStatus;
}

void ServerTree::setServerData(QTreeWidgetItem* item, DataRole role, Server* data) {
  if (isServerItem(item))
    item->setData(0, role, QVariant(QMetaType::VoidStar, reinterpret_cast<void*>(&data)));
}

void ServerTree::setMapData(QTreeWidgetItem* item, DataRole role, Map* data) {
  if (isMapItem(item))
    item->setData(0, role, QVariant(QMetaType::VoidStar, reinterpret_cast<void*>(&data)));
}

void ServerTree::setStatusData(QTreeWidgetItem* item, ItemStatus status) {
  if (item) {
    QVariant var;
    var.setValue(status);
    item->setData(1, Qt::UserRole, var);
    item->setText(1, ITEM_STATUS_NAMES[status]);
  }
}

// Si se elimina el DataRole "SavedData" de un server, se eliminan también los datos de
// todos los mapas de ese server, pero no los items
void ServerTree::deleteData(QTreeWidgetItem* item, DataRole role) {
  Map* data = extractMapData(item, role);
  if (data) {
    delete data;
    setMapData(item, role, 0);
  }
  else {
    Server* data = extractServerData(item, role);
    if (data) {
      delete data;
      setServerData(item, role, 0);
    }
  }
}

void ServerTree::onContextMenuRequested(QPoint pos) {
  QTreeWidgetItem* item = itemAt(pos);
  QPoint mapped_pos = mapToParent(pos);
  if (item)
    emit(customEditMenuRequested(mapped_pos));
  else
    emit(globalEditMenuRequested(mapped_pos));
}

void ServerTree::onItemNameChanged(QTreeWidgetItem* item, int col) {
  if (col == 0) {
    QString new_name = item->text(0);

    Server* server_data = extractServerData(item, SavedData);
    if (server_data)
      item->setText(0, server_data->getName());
    else {
      Map * map_data = extractMapData(item, SavedData);
      if (map_data)
        item->setText(0, map_data->getName());
    }
    if (new_name != item->text(0)) {
      changeItemName(item, new_name);
      emit(itemNameChanged(item->text(0)));
    }
  }
}

QDataStream& operator<<(QDataStream& ds, const ServerTree& tree) {
  int server_count = tree.topLevelItemCount();
  ds << server_count;

  for (int i = 0; i < server_count; ++i) {
    QTreeWidgetItem* server_itm = tree.topLevelItem(i);
    Server* server_data = tree.extractServerData(server_itm, SavedData);
    if (server_data)
      ds << true << *server_data;
    else
      ds << false;
  }

  return ds;
}

QDataStream& operator>>(QDataStream& ds, ServerTree& tree) {
  int server_count;
  ds >> server_count;

  bool valid_server;
  for (int i = 0; i < server_count; ++i) {
    ds >> valid_server;
    if (valid_server) {
      Server* server = new Server;
      ds >> *server;
      tree.addServer(server->getName(), server);
    }
  }

  return ds;
}
