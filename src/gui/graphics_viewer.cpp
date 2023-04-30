#include "gui/graphics_viewer.h"

#include <QScrollBar>

GraphicsViewer::GraphicsViewer(QWidget* parent) : QGraphicsView(parent) {
    QGraphicsScene* scene = new QGraphicsScene();
    setScene(scene);

    // Setup selection
    _selection_pen.setWidthF(1.5);
    _selection_pen.setColor(Qt::blue);

    activate_pan();
    set_scene_rectangle();
}
GraphicsViewer::~GraphicsViewer() { clear(); }

// ///////////////////// //
// GRAPHICS VIEWER SLOTS //
// ///////////////////// //

void GraphicsViewer::wheelEvent(QWheelEvent* event) {
    QPointF current_center = mapToScene(viewport()->rect().center());

    // Scale up / down
    if (event->angleDelta().y() > 0) scale(1.1, 1.1);
    else scale(0.9, 0.9);

    // Set minimum and maximum scale
    if (transform().m11() > 100.0f) {
        resetTransform();
        scale(100.0f, 100.0f);
        centerOn(current_center);
    } else if (transform().m11() < 0.5f) {
        resetTransform();
        scale(0.5f, 0.5f);
        centerOn(current_center);
    }

    // Calculate the desired pen width in pixels
    const auto desiredPenWidth = 1.5; // adjust this as needed
    const auto pixelWidth      = desiredPenWidth / transform().m11();
    _selection_pen.setWidthF(pixelWidth);

    // Update selection rectangle if necessary
    if (_selection_rect != nullptr) _selection_rect->setPen(_selection_pen);
}

void GraphicsViewer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Plus) scale(1.1, 1.1);
    else if (event->key() == Qt::Key_Minus) scale(0.9, 0.9);
    else if (_current_control_mode == ControlMode::Move && !_in_move_mode) {
        if (event->key() == Qt::Key_Up) _on_move_preformed(0, -1, true);
        else if (event->key() == Qt::Key_Down) _on_move_preformed(0, 1, true);
        else if (event->key() == Qt::Key_Left) _on_move_preformed(-1, 0, true);
        else if (event->key() == Qt::Key_Right) _on_move_preformed(1, 0, true);
    }
}

#include <QDebug>

void GraphicsViewer::mousePressEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (_current_control_mode == ControlMode::Select) {
            if (_selection_rect == nullptr) return;

            // Get pivot
            _selection_pivot = mapToScene(event->pos()).toPoint();
            _selection_rect->setRect(
                _selection_pivot.x(), _selection_pivot.y(), 1, 1
            );
            _in_selection_mode = true;

            // Delete selection lines
            scene()->removeItem(_selection_x_line);
            scene()->removeItem(_selection_y_line);
            delete _selection_x_line;
            delete _selection_y_line;
            _selection_x_line = nullptr;
            _selection_y_line = nullptr;
        } else if (_current_control_mode == ControlMode::Move) {
            // Get pivot
            _move_pivot   = mapToScene(event->pos()).toPoint();
            _in_move_mode = true;
            _on_move_preformed(0, 0, true);
        } else if (_current_control_mode == ControlMode::Pan)
            ;
    }
    QGraphicsView::mousePressEvent(event);
}
void GraphicsViewer::mouseMoveEvent(QMouseEvent* event) {
    if (_current_control_mode == ControlMode::Select) {
        if (_in_selection_mode) {
            if (_selection_rect == nullptr) return;

            // Update selection rectangle
            const auto end_point = mapToScene(event->pos()).toPoint();
            const auto x         = qMin(end_point.x(), _selection_pivot.x());
            const auto y         = qMin(end_point.y(), _selection_pivot.y());
            const auto width  = qAbs(end_point.x() - _selection_pivot.x()) + 1;
            const auto height = qAbs(end_point.y() - _selection_pivot.y()) + 1;

            _selection_rect->setRect(x, y, width, height);
        } else {
            // Draw selection lines
            const auto pos = mapToScene(event->pos()).toPoint();
            _selection_x_line->setLine(-10000, pos.y(), 10000, pos.y());
            _selection_y_line->setLine(pos.x(), -10000, pos.x(), 10000);
        }
    } else if (_current_control_mode == ControlMode::Move) {
        if (_in_move_mode) {
            // Get distance traveled
            const auto end_point = mapToScene(event->pos()).toPoint();
            const auto delta_x   = end_point.x() - _move_pivot.x();
            const auto delta_y   = end_point.y() - _move_pivot.y();

            // Update pivot
            _move_pivot.setX(end_point.x());
            _move_pivot.setY(end_point.y());

            // Move current object
            _on_move_preformed(delta_x, delta_y, false);
        }
    } else if (_current_control_mode == ControlMode::Pan)
        ;
    QGraphicsView::mouseMoveEvent(event);
}
void GraphicsViewer::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        if (_current_control_mode == ControlMode::Select) {
            // Activate on completion callback
            const auto rect = _selection_rect->rect();
            _on_selection_preformed(
                (short) rect.x(),
                (short) rect.y(),
                (ushort) qMax(rect.width(), 0.0),
                (ushort) qMax(rect.height(), 0.0)
            );

            // Back to pan
            activate_pan();
        } else if (_current_control_mode == ControlMode::Move) {
            // Finish movement
            _in_move_mode = false;

            // Set rectangle
            set_scene_rectangle();
        } else
            ;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// ////////////////////////////// //
