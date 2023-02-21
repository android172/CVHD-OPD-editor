#include "gui/graphics_viewer.h"

GraphicsViewer::GraphicsViewer(QWidget* parent) : QGraphicsView(parent) {
    setDragMode(QGraphicsView::ScrollHandDrag);
    QGraphicsScene* scene = new QGraphicsScene();
    setScene(scene);
}
GraphicsViewer::~GraphicsViewer() {}

// ///////////////////// //
// GRAPHICS VIEWER SLOTS //
// ///////////////////// //

void GraphicsViewer::wheelEvent(QWheelEvent* event) {
    if (event->angleDelta().y() > 0) scale(1.1, 1.1);
    else scale(0.9, 0.9);
}

void GraphicsViewer::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Plus) scale(1.1, 1.1);
    else if (event->key() == Qt::Key_Minus) scale(0.9, 0.9);
}

// ////////////////////////////// //
// GRAPHICS VIEWER PUBLIC METHODS //
// ////////////////////////////// //

QPixmap compute_pixel_map(
    const QVector<QVector<uchar>>& pixels,
    const Palette&                 palette,
    bool                           with_background = false
);

void GraphicsViewer::show_frame(const Frame& frame) {
    auto scene = this->scene();
    scene->clear();

    // Draw all frame parts
    for (auto const& part : frame.parts) {
        const auto& sprite  = *part.sprite;
        const auto& palette = *part.palette;

        const auto pixmap     = compute_pixel_map(sprite.pixels, palette);
        const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

        const auto flip_x = (part.flip_mode & 2) ? -1 : 1;
        const auto flip_y = (part.flip_mode & 1) ? -1 : 1;

        QTransform t = QTransform();
        t.translate(1.0 * sprite.width / 2, 1.0 * sprite.height / 2);
        t.scale(flip_x, flip_y);
        t.translate(-1.0 * sprite.width / 2, -1.0 * sprite.height / 2);
        t.translate(flip_x * part.x_offset, flip_y * part.y_offset);

        pixmap_gpi->setTransform(t);

        scene->addItem(pixmap_gpi);
    }

    // Draw hitboxes
    if (hitbox_visible) {
        for (auto const& hitbox : frame.hitboxes) {
            auto hitbox_rectangle = new QGraphicsRectItem(
                hitbox.x_position,
                hitbox.y_position,
                hitbox.width,
                hitbox.height
            );
            hitbox_rectangle->setOpacity(0.2);
            hitbox_rectangle->setBrush(Qt::red);
            scene->addItem(hitbox_rectangle);
        }
    }

    // Set into postion
    translate(frame.x_offset, frame.y_offset);
}

void GraphicsViewer::show_sprite(
    const Sprite& sprite, const Palette& palette, bool with_background
) {
    auto scene = this->scene();
    scene->clear();

    const auto pixmap =
        compute_pixel_map(sprite.pixels, palette, with_background);
    const auto pixmap_gpi = new QGraphicsPixmapItem(pixmap);

    scene->addItem(pixmap_gpi);
}

QPixmap compute_pixel_map(
    const QVector<QVector<uchar>>& pixels,
    const Palette&                 palette,
    bool                           with_background
) {
    auto   height = pixels.size();
    auto   width  = pixels[0].size();
    QImage image  = QImage(width, height, QImage::Format_RGBA8888);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto  color_index = pixels[i][j];
            Color color;
            if (color_index <= palette.size) color = palette[color_index];
            else
                color = { (uchar) (color_index * 17),
                          (uchar) (color_index * 17),
                          (uchar) (color_index * 17) };
            uchar  alpha = (with_background || color_index) ? 0xff : 0x00;
            QColor q_color { color.r, color.g, color.b, alpha };
            image.setPixel(j, i, q_color.rgba());
        }
    }

    return QPixmap::fromImage(image);
}
