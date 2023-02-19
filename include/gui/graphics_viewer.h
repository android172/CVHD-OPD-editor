#ifndef __GRAPHICS_VIEWER_H__
#define __GRAPHICS_VIEWER_H__

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>

#include "opd.h"

class GraphicsViewer : public QGraphicsView {
    Q_OBJECT
  public:
    explicit GraphicsViewer(QWidget* parent);
    ~GraphicsViewer();

    void show_frame(const Frame& frame);
    void show_sprite(const Sprite& sprite, const Palette& palette);

    bool hitbox_visible = false;

  protected Q_SLOTS:
    void wheelEvent(QWheelEvent* event);
    void keyPressEvent(QKeyEvent* event);
};

#endif // __GRAPHICS_VIEWER_H__