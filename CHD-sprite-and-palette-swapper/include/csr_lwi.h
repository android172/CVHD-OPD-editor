#ifndef __CSR_LWI_H__
#define __CSR_LWI_H__

#include <QListWidgetItem>

// CSR list widget item
class CsrLWI : public QListWidgetItem {
  public:
    CsrLWI(const QString& name, const QString& path)
        : QListWidgetItem(name), name(name), path(path) {}

    const QString name;
    const QString path;
};

#endif // __CSR_LWI_H__