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