// GRAPHICS VIEWER PUBLIC METHODS //
// ////////////////////////////// //

void GraphicsViewer::show_frame(
    const Frame& frame, const ushort current_index, const uint mode
) {
    auto scene = this->scene();
    clear_state();
    scene->clear();

    // Draw all frame parts
    for (auto const& part : frame.parts) {
        const auto& sprite  = *part.sprite;
        const auto& palette = *part.palette;

        // Compute pixmap for sprite
        const auto pixmap     = QPixmap::fromImage(sprite.to_image(palette));
        const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

        // Transform pixmap properly
        const auto flip_x = (part.flip_mode & 2) ? -1 : 1;
        const auto flip_y = (part.flip_mode & 1) ? -1 : 1;

        QTransform t = QTransform();
        t.translate(1.0 * sprite.width / 2, 1.0 * sprite.height / 2);
        t.scale(flip_x, flip_y);
        t.translate(-1.0 * sprite.width / 2, -1.0 * sprite.height / 2);
        t.translate(
            flip_x * (part.x_offset - frame.x_offset),
            flip_y * (part.y_offset - frame.y_offset)
        );

        pixmap_gpi->setTransform(t);

        // Add to scene
        scene->addItem(pixmap_gpi);
    }

    // Draw hitboxes
    if (mode == 2) {
        for (auto const& hitbox : frame.hitboxes) {
            // Add hitbox rectangle
            auto hitbox_rectangle = new QGraphicsRectItem(
                hitbox.x_position - frame.x_offset,
                hitbox.y_position - frame.y_offset,
                hitbox.width,
                hitbox.height
            );
            hitbox_rectangle->setOpacity(0.2);
            hitbox_rectangle->setBrush(Qt::red);
            scene->addItem(hitbox_rectangle);

            // Show currently selected hitbox
            // Only visible if move control mode is active
            if (hitbox.index == current_index) {
                add_selection(
                    hitbox.x_position - frame.x_offset,
                    hitbox.y_position - frame.y_offset,
                    hitbox.width,
                    hitbox.height
                );
                _selection_rect->setVisible(
                    _current_control_mode == ControlMode::Move
                );
            }
        }
    }
    // Add selection box if required (NOTE: Needs to be added at the end)
    else if (mode == 1) {
        for (auto const& part : frame.parts)
            if (part.index == current_index) {
                // Show currently selected frame part
                // Only visible if move control mode is active
                add_selection(
                    part.x_offset - frame.x_offset,
                    part.y_offset - frame.y_offset,
                    part.sprite->width,
                    part.sprite->height
                );
                _selection_rect->setVisible(
                    _current_control_mode == ControlMode::Move
                );
            }
    }

    // Set rectangle
    set_scene_rectangle();
}

void GraphicsViewer::show_sprite(const Sprite& sprite, const Palette& palette) {
    auto scene = this->scene();
    clear_state();
    scene->clear();

    if (sprite.width == 0 || sprite.height == 0) return;

    // Get pixmap
    const auto pixmap =
        QPixmap::fromImage(sprite.to_image(palette, with_background));
    const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

    // Show as currently selected if in MOVE mode
    add_selection(0, 0, sprite.width, sprite.height);
    _selection_rect->setVisible(_current_control_mode == ControlMode::Move);

    scene->addItem(pixmap_gpi);

    // Set rectangle
    set_scene_rectangle();
}

