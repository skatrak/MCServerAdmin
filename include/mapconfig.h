#ifndef MAPCONFIG_H
#define MAPCONFIG_H

#include "configwidget.h"
#include "map.h"

namespace Ui {
class MapConfig;
}

class MapConfig: public ConfigWidget {
  Q_OBJECT

public:
  explicit MapConfig(const Map* map, QWidget* parent = 0);
  ~MapConfig();

  const Map* getMapInfo() const { return map; }
  void setMapInfo(const Map* map);
  Map getActualConfiguration() const; // Desvinculado del server (para no añadirlo)

  void loadMap();

public slots:
  void changeName(QString name);

protected slots:
  void onNameChanged();

private:
  Ui::MapConfig* ui;
  const Map* map;

};

#endif // MAPCONFIG_H
