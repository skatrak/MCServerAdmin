#include "configwidget.h"

ConfigWidget::ConfigWidget(QWidget* parent): QTabWidget(parent), changed(false) {}

ConfigWidget::~ConfigWidget() {}

void ConfigWidget::onModification() {
  changed = true;
  emit(dataModified());
}
