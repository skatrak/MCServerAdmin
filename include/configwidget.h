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

#ifndef CONFIGWIDGET_H
#define CONFIGWIDGET_H

#include <QTabWidget>

class ConfigWidget: public QTabWidget {
  Q_OBJECT

public:
  explicit ConfigWidget(QWidget* parent = 0);
  virtual ~ConfigWidget();

  void setChanged(bool b) { changed = b; }
  bool isChanged() const { return changed; }

public slots:
  virtual void changeName(QString name) = 0;

protected slots:
  void onModification();
  virtual void onNameChanged() = 0;

signals:
  void dataModified();
  void nameChanged(QString name);
  void saveChanges();
  void discardChanges();

private:
  bool changed;
};

#endif // CONFIGWIDGET_H
