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
