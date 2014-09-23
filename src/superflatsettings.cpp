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

#include "superflatsettings.h"

SuperflatSettings::SuperflatSettings(QString text) {
  if (text != "")
    setStringRepresentation(text);
  else {
    // by default we have a "classic flat" configuration
    biome = PLAINS;
    addTopLayer(BEDROCK, 1);
    addTopLayer(DIRT, 2);
    addTopLayer(GRASSBLOCK, 1);
  }
}

SuperflatSettings::SuperflatSettings(const SuperflatSettings& other)
  : layers(other.layers) {}

bool SuperflatSettings::setStringRepresentation(QString text) {
  // TODO
  return true;
}

QString SuperflatSettings::getStringRepresentation() const {
  QString rep;

  // version
  rep += "3;";

  // layers
  for (QList<QPair <BlockType, int> >::const_iterator i = layers.cbegin(); i != layers.cend(); ++i) {
    if (i->second > 1)
      rep += QString::number(i->second) + "*";
    rep += QString::number((int) i->first) + ",";
  }
  // substitute the comma for a semicolon
  rep[rep.length()-1] = ';';

  // biome
  rep += QString::number((int) biome) + ";";

  // TODO structures

  return rep;
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
