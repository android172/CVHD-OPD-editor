#ifndef __OPD_TYPES_H__
#define __OPD_TYPES_H__

#include <QVector>
#include <QLinkedList>

struct Color {
    uchar r;
    uchar g;
    uchar b;

    bool operator==(const Color& other) const;

    QString to_hex();
};
uint qHash(const Color& key, uint seed = 0);

struct ColorPair {
    Color original;
    Color display;

    bool operator==(const ColorPair& other) const;
    bool operator<(const ColorPair& other) const;
};

struct Palette {
    ushort index;
    uchar  size = 0;

    typedef std::array<Color, 16>::iterator       iterator;
    typedef std::array<Color, 16>::const_iterator const_iterator;

    iterator begin();
    iterator end();

    const_iterator begin() const;
    const_iterator end() const;

    Color&       operator[](size_t index);
    const Color& operator[](size_t index) const;

  private:
    std::array<Color, 16> _colors;
};
typedef QLinkedList<Palette>::iterator PalettePtr;

struct GFXPage {
  public:
    ushort  index;
    QString name;
    QString dir;
    ushort  width;
    ushort  height;

    QVector<QVector<uchar>> pixels;

    void initialize();

  private:
    ushort _previous_width  = 0;
    ushort _previous_height = 0;
};
typedef QLinkedList<GFXPage>::iterator GFXPagePtr;

struct Sprite {
    ushort     index;
    GFXPagePtr gfx_page;
    ushort     gfx_x_pos;
    ushort     gfx_y_pos;
    ushort     width;
    ushort     height;

    QVector<QVector<uchar>> pixels;

    bool operator==(const Sprite& other) const;

    void initialize();

  private:
    ushort _previous_width;
    ushort _previous_height;
};
typedef QLinkedList<Sprite>::iterator SpritePtr;

struct Frame {
    struct Part {
        ushort     index;
        SpritePtr  sprite;
        PalettePtr palette;
        short      x_offset;
        short      y_offset;
        uchar      flip_mode;
    };

    struct HitBox {
        ushort index;
        ushort x_position;
        ushort y_position;
        ushort width;
        ushort height;
    };

    ushort  index;
    QString name;
    short   x_offset;
    short   y_offset;

    QLinkedList<Part>   parts;
    QLinkedList<HitBox> hit_boxes;

    bool used = false;

    void initialize();
};
typedef QLinkedList<Frame>::iterator FramePtr;

struct Animation {
    struct Frame {
        ushort   index;
        FramePtr data;
        ushort   delay;
        short    x_offset;
        short    y_offset;
    };

    ushort             index;
    QString            name;
    QLinkedList<Frame> frames;
};

typedef QLinkedList<Animation>::iterator        AnimationPtr;
typedef QLinkedList<Animation::Frame>::iterator AnimationFramePtr;
typedef QLinkedList<Frame::Part>::iterator      FramePartPtr;

class InvalidPtr {
  public:
    static const AnimationPtr      animation;
    static const FramePtr          frame;
    static const AnimationFramePtr animation_frame;
    static const FramePartPtr      frame_part;
};

inline const auto InvalidPtr::animation       = AnimationPtr();
inline const auto InvalidPtr::frame           = FramePtr();
inline const auto InvalidPtr::animation_frame = AnimationFramePtr();
inline const auto InvalidPtr::frame_part      = FramePartPtr();

#endif // __OPD_TYPES_H__