void GraphicsViewer::show_image(
    const PixelMap& pixels, const Palette& palette
) {
    auto scene = this->scene();
    activate_pan();
    clear_state();
    scene->clear();

    if (pixels.size() == 0 || pixels[0].size() == 0) return;

    // Compute image
    auto   height = pixels.size();
    auto   width  = pixels[0].size();
    QImage image  = QImage(width, height, QImage::Format_RGBA8888);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            // Compute color index
            const auto color_index = pixels[i][j];

            // Get color from index
            const auto color = palette.get_color(color_index);

            // Compute alpha
            const uchar alpha = (with_background || color_index) ? 0xff : 0x00;

            // Set pixel
            const QColor q_color { color.r, color.g, color.b, alpha };
            image.setPixel(j, i, q_color.rgba());
        }
    }

    // Get pixmap from image
    const auto pixmap     = QPixmap::fromImage(image);
    const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

    // Show
    scene->addItem(pixmap_gpi);

    // Set rectangle
    set_scene_rectangle();
}

void GraphicsViewer::show_csr(const GFXPage& csr, const Palette& palette) {
    auto scene = this->scene();
    scene->clear();

    // Compute image
    const QImage image = csr.to_image(palette, with_background);

    // Get pixmap from image
    const auto pixmap     = QPixmap::fromImage(image);
    const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

    // Show
    scene->addItem(pixmap_gpi);

    // Set rectangle
    set_scene_rectangle();
}

void GraphicsViewer::clear() {
    auto scene = this->scene();
    clear_state();
    scene->clear();
}

void GraphicsViewer::reset_view() {
    resetTransform();
    centerOn(0, 0);
}

void GraphicsViewer::add_sprite(
    const Sprite& sprite, const Palette& palette, const float alpha
) {
    auto scene = this->scene();
    clear_state();

    if (sprite.width == 0 || sprite.height == 0) return;

    // Compute pixmap
    const auto pixmap =
        QPixmap::fromImage(sprite.to_image(palette, with_background, alpha));
    const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

    // Show as currently selected if in MOVE mode
    add_selection(0, 0, sprite.width, sprite.height);
    _selection_rect->setVisible(_current_control_mode == ControlMode::Move);

    scene->addItem(pixmap_gpi);
}
void GraphicsViewer::add_selection(
    const short  x_pos,
    const short  y_pos,
    const ushort width,
    const ushort height
) {
    auto scene = this->scene();

    auto rectangle = new QGraphicsRectItem(x_pos, y_pos, width, height);

    if (_selection_rect != nullptr) delete _selection_rect;
    _selection_rect = rectangle;

    rectangle->setPen(_selection_pen);
    rectangle->setBrush(Qt::transparent);

    // Create a QTimer to periodically update the pen dash offset
    _selection_timer = new QTimer();
    connect(_selection_timer, &QTimer::timeout, [=]() {
        static int offset = 0;
        _selection_pen.setDashOffset(offset);
        offset += 5; // adjust the offset amount as needed
        rectangle->setPen(_selection_pen);
    });
    _selection_timer->start(100); // adjust the timer interval as needed

    scene->addItem(rectangle);
}

void GraphicsViewer::activate_pan() {
    if (_current_control_mode == ControlMode::Select) {
        _in_selection_mode = false;

        // Clear selection lines if already present
        if (_selection_x_line) {
            scene()->removeItem(_selection_x_line);
            delete _selection_x_line;
            _selection_x_line = nullptr;
        }
        if (_selection_y_line) {
            scene()->removeItem(_selection_y_line);
            delete _selection_y_line;
            _selection_y_line = nullptr;
        }
    } else if (_current_control_mode == ControlMode::Move) {
        if (_selection_rect) _selection_rect->setVisible(false);
    }

    // Back to pan
    _current_control_mode = ControlMode::Pan;
    setDragMode(QGraphicsView::ScrollHandDrag);
}

