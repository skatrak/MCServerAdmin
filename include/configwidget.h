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
