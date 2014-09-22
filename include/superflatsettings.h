#ifndef SUPERFLATSETTINGS_H
#define SUPERFLATSETTINGS_H

#include <QList>
#include <QPair>
#include <QString>

class SuperflatSettings {
public:
  explicit SuperflatSettings(QString text = "");
  SuperflatSettings(const SuperflatSettings& other);

  bool setStringRepresentation(QString text);
  QString getStringRepresentation() const;

  void addTopLayer(int element, int height);
  void addLayer(int position, int element, int height);
  void addBottomLayer(int element, int height);

  void setLayerElement(int layer, int element);
  void setLayerSize(int layer, int size);

  QPair<int, int> getLayerInfo(int layer) const; // first: element, second: height
  int getLayerElement(int layer) const;
  int getLayerHeight(int layer) const;
  int getLayerCount() const;

  QList< QPair<int, int> > getLayers() const;

private:
  QList< QPair<int, int> > layers;

};

#endif // SUPERFLATSETTINGS_H
