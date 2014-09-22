#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "configwidget.h"

namespace Ui {
class MainWindow;
}

class QAction;
class QMenu;
class QTreeWidgetItem;
class ServerTree;

class MainWindow : public QMainWindow {
  Q_OBJECT

public:
  explicit MainWindow(QWidget* parent = 0);
  ~MainWindow(void);

protected:
  void closeEvent(QCloseEvent* event);

public slots:
  bool saveDatabase(QString fileName);
  bool loadDatabase(QString fileName);

  void onCurrentChanged(QTreeWidgetItem* cur, QTreeWidgetItem* prev);
  void onItemNameChanged(QString name);
  void onModifiedDataChanged();

  void onDatabaseModified();
  void onDatabaseSaved();

  // Slots para las opciones generales
  void onAddServerTriggered();
  void onSaveAllTriggered();
  void onDiscardAllTriggered();

  // Slots para las opciones de los servidores
  void onAddMapTriggered();
  void onSaveServerChangesTriggered();
  void onDiscardServerChangesTriggered();
  void onRemoveServerTriggered();

  // Slots para las opciones de los mapas
  void onSetActiveMapTriggered();
  void onSaveMapChangesTriggered();
  void onDiscardMapChangesTriggered();
  void onRemoveMapTriggered();

protected slots:
  void createNewServerFolder(QString server);
  void renameServerFolder(QString prev, QString act);
  void createNewMapFolder(QString server, QString map);
  void renameMapFolder(QString server, QString prev, QString act);

private slots:
  void showGlobalEditContextMenu(QPoint pos);
  void showCustomEditContextMenu(QPoint pos);

  // Slots para los menús fijos
  void on_action_new_database_triggered();
  void on_action_open_triggered();
  void on_action_save_triggered();
  void on_action_save_as_triggered();
  void on_action_import_database_triggered();
  void on_action_import_server_properties_triggered();
  void on_action_about_triggered();
  void on_action_web_page_triggered();

private:
  bool promptIfChangesPending();
  void createLanguageList();
  void setupActions();

  Ui::MainWindow* ui;
  ServerTree* tree;
  ConfigWidget* rightWidget;
  QString loadedFile;
  bool modifiedDatabase;

  QMenu* globalEditMenu;
  QMenu* customMenu;
  QAction* customMenuAction;

  QAction* addServerAction;
  QAction* saveAllAction;
  QAction* discardAllAction;

  QAction* addMapAction;
  QAction* saveServerChangesAction;
  QAction* discardServerChangesAction;
  QAction* removeServerAction;

  QAction* setActiveAction;
  QAction* saveMapChangesAction;
  QAction* discardMapChangesAction;
  QAction* removeMapAction;

};

#endif // MAINWINDOW_H
