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
    bool with_background = false;

    explicit GraphicsViewer(QWidget* parent);
    ~GraphicsViewer();

    // Show element
    void show_frame(
        const Frame& frame, const ushort current_index, const uint mode
    );
    void show_sprite(const Sprite& sprite, const Palette& palette);
    void show_image(const PixelMap& pixels, const Palette& palette);
    void show_csr(const GFXPage& csr, const Palette& palette);

    // Add element
    void add_sprite(
        const Sprite& sprite, const Palette& palette, const float alpha = 1.0f
    );
    void add_selection(
        const short  x_pos,
        const short  y_pos,
        const ushort width,
        const ushort height
    );

    // Clear all
    void clear();
    void reset_view();

    // Change mode
    void activate_pan();
    void activate_selection(
        const std::function<void(short, short, ushort, ushort)>&
            on_selection_preformed
    );
    void activate_move(
        const std::function<void(short, short, bool)>& on_move_preformed
    );

    // Change grid
    void grid_set_xy();
    void grid_set_full();
    void grid_disable();

    // Getters
    QString get_current_control_mod() const;
    QString get_current_grid_mod() const;

  protected Q_SLOTS:
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

  protected:
    void drawBackground(QPainter* painter, const QRectF& rect) override;

  private:
    enum class ControlMode { Pan, Move, Select };
    ControlMode _current_control_mode = ControlMode::Pan;

    enum class GridMode { NoGrid, XYAxis, FullGrid };
    GridMode _current_grid_mode = GridMode::NoGrid;

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
    std::function<void(short, short, bool)> _on_move_preformed;
    bool                                    _in_move_mode = false;
    QPoint                                  _move_pivot {};

    void clear_state();
    void set_scene_rectangle();
};

#endif // __GRAPHICS_VIEWER_H__