void GraphicsViewer::activate_selection(
    const std::function<void(short, short, ushort, ushort)>&
        on_selection_preformed
) {
    // Change current mode to select
    _current_control_mode   = ControlMode::Select;
    _on_selection_preformed = on_selection_preformed;

    // Clear selection lines if already present
    if (_selection_x_line) {
        scene()->removeItem(_selection_x_line);
        delete _selection_x_line;
    }
    if (_selection_y_line) {
        scene()->removeItem(_selection_y_line);
        delete _selection_y_line;
    }

    // Create new selection lines
    _selection_x_line = new QGraphicsLineItem {};
    _selection_y_line = new QGraphicsLineItem {};
    _selection_x_line->setPen(_selection_pen);
    _selection_y_line->setPen(_selection_pen);

    scene()->addItem(_selection_x_line);
    scene()->addItem(_selection_y_line);

    // Disable drag
    setDragMode(QGraphicsView::NoDrag);
}

void GraphicsViewer::activate_move(
    const std::function<void(short, short, bool)>& on_move_preformed
) {
    // Change current mode to move
    _current_control_mode = ControlMode::Move;
    _on_move_preformed    = on_move_preformed;

    // Set visiblity of selection
    if (_selection_rect) _selection_rect->setVisible(true);

    // Disable drag
    setDragMode(QGraphicsView::NoDrag);
}

void GraphicsViewer::grid_set_xy() {
    _current_grid_mode = GridMode::XYAxis;
    viewport()->update();
}
void GraphicsViewer::grid_set_full() {
    _current_grid_mode = GridMode::FullGrid;
    viewport()->update();
}
void GraphicsViewer::grid_disable() {
    _current_grid_mode = GridMode::NoGrid;
    viewport()->update();
}

QString GraphicsViewer::get_current_control_mod() const {
    switch (_current_control_mode) {
    case ControlMode::Pan: return "Pan";
    case ControlMode::Move: return "Move";
    case ControlMode::Select: return "Select";
    }
    return "Error";
}
QString GraphicsViewer::get_current_grid_mod() const {
    switch (_current_grid_mode) {
    case GridMode::NoGrid: return "NoGrid";
    case GridMode::XYAxis: return "XYAxis";
    case GridMode::FullGrid: return "FullGrid";
    }
    return "Error";
}

// /////////////////////////////// //
// GRAPHICS VIEWER PRIVATE METHODS //
// /////////////////////////////// //

void GraphicsViewer::drawBackground(QPainter* painter, const QRectF& rect) {
    // Draws background differently if grid is enabled
    if (_current_grid_mode == GridMode::NoGrid) return; // Not enabled
    const auto scale = transform().m11();

    // Get pen
    QPen pen(Qt::DotLine);
    pen.setWidthF(1.5f / scale);

    // Expand top / bottom / left / right
    const auto left   = int(rect.left() - 1.0f);
    const auto right  = int(rect.right() + 1.0f);
    const auto top    = int(rect.top() - 1.0f);
    const auto bottom = int(rect.bottom() + 1.0f);

    // Draw grid
    if (_current_grid_mode == GridMode::FullGrid) {
        pen.setColor(QColor(200, 200, 200));
        painter->setPen(pen);

        const auto grid_scale = (scale < 1)    ? 20
                                : (scale < 3)  ? 10
                                : (scale < 7)  ? 5
                                : (scale < 12) ? 2
                                               : 1;

        // Draw vertical lines
        for (int x = left - left % grid_scale; x < right; x += grid_scale)
            painter->drawLine(x, top, x, bottom);

        // Draw horizontal lines
        for (int y = top - top % grid_scale; y < bottom; y += grid_scale)
            painter->drawLine(left, y, right, y);
    }

    // Draw X & Y axis
    pen.setColor(Qt::red);
    painter->setPen(pen);
    painter->drawLine(0, top, 0, bottom);

    pen.setColor(Qt::green);
    painter->setPen(pen);
    painter->drawLine(left, 0, right, 0);
}

// /////////////////////////////// //
// GRAPHICS VIEWER PRIVATE METHODS //
// /////////////////////////////// //

void GraphicsViewer::clear_state() {
    // Stop timer
    if (_selection_timer) {
        _selection_timer->stop();
        _selection_timer->disconnect();
        delete _selection_timer;
        _selection_timer = nullptr;
    }

    // Delete rectangle
    if (_selection_rect) {
        delete _selection_rect;
        _selection_rect = nullptr;
    }
}

void GraphicsViewer::set_scene_rectangle() {
    if (_in_move_mode == false) {
        const auto rect = scene()->itemsBoundingRect();
        setSceneRect(QRectF(
            rect.x() - 500.0f,
            rect.y() - 500.0f,
            1000.0f + rect.width(),
            1000.0f + rect.height()
        ));
    }
}