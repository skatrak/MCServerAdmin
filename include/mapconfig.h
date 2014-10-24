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

protected:
  void changeEvent(QEvent* event);

protected slots:
  void onNameChanged();

private:
  void translateComboBoxes();

private:
  Ui::MapConfig* ui;
  const Map* map;

};

#endif // MAPCONFIG_H
