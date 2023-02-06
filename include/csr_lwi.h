#ifndef __CSR_LWI_H__
#define __CSR_LWI_H__

#include <QListWidgetItem>

// CSR list widget item
class CsrLWI : public QListWidgetItem {
  public:
    CsrLWI(const QString& name, const QString& path);

    const QString name;
    const QString path;

    const QVector<QVector<uchar>>& pixels();
    void                           import_data();

  private:
    QVector<QVector<uchar>> _pixels { 128 };
};

#endif // __CSR_LWI_H__