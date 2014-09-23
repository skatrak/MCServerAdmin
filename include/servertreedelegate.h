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

#ifndef SERVERTREEDELEGATE_H
#define SERVERTREEDELEGATE_H

#include <QStyledItemDelegate>

class ServerTreeDelegate: public QStyledItemDelegate {
public:
  ServerTreeDelegate(QObject* parent = 0);
  virtual ~ServerTreeDelegate();

  virtual QWidget* createEditor(QWidget* parent,
                                const QStyleOptionViewItem& option,
                                const QModelIndex& index) const;
};

#endif // SERVERTREEDELEGATE_H
