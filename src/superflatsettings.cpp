#include "superflatsettings.h"

SuperflatSettings::SuperflatSettings(QString text) {
  if (text != "")
    setStringRepresentation(text);
}

SuperflatSettings::SuperflatSettings(const SuperflatSettings& other)
  : layers(other.layers) {}

bool SuperflatSettings::setStringRepresentation(QString text) {
  // TODO
  return true;
}

QString SuperflatSettings::getStringRepresentation() const {
  // TODO
  return QString();
}

void SuperflatSettings::addTopLayer(BlockType element, int height) {
  if (layers.empty() || layers.last().first != element)
    layers.push_back(QPair<BlockType, int>(element, height));
  else
    layers.last().second += height;
}

void SuperflatSettings::addLayer(int position, BlockType element, int height) {
  if (position >= layers.size())
    addTopLayer(element, height);
  else if (position <= 0)
    addBottomLayer(element, height);
  else {
    if (layers.at(position).first == element)
      layers[position].second += height;
    else if (layers.at(position-1).first == element)
      layers[position-1].second += height;
    else
      layers.insert(position, QPair<BlockType, int>(element, height));
  }
}

void SuperflatSettings::addBottomLayer(BlockType element, int height) {
  if (layers.empty() || layers.first().first != element)
    layers.push_front(QPair<BlockType, int>(element, height));
  else
    layers.first().second += height;
}

void SuperflatSettings::setLayerElement(int layer, BlockType element) {
  if (layer >= 0 && layer < layers.size())
    layers[layer].first = element;
}

void SuperflatSettings::setLayerSize(int layer, int size) {
  if (layer >= 0 && layer < layers.size())
    layers[layer].second = size;
}

QPair<BlockType, int> SuperflatSettings::getLayerInfo(int layer) const {
  if (layer >= 0 && layer < layers.size())
    return layers.at(layer);
  else return QPair<BlockType, int>(INVALIDBLOCKTYPE, 0);
}

BlockType SuperflatSettings::getLayerElement(int layer) const {
  if (layer >= 0 && layer < layers.size())
    return layers.at(layer).first;
  else return INVALIDBLOCKTYPE;
}

int SuperflatSettings::getLayerHeight(int layer) const {
  if (layer >= 0 && layer < layers.size())
    return layers.at(layer).second;
  else return 0;
}

int SuperflatSettings::getLayerCount() const {
  return layers.size();
}

QList< QPair<BlockType, int> > SuperflatSettings::getLayers() const {
  return layers;
}
