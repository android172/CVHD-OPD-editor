#ifndef __CSR_LWI_H__
#define __CSR_LWI_H__

#include <QListWidgetItem>
#include "opd/gfx_page.h"

class CSRLwi : public QListWidgetItem {
  public:
    const GFXPagePtr csr;

    CSRLwi(const GFXPagePtr csr) : QListWidgetItem(), csr(csr) {
        compute_name();
    }
    ~CSRLwi() {}

    void compute_name() { setText(csr->name); }
};

#endif // __CSR_LWI_H__