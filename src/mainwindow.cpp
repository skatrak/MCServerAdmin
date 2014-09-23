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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "mapconfig.h"
#include "serverconfig.h"
#include "servertree.h"

#include <QCloseEvent>
#include <QDesktopServices>
#include <QDir>
#include <QFile>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
  : QMainWindow(parent), ui(new Ui::MainWindow), tree(new ServerTree(this)),
    rightWidget(0), globalEditMenu(new QMenu(this)), customMenu(new QMenu(this)) {

  ui->setupUi(this);
  setupActions();
  createLanguageList();

  onDatabaseSaved();

  globalEditMenu->addAction(addServerAction);
  globalEditMenu->addSeparator();
  globalEditMenu->addAction(saveAllAction);
  globalEditMenu->addAction(discardAllAction);
  globalEditMenu->addSeparator();

  ui->servers_layout->addWidget(tree);
  ui->menuEdit->addActions(globalEditMenu->actions());

  connect(ui->action_exit, SIGNAL(triggered()), this, SLOT(close()));
  connect(tree, SIGNAL(currentItemChanged(QTreeWidgetItem*,QTreeWidgetItem*)),
          this, SLOT(onCurrentChanged(QTreeWidgetItem*,QTreeWidgetItem*)));
  connect(tree, SIGNAL(serverCreated(QString)),
          this, SLOT(createNewServerFolder(QString)));
  connect(tree, SIGNAL(serverRenamed(QString,QString)),
          this, SLOT(renameServerFolder(QString,QString)));
  connect(tree, SIGNAL(mapCreated(QString,QString)),
          this, SLOT(createNewMapFolder(QString,QString)));
  connect(tree, SIGNAL(mapRenamed(QString,QString,QString)),
          this, SLOT(renameMapFolder(QString,QString,QString)));
  connect(tree, SIGNAL(globalEditMenuRequested(QPoint)),
          this, SLOT(showGlobalEditContextMenu(QPoint)));
  connect(tree, SIGNAL(customEditMenuRequested(QPoint)),
          this, SLOT(showCustomEditContextMenu(QPoint)));
  connect(tree, SIGNAL(dataModified()), this, SLOT(onDatabaseModified()));
}

MainWindow::~MainWindow (void) {
  delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
  if (promptIfChangesPending())
    event->accept();
  else
    event->ignore();
}

bool MainWindow::saveDatabase(QString fileName) {
  QFile file(fileName);

  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::critical(this, tr("Error saving file"),
                          tr("Impossible to save database to %1.\n"
                             "Make sure you have write permissions for that file.").arg(fileName),
                          QMessageBox::Ok);
    return false;
  }

  QDataStream out(&file);
  out << *tree;

  file.flush();
  file.close();

  loadedFile = fileName;
  onDatabaseSaved();
  return true;
}

bool MainWindow::loadDatabase(QString fileName) {
  QFile file(fileName);

  if (!file.open(QIODevice::ReadOnly)) {
    QMessageBox::critical(this, tr("Error loading file"),
                          tr("Impossible to load database from %1.\n"
                             "Make sure you have read permissions for that file.").arg(fileName),
                          QMessageBox::Ok);
    return false;
  }

  QDataStream in(&file);
  in >> *tree;

  file.close();
  return true;
}

