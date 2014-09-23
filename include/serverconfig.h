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

#ifndef SERVERCONFIG_H
#define SERVERCONFIG_H

#include "configwidget.h"
#include "server.h"

namespace Ui {
class ServerConfig;
}

class ServerConfig: public ConfigWidget {
  Q_OBJECT

public:
  explicit ServerConfig(const Server* svr, QWidget* parent = 0);
  ~ServerConfig();

  const Server* getServerInfo() const { return server; }
  void setServerInfo(const Server* svr);
  Server getActualConfiguration() const;

  void loadServer();

public slots:
  void changeName(QString name);

protected slots:
  void onNameChanged();

private slots:
  void on_bannedIPsButton_clicked();
  void on_bannedPlayersButton_clicked();
  void on_opsButton_clicked();
  void on_blacklistButton_clicked();
  void on_whitelistButton_clicked();
  void on_clearButton_clicked();

protected:
  void loadJsonAttributes(QString filename, QStringList attributes);
  void setHeader(QStringList titles);

private:
  Ui::ServerConfig* ui;
  const Server* server;

};

#endif // SERVERCONFIG_H
