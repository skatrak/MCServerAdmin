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

#include "mapconfig.h"
#include "ui_mapconfig.h"

MapConfig::MapConfig(const Map* m, QWidget* parent)
  : ConfigWidget(parent), ui(new Ui::MapConfig), map(m) {
  ui->setupUi(this);

  ui->difficulty->addItems(DIFFICULTY_NAMES.keys());
  ui->gamemode->addItems(GAME_MODE_NAMES.keys());
  ui->level_type->addItems(LEVEL_TYPE_NAMES.keys());

  QRegExp file_rx("[^\\\\/:*?\"<>|]+");
  QRegExp url_rx("((http|ftp|https|sftp)://)?[a-zA-Z0-9._/-]+(:\\d+)?");
  QValidator* file_validator = new QRegExpValidator(file_rx, this);
  QValidator* url_validator = new QRegExpValidator(url_rx, this);

  ui->map_name->setValidator(file_validator);
  ui->resource_pack->setValidator(url_validator);

  loadMap();

  connect(ui->spawn_animals, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->command_block, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->difficulty, SIGNAL(activated(int)), this, SLOT(onModification()));
  connect(ui->allow_flight, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->gamemode, SIGNAL(activated(int)), this, SLOT(onModification()));
  connect(ui->force_gamemode, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->hardcore, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->idle_timeout, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->level_type, SIGNAL(activated(int)), this, SLOT(onModification()));
  connect(ui->max_build_height, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->max_players, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->spawn_monsters, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->map_name, SIGNAL(editingFinished()), this, SLOT(onNameChanged()));
  connect(ui->allow_nether, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->spawn_npcs, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->pvp, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->resource_pack, SIGNAL(textEdited(QString)), this, SLOT(onModification()));
  connect(ui->level_seed, SIGNAL(textEdited(QString)), this, SLOT(onModification()));
  connect(ui->spawn_protection, SIGNAL(valueChanged(int)), this, SLOT(onModification()));
  connect(ui->generate_structures, SIGNAL(clicked()), this, SLOT(onModification()));
  connect(ui->view_distance, SIGNAL(valueChanged(int)), this, SLOT(onModification()));

  connect(ui->save_button, SIGNAL(clicked()), this, SIGNAL(saveChanges()));
  connect(ui->discard_button, SIGNAL(clicked()), this, SIGNAL(discardChanges()));
}

MapConfig::~MapConfig (void) {
  delete ui;
}

void MapConfig::setMapInfo(const Map* m) {
  map = m;
  loadMap();
}

Map MapConfig::getActualConfiguration() const {
  Map m(0);

  m.setAnimalsSpawn(ui->spawn_animals->isChecked());
  m.setCommandBlock(ui->command_block->isChecked());
  m.setDifficulty(DIFFICULTY_NAMES.value(ui->difficulty->currentText()));
  m.setFlightAllowed(ui->allow_flight->isChecked());
  m.setGameMode(GAME_MODE_NAMES.value(ui->gamemode->currentText()));
  m.setGameModeForced(ui->force_gamemode->isChecked());
  m.setHardcore(ui->hardcore->isChecked());
  m.setIdleTimeout(ui->idle_timeout->value());
  m.setLevelType(LEVEL_TYPE_NAMES.value(ui->level_type->currentText()));
  m.setMaxBuildHeight(ui->max_build_height->value());
  m.setMaxPlayers(ui->max_players->value());
  m.setMonstersSpawn(ui->spawn_monsters->isChecked());
  m.setName(ui->map_name->text());
  m.setNetherAllowed(ui->allow_nether->isChecked());
  m.setNpcsSpawn(ui->spawn_npcs->isChecked());
  m.setPVP(ui->pvp->isChecked());
  m.setResourcePack(ui->resource_pack->text());
  m.setSeed(ui->level_seed->text());
  m.setSpawnProtectionRadius(ui->spawn_protection->value());
  m.setStructuresGenerate(ui->generate_structures->isChecked());
  m.setViewDistance(ui->view_distance->value());

  return m;
}

void MapConfig::loadMap() {
  if (map) {
    ui->spawn_animals->setChecked(map->doAnimalsSpawn());
    ui->command_block->setChecked(map->getCommandBlock());
    ui->difficulty->setCurrentIndex(map->getDifficulty());
    ui->allow_flight->setChecked(map->isFlightAllowed());
    ui->gamemode->setCurrentIndex(map->getGameMode());
    ui->force_gamemode->setChecked(map->isGameModeForced());
    ui->hardcore->setChecked(map->isHardcore());
    ui->idle_timeout->setValue(map->getIdleTimeout());
    ui->level_type->setCurrentIndex(map->getLevelType());
    ui->max_build_height->setValue(map->getMaxBuildHeight());
    ui->max_players->setValue(map->getMaxPlayers());
    ui->spawn_monsters->setChecked(map->doMonstersSpawn());
    ui->map_name->setText(map->getName());
    ui->allow_nether->setChecked(map->isNetherAllowed());
    ui->spawn_npcs->setChecked(map->doNpcsSpawn());
    ui->pvp->setChecked(map->isPVP());
    ui->resource_pack->setText(map->getResourcePack());
    ui->level_seed->setText(map->getSeed());
    ui->spawn_protection->setValue(map->getSpawnProtectionRadius());
    ui->generate_structures->setChecked(map->doStructuresGenerate());
    ui->view_distance->setValue(map->getViewDistance());

    setChanged(false);
  }
}

void MapConfig::changeName(QString name) {
  ui->map_name->setText(name);
}

void MapConfig::changeEvent(QEvent* event) {
  if (event && event->type() == QEvent::LanguageChange)
    ui->retranslateUi(this);
}

void MapConfig::onNameChanged() {
  emit(nameChanged(ui->map_name->text()));
}