void MainWindow::onCurrentChanged(QTreeWidgetItem* cur, QTreeWidgetItem* /*prev*/) {
  // Eliminamos el menú de la derecha y los elementos del menú personalizado
  customMenu->clear();
  if (rightWidget)
    delete rightWidget;

  // Mostramos la pantalla de configuración correspondiente al elemento seleccionado
  if (tree->isServerItem(cur)) {
    Server* server = tree->extractServerData(cur, ModifiedData);
    if (!server)
      server = tree->extractServerData(cur, SavedData);

    rightWidget = new ServerConfig(server, this);

    connect(rightWidget, SIGNAL(saveChanges()), this, SLOT(onSaveServerChangesTriggered()));
    connect(rightWidget, SIGNAL(discardChanges()), this, SLOT(onDiscardServerChangesTriggered()));

    customMenu->setTitle(tr("Server"));
    customMenu->addAction(addMapAction);
    customMenu->addSeparator();
    customMenu->addAction(saveServerChangesAction);
    customMenu->addAction(discardServerChangesAction);
    customMenu->addSeparator();
    customMenu->addAction(removeServerAction);
  }
  else if (tree->isMapItem(cur)) {
    Map* map = tree->extractMapData(cur, ModifiedData);
    if (!map)
      map = tree->extractMapData(cur, SavedData);

    rightWidget = new MapConfig(map, this);

    connect(rightWidget, SIGNAL(saveChanges()), this, SLOT(onSaveMapChangesTriggered()));
    connect(rightWidget, SIGNAL(discardChanges()), this, SLOT(onDiscardMapChangesTriggered()));

    customMenu->setTitle(tr("Map"));
    customMenu->addAction(setActiveAction);
    customMenu->addSeparator();
    customMenu->addAction(saveMapChangesAction);
    customMenu->addAction(discardMapChangesAction);
    customMenu->addSeparator();
    customMenu->addAction(removeMapAction);
  }
  else {
    ui->menuEdit->removeAction(customMenuAction);
    rightWidget = 0;
    return;
  }

  connect(rightWidget, SIGNAL(nameChanged(QString)), this, SLOT(onItemNameChanged(QString)));
  connect(rightWidget, SIGNAL(dataModified()), this, SLOT(onModifiedDataChanged()));
  connect(tree, SIGNAL(itemNameChanged(QString)), rightWidget, SLOT(changeName(QString)));

  customMenuAction = ui->menuEdit->addMenu(customMenu);
  ui->main_layout->addWidget(rightWidget);
}

void MainWindow::onItemNameChanged(QString name) {
  tree->changeItemName(tree->currentItem(), name);
  rightWidget->changeName(name);
}

void MainWindow::onModifiedDataChanged() {
  QTreeWidgetItem* item = tree->currentItem();
  if (tree->isServerItem(item)) {
    ServerConfig* server_config = (ServerConfig*) rightWidget;
    Server* server_data = tree->extractServerData(item, ModifiedData);
    if (server_data)
      *server_data = server_config->getActualConfiguration();
    else
      tree->setServerData(item, ModifiedData,
                          new Server(server_config->getActualConfiguration()));
  }
  else if (tree->isMapItem(item)) {
    MapConfig* map_config = (MapConfig*) rightWidget;
    Map* map_data = tree->extractMapData(item, ModifiedData);
    if (map_data)
      *map_data = map_config->getActualConfiguration();
    else
      tree->setMapData(item, ModifiedData,
                       new Map(map_config->getActualConfiguration()));
  }
  tree->setStatusData(item, ModifiedStatus);
}

void MainWindow::onDatabaseModified() {
  modifiedDatabase = true;
  QString name = loadedFile;
  if (name.length() == 0)
    name = tr("Unsaved");

  setWindowTitle("MCServerAdmin (" + name + ")*");
}

void MainWindow::onDatabaseSaved() {
  modifiedDatabase = false;
  QString name = loadedFile;
  if (name.length() == 0)
    name = tr("Unsaved");

  setWindowTitle("MCServerAdmin (" + name + ")");
}

void MainWindow::onAddServerTriggered() {
  tree->addServer(tr("New server"));
}

void MainWindow::onSaveAllTriggered() {
  for (int i = 0, sz = tree->topLevelItemCount(); i < sz; ++i) {
    QTreeWidgetItem* server = tree->topLevelItem(i);
    tree->saveModifications(server);
    for (int j = 0; j < server->childCount(); ++j)
      tree->saveModifications(server->child(j));
  }
}

void MainWindow::onDiscardAllTriggered() {
  for (int i = 0, sz = tree->topLevelItemCount(); i < sz; ++i) {
    QTreeWidgetItem* server = tree->topLevelItem(i);
    tree->discardModifications(server);
    for (int j = 0; j < server->childCount(); ++j)
      tree->discardModifications(server->child(j));
  }
}

void MainWindow::onAddMapTriggered() {
  tree->addMap(tree->currentItem(), tr("New map"));
  tree->expandItem(tree->currentItem());
}

void MainWindow::onSaveServerChangesTriggered() {
  tree->saveModifications(tree->currentItem());
}

void MainWindow::onDiscardServerChangesTriggered() {
  tree->discardModifications(tree->currentItem());
  ((ServerConfig*) rightWidget)->setServerInfo(tree->extractServerData(tree->currentItem(), SavedData));
}

