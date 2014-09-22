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
