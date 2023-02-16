#include "gui/graphics_viewer.h"

GraphicsViewer::GraphicsViewer(QWidget* parent) : QGraphicsView(parent) {
    setDragMode(QGraphicsView::ScrollHandDrag);

    QGraphicsPixmapItem* pixmapItem =
        new QGraphicsPixmapItem(QPixmap("../.test_files/r1.png"));
    pixmapItem->setTransformationMode(Qt::FastTransformation);

    QGraphicsScene* scene = new QGraphicsScene();
    scene->addItem(pixmapItem);
    setScene(scene);
}
GraphicsViewer::~GraphicsViewer() {}

// ///////////////////// //
// GRAPHICS VIEWER SLOTS //
// ///////////////////// //

void GraphicsViewer::wheelEvent(QWheelEvent* event) {
    if (event->delta() > 0) scale(1.1, 1.1);
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
    const QVector<QVector<uchar>>& pixels, const Palette& palette
);

void GraphicsViewer::show_frame(const Frame& frame) {
    auto scene = this->scene();
    scene->clear();

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
    translate(frame.x_offset, frame.y_offset);
}

QPixmap compute_pixel_map(
    const QVector<QVector<uchar>>& pixels, const Palette& palette
) {
    auto   height = pixels.size();
    auto   width  = pixels[0].size();
    QImage image  = QImage(width, height, QImage::Format_RGBA8888);

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            auto  color_index = pixels[i][j];
            Color color;
            if (color_index < palette.size) color = palette[color_index];
            else
                color = { (uchar) (color_index * 17),
                          (uchar) (color_index * 17),
                          (uchar) (color_index * 17) };
            uchar  alpha = (color_index) ? 0xff : 0x00;
            QColor q_color { color.r, color.g, color.b, alpha };
            image.setPixel(j, i, q_color.rgba());
        }
    }

    return QPixmap::fromImage(image);
}