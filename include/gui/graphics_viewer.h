#ifndef __GRAPHICS_VIEWER_H__
#define __GRAPHICS_VIEWER_H__

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QWheelEvent>
#include <QTimer>

#include <functional>

#include "opd/opd.h"

class GraphicsViewer : public QGraphicsView {
    Q_OBJECT
  public:
    // Draw settings
    bool   hitbox_visible  = false;
    bool   with_background = false;
    ushort current_index   = 0;

    explicit GraphicsViewer(QWidget* parent);
    ~GraphicsViewer();

    void show_frame(const Frame& frame);
    void show_sprite(const Sprite& sprite, const Palette& palette);
    void show_image(const PixelMap& pixels, const QVector<ColorPair>& palette);

    void clear();

    void add_sprite(
        const Sprite& sprite, const Palette& palette, const float alpha = 1.0f
    );
    void add_selection(
        const short  x_pos,
        const short  y_pos,
        const ushort width,
        const ushort height
    );

    void activate_pan();
    void activate_selection(
        const std::function<void(short, short, ushort, ushort)>&
            on_selection_preformed
    );
    void activate_move(
        const std::function<void(short, short)>& on_move_preformed
    );

  protected Q_SLOTS:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

  private:
    enum class ControlMode { Pan, Move, Select };
    ControlMode _current_mode = ControlMode::Pan;

    // Selection
    std::function<void(short, short, ushort, ushort)> _on_selection_preformed;
    bool               _in_selection_mode = false;
    QPoint             _selection_pivot {};
    QPen               _selection_pen { Qt::DashLine };
    QTimer*            _selection_timer  = nullptr;
    QGraphicsRectItem* _selection_rect   = nullptr;
    QGraphicsLineItem* _selection_x_line = nullptr;
    QGraphicsLineItem* _selection_y_line = nullptr;

    // Move
    std::function<void(short, short)> _on_move_preformed;
    bool                              _in_move_mode = false;
    QPoint                            _move_pivot {};
    QGraphicsItem*                    _moving_object = nullptr;

    void clear_state();
};

#endif // __GRAPHICS_VIEWER_H__