void MainWindow::onRemoveServerTriggered() {
  int result = QMessageBox::question(this, tr("Remove server"),
                                    tr("Are you sure you want to remove this server and all its maps?\n"
                                    "This operation cannot be undone."),
                                     QMessageBox::Yes, QMessageBox::No);
  if (result == QMessageBox::Yes)
    tree->deleteItem(tree->currentItem());
}

void MainWindow::onSetActiveMapTriggered() {
  // Guardamos las modificaciones del elemento
  QTreeWidgetItem* item = tree->currentItem();
  tree->saveModifications(item);

  // Creamos el fichero server.properties
  Map* map = tree->extractMapData(item, SavedData);
  QFile server_properties(map->getServer()->getName() + "/server.properties");
  if (map->createServerProperties(server_properties)) {
    server_properties.flush();
    server_properties.close();

    // Desmarcamos como activos a todos los mapas en este servidor
    QTreeWidgetItem* parent = item->parent();
    for (int i = 0, sz = parent->childCount(); i < sz; ++i) {
      if (tree->extractStatusData(parent->child(i)) == ActiveStatus) {
        tree->setStatusData(parent->child(i), SavedStatus);
        break;
      }
    }

    // Marcamos el mapa actual como activo
    tree->setStatusData(item, ActiveStatus);
  }
  else
    QMessageBox::critical(this, tr("File creation error"),
                          tr("The file \"server.properties\" could not be created.\n"
                          "Make sure you have write permissions in the current folder and in the server folder."),
                          QMessageBox::Ok);
}

void MainWindow::onSaveMapChangesTriggered() {
  ItemStatus status = tree->extractStatusData(tree->currentItem());
  tree->saveModifications(tree->currentItem());

  if (status == ActiveStatus)
    tree->setStatusData(tree->currentItem(), ActiveStatus);
}

void MainWindow::onDiscardMapChangesTriggered() {
  tree->discardModifications(tree->currentItem());
  ((MapConfig*) rightWidget)->setMapInfo(tree->extractMapData(tree->currentItem(), SavedData));
}

void MainWindow::onRemoveMapTriggered() {
  int result = QMessageBox::question(this, tr("Remove map"),
                                    tr("Are you sure you want to remove this map?\n"
                                    "This operation cannot be undone."),
                                     QMessageBox::Yes, QMessageBox::No);
  if (result == QMessageBox::Yes)
    tree->deleteItem(tree->currentItem());
}

void MainWindow::createNewServerFolder(QString server) {
  QDir dir;
  if (!dir.mkpath(server)) {
    QMessageBox::critical(this, tr("Directory creation error"),
                          tr("The folder for the server files could not be created.\n"
                          "Make sure you have write permissions in the current folder."),
                          QMessageBox::Ok);
    return;
  }
  else
    dir.cd(server);

  QStringList files = {"banned-ips.json", "banned-players.json", "ops.json", "usercache.json",
                       "whitelist.json"};
  for (QStringList::const_iterator i = files.cbegin(); i != files.cend(); ++i) {
    QString path = dir.absoluteFilePath(*i);
    if (!QFile::exists(path)) {
      QFile file(path);
      file.open(QIODevice::WriteOnly);
      file.write("");
      file.flush();
      file.close();
    }
  }
  dir.mkdir("logs");
}

void MainWindow::renameServerFolder(QString prev, QString act) {
  QDir dir;
  dir.rename(prev, act);
}

void MainWindow::createNewMapFolder(QString server, QString map) {
  QDir dir;
  dir.mkpath(server + "/" + map);
}

void MainWindow::renameMapFolder(QString server, QString prev, QString act) {
  QDir dir;
  dir.cd(server);
  dir.rename(prev, act);
}

void MainWindow::showGlobalEditContextMenu(QPoint pos) {
  globalEditMenu->popup(mapToGlobal(pos));
}

void MainWindow::showCustomEditContextMenu(QPoint pos) {
  if (customMenu)
    customMenu->popup(mapToGlobal(pos));
}

void MainWindow::on_action_new_database_triggered() {
  if (promptIfChangesPending()) {
    while (tree->topLevelItemCount() > 0)
      tree->deleteItem(tree->topLevelItem(0));

    loadedFile = "";
    onDatabaseSaved();
  }
}

void MainWindow::on_action_open_triggered() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Open file"),
                                                  QString(),
                                                  tr("MCServerAdmin Database Files (*.mcsadb)"));

  if (fileName.length() > 0) {
    on_action_new_database_triggered();

    if (!modifiedDatabase) {
      if (loadDatabase(fileName)) {
        loadedFile = fileName;
        onDatabaseSaved();
      }
    }
  }
}

void MainWindow::on_action_save_triggered() {
  if (loadedFile.length() != 0)
    saveDatabase(loadedFile);
  else
    on_action_save_as_triggered();
}

void MainWindow::on_action_save_as_triggered() {
  QString fileName = QFileDialog::getSaveFileName(this, tr("Save file"),
                                                  "database.mcsadb",
                                                  tr("MCServerAdmin Database Files (*.mcsadb)"));
  if (fileName.length() != 0)
    saveDatabase(fileName);
}

void MainWindow::on_action_import_database_triggered() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Import database"),
                                                  QString(),
                                                  tr("MCServerAdmin Database Files (*.mcsadb)"));

  if (fileName.length() > 0) {
    if (loadDatabase(fileName)) {
      loadedFile = "";
      onDatabaseModified();
    }
  }
}

void MainWindow::on_action_import_server_properties_triggered() {
  QString fileName = QFileDialog::getOpenFileName(this, tr("Import server.properties"),
                                                  QString(),
                                                  tr("server.properties"));
  if (fileName.length() != 0) {
    if (!tree->importServerProperties(fileName))
      QMessageBox::warning(this, tr("Error importing server.properties"),
                           tr("The \"server.properties\" specified could not be read. Please "
                              "make sure you have read permissions in the file and the file's "
                              "contents are correct."), QMessageBox::Ok);
  }
}

void MainWindow::on_action_about_triggered() {
  QMessageBox::about(this, tr("About MCServerAdmin"),
                     tr("<b>MCServerAdmin 0.5</b><br/>"
                        "Copyright 2014 Sergio M. Afonso Fumero. All rights reserved.<br/><br/>"
                        "This program is free software: you can redistribute it and/or modify<br/>"
                        "it under the terms of the GNU General Public License as published by<br/>"
                        "the Free Software Foundation, either version 3 of the License, or<br/>"
                        "(at your option) any later version.<br/><br/>"
                        "This program is distributed in the hope that it will be useful,<br/>"
                        "but WITHOUT ANY WARRANTY; without even the implied warranty of<br/>"
                        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the<br/>"
                        "GNU General Public License for more details."));
}

void MainWindow::on_action_web_page_triggered() {
  QDesktopServices::openUrl(QUrl("http://skatrak.github.io/mcserveradmin.html"));
}

