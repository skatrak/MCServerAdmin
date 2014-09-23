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

#include "servertreedelegate.h"

#include <QLineEdit>
#include <QValidator>

ServerTreeDelegate::ServerTreeDelegate(QObject* parent)
  : QStyledItemDelegate(parent) {}

ServerTreeDelegate::~ServerTreeDelegate() {}

QWidget* ServerTreeDelegate::createEditor(QWidget* parent,
                                      const QStyleOptionViewItem& option,
                                      const QModelIndex& index) const {
  if (index.column() == 0) {
    QWidget* editor = QStyledItemDelegate::createEditor(parent, option, index);
    QLineEdit* ed;
    if ((ed = qobject_cast<QLineEdit*>(editor)) != NULL)
      ed->setValidator(new QRegExpValidator(QRegExp("[^\\\\/:*?\"<>|]+"), editor));

    return editor;
  }

  return 0;
}
