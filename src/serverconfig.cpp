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

#include "serverconfig.h"
#include "ui_serverconfig.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>

ServerConfig::ServerConfig(const Server* svr, QWidget *parent)
  : ConfigWidget(parent), ui(new Ui::ServerConfig), server(svr) {
  ui->setupUi(this);

  QRegExp file_rx("[^\\\\/:*?\"<>|]+");
  QValidator* validator = new QRegExpValidator(file_rx, this);
  ui->server_name->setValidator(validator);

  loadServer();

  connect(ui->announce_achievements, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->ip_1, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->ip_2, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->ip_3, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->ip_4, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->motd, SIGNAL(textEdited(QString)), this, SLOT(onModification()));
  connect(ui->online_mode, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->op_permission_level, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->query, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->query_port, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->rcon, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->rcon_password, SIGNAL(textEdited(QString)), this, SLOT(onModification()));
  connect(ui->rcon_port, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->server_name, SIGNAL(editingFinished()), this, SLOT(onNameChanged()));
  connect(ui->server_port, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->snooper, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->white_list, SIGNAL(clicked()), this, SLOT(onModification()));

  connect(ui->save_button, SIGNAL(clicked()), this, SIGNAL(saveChanges()));
  connect(ui->discard_button, SIGNAL(clicked()), this, SIGNAL(discardChanges()));
}

ServerConfig::~ServerConfig() {
  delete ui;
}

void ServerConfig::setServerInfo(const Server* svr) {
  server = svr;
  loadServer();
}

Server ServerConfig::getActualConfiguration() const {
  Server s;

  s.setAchievementsAnnounced(ui->announce_achievements->isChecked());
  s.setMessageOfTheDay(ui->motd->text());
  s.setOnline(ui->online_mode->isChecked());
  s.setOpPermissionLevel(ui->op_permission_level->value());
  s.setQueryEnabled(ui->query->isChecked());
  s.setQueryPort(ui->query_port->value());
  s.setRemoteConsoleEnabled(ui->rcon->isChecked());
  s.setRconPassword(ui->rcon_password->text());
  s.setRconPort(ui->rcon_port->value());
  s.setName(ui->server_name->text());
  s.setPort(ui->server_port->value());
  s.setSnooperEnabled(ui->snooper->isChecked());
  s.setWhiteList(ui->white_list->isChecked());

  quint8 ip[4] = {ui->ip_1->value(), ui->ip_2->value(), ui->ip_3->value(), ui->ip_4->value()};
  s.setIPAddress(ip);

  return s;
}

void ServerConfig::loadServer() {
  if (server) {
    ui->announce_achievements->setChecked(server->areAchievementsAnnounced());
    ui->motd->setText(server->getMessageOfTheDay());
    ui->online_mode->setChecked(server->isOnline());
    ui->op_permission_level->setValue(server->getOpPermissionLevel());
    ui->query->setChecked(server->isQueryEnabled());
    ui->query_port->setValue(server->getQueryPort());
    ui->rcon->setChecked(server->isRemoteConsoleEnabled());
    ui->rcon_password->setText(server->getRconPassword());
    ui->rcon_port->setValue(server->getRconPort());
    ui->server_name->setText(server->getName());
    ui->server_port->setValue(server->getPort());
    ui->snooper->setChecked(server->isSnooperEnabled());
    ui->white_list->setChecked(server->hasWhiteList());

    const quint8* ip = server->getIPAddress();
    ui->ip_1->setValue(ip[0]);
    ui->ip_2->setValue(ip[1]);
    ui->ip_3->setValue(ip[2]);
    ui->ip_4->setValue(ip[3]);

    ui->query_port->setEnabled(ui->query->isChecked());
    ui->rcon_password->setEnabled(ui->rcon->isChecked());
    ui->rcon_port->setEnabled(ui->rcon->isChecked());

    setChanged(false);
  }
}

void ServerConfig::changeName(QString name) {
  ui->server_name->setText(name);
}

void ServerConfig::changeEvent(QEvent* event) {
  if (event && event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
}

void ServerConfig::onNameChanged() {
  emit(nameChanged(ui->server_name->text()));
}

void ServerConfig::on_bannedIPsButton_clicked() {
  ui->treeWidget->clear();
  setHeader({"IP", "Banned by", "Reason"});
  loadJsonAttributes("banned-ips.json", {"ip", "source", "reason"});
}

void ServerConfig::on_bannedPlayersButton_clicked() {
  ui->treeWidget->clear();
  setHeader({"Player", "Banned by", "Reason"});
  loadJsonAttributes("banned-players.json", {"name", "source", "reason"});
}

void ServerConfig::on_opsButton_clicked() {
  ui->treeWidget->clear();
  setHeader({"Name", "Permission level"});
  loadJsonAttributes("ops.json", {"name", "level"});
}

void ServerConfig::on_blacklistButton_clicked() {
  ui->treeWidget->clear();
  setHeader({"Name"});
  loadJsonAttributes("blacklist.json", {"name"});
}

void ServerConfig::on_whitelistButton_clicked() {
  ui->treeWidget->clear();
  setHeader({"Name"});
  loadJsonAttributes("whitelist.json", {"name"});
}

void ServerConfig::on_clearButton_clicked() {
  ui->treeWidget->clear();
  setHeader(QStringList());
}

void ServerConfig::loadJsonAttributes(QString filename, QStringList attributes) {
  QFile ban_ips(ui->server_name->text() + "/" + filename);
  if (ban_ips.open(QIODevice::ReadOnly | QIODevice::Text)) {
    QJsonParseError error;
    QJsonDocument doc = QJsonDocument::fromJson(ban_ips.readAll(), &error);

    if (error.error == QJsonParseError::NoError) {
      QJsonArray objs = doc.array();
      for (int i = 0; i < objs.size(); ++i) {
        if (objs[i].isObject()) {
          QJsonObject obj = objs[i].toObject();
          QStringList values;

          for (QStringList::const_iterator i = attributes.cbegin(); i != attributes.cend(); ++i) {
            QJsonValue val = obj[*i];
            if (val.isString())
              values.push_back(val.toString());
            else if (val.isDouble())
              values.push_back(QString::number(val.toInt()));
            else if (val.isBool())
              values.push_back(val.toBool()? "true" : "false");
          }
          ui->treeWidget->addTopLevelItem(new QTreeWidgetItem(ui->treeWidget, values));
        }
      }
    }

    ban_ips.close();
  }
}

void ServerConfig::setHeader(QStringList titles) {
  if (titles.size() > 0) {
    ui->treeWidget->setHeaderHidden(false);
    ui->treeWidget->setHeaderItem(new QTreeWidgetItem(titles));

    for (int i = 0, sz = titles.size(); i < sz; ++i)
      ui->treeWidget->setColumnWidth(i, (ui->treeWidget->width()-10) / sz);
  }
  else
    ui->treeWidget->setHeaderHidden(true);
}
