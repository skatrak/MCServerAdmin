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
