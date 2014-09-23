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

#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[]) {
  QApplication app(argc, argv);

  QTranslator translator;
  translator.load(QLocale::system(), ":/tr/mcserveradmin", ".");
  app.installTranslator(&translator);

  MainWindow w;
  w.show();

  return app.exec();
}