bool MainWindow::promptIfChangesPending() {
  bool non_saved_changes = false;
  for (int i = 0, sz = tree->topLevelItemCount(); i < sz; ++i) {
    QTreeWidgetItem* server = tree->topLevelItem(i);
    if (tree->extractStatusData(server) == ModifiedStatus) {
      non_saved_changes = true;
      break;
    }
    for (int j = 0, sz2 = server->childCount(); j < sz2; ++j) {
      if (tree->extractStatusData(server->child(j)) == ModifiedStatus) {
        non_saved_changes = true;
        goto endfor;
      }
    }
  }
  endfor:

  if (non_saved_changes) {
    int result = QMessageBox::question(this, tr("Unsaved changes"),
                                       tr("There are currently changes not saved into the database. Do you want to save them?"),
                                       QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    switch(result) {
    case QMessageBox::Yes:
      onSaveAllTriggered();
      on_action_save_triggered();
      break;
    case QMessageBox::No:
      break;
    case QMessageBox::Cancel:
      return false;
    }
  }

  if (modifiedDatabase) {
    int result = QMessageBox::question(this, tr("Unsaved database"),
                                       tr("The database currently active has unsaved changes. Do you want to save it?"),
                                       QMessageBox::Yes, QMessageBox::No, QMessageBox::Cancel);
    switch(result) {
    case QMessageBox::Yes:
      on_action_save_triggered();
    case QMessageBox::No:
      return true;
    case QMessageBox::Cancel:
      return false;
    }
  }

  return true;
}

void MainWindow::createLanguageList() {
  // We create an exclusive action group to let the user only select one of
  // the languages at a time. Also this allows us to connect the triggered()
  // signal of any of the languages actions to the same slot in one sentence.
  languagesActions = new QActionGroup(ui->menuLanguage);
  languagesActions->setExclusive(true);

  // By default the interface is in English, so there's no need for
  // translation files. We add it manually and select it unless there's a more
  // appropriate translation available.
  QAction* lang = new QAction("English", this);
  lang->setCheckable(true);
  lang->setData("en");
  lang->setChecked(true);
  languagesActions->addAction(lang);

  // We find the language we would like to translate the ui to.
  QString default_name = QLocale::system().name();
  default_name.truncate(default_name.lastIndexOf('_'));

  // We find the translation files we've got in order to create the languages
  // list.
  QStringList fileNames = QDir(":/tr").entryList();
  fileNames.replaceInStrings("mcserveradmin.", "");
  fileNames.replaceInStrings(".qm", "");

  for (int i = 0; i < fileNames.size(); ++i) {
    QLocale loc(fileNames[i]);

    // TODO Manejador de eventos que retraduzca la interfaz
    lang = new QAction(loc.languageToString(loc.language()), this);
    lang->setCheckable(true);
    lang->setData(fileNames[i]);

    languagesActions->addAction(lang);

    // If the language found is the one that we wanted, we select it because
    // this is the language that was loaded by default at startup.
    if (default_name == fileNames[i])
        lang->setChecked(true);
  }

  // We add the languages to the languages menu
  ui->menuLanguage->addActions(languagesActions->actions());
}

void MainWindow::setupActions() {
  addServerAction = new QAction(tr("Add server"), this);
  addServerAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::CTRL + Qt::Key_S));
  connect(addServerAction, SIGNAL(triggered()), this, SLOT(onAddServerTriggered()));

  saveAllAction = new QAction(tr("Save all"), this);
  saveAllAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_S);
  connect(saveAllAction, SIGNAL(triggered()), this, SLOT(onSaveAllTriggered()));

  discardAllAction = new QAction(tr("Discard all"), this);
  discardAllAction->setShortcut(Qt::CTRL + Qt::SHIFT + Qt::Key_D);
  connect(discardAllAction, SIGNAL(triggered()), this, SLOT(onDiscardAllTriggered()));

  addMapAction = new QAction(tr("Add map"), this);
  addMapAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::CTRL + Qt::Key_M));
  connect(addMapAction, SIGNAL(triggered()), this, SLOT(onAddMapTriggered()));

  saveServerChangesAction = new QAction(tr("Save changes", "Server changes"), this);
  saveServerChangesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::Key_S));
  connect(saveServerChangesAction, SIGNAL(triggered()), this, SLOT(onSaveServerChangesTriggered()));

  discardServerChangesAction = new QAction(tr("Discard changes", "Server changes"), this);
  discardServerChangesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::Key_D));
  connect(discardServerChangesAction, SIGNAL(triggered()), this, SLOT(onDiscardServerChangesTriggered()));

  removeServerAction = new QAction(tr("Remove server"), this);
  removeServerAction->setShortcut(Qt::Key_Delete);
  connect(removeServerAction, SIGNAL(triggered()), this, SLOT(onRemoveServerTriggered()));

  setActiveAction = new QAction(tr("Set active"), this);
  setActiveAction->setShortcut(Qt::CTRL + Qt::Key_A);
  connect(setActiveAction, SIGNAL(triggered()), this, SLOT(onSetActiveMapTriggered()));

  saveMapChangesAction = new QAction(tr("Save changes", "Map changes"), this);
  saveMapChangesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::Key_S));
  connect(saveMapChangesAction, SIGNAL(triggered()), this, SLOT(onSaveMapChangesTriggered()));

  discardMapChangesAction = new QAction(tr("Discard changes", "Map changes"), this);
  discardMapChangesAction->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_K, Qt::Key_D));
  connect(discardMapChangesAction, SIGNAL(triggered()), this, SLOT(onDiscardMapChangesTriggered()));

  removeMapAction = new QAction(tr("Remove map"), this);
  removeMapAction->setShortcut(Qt::Key_Delete);
  connect(removeMapAction, SIGNAL(triggered()), this, SLOT(onRemoveMapTriggered()));
}
