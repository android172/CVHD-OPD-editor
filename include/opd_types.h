#ifndef __OPD_TYPES_H__
#define __OPD_TYPES_H__

#include <QVector>
#include <list>
#include <array>

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
    ushort index = (ushort) -1;
    uchar  size  = 0;

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
typedef std::list<Palette>::iterator PalettePtr;

struct GFXPage {
  public:
    ushort  index = (ushort) -1;
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
typedef std::list<GFXPage>::iterator GFXPagePtr;

struct Sprite {
    ushort     index = (ushort) -1;
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
typedef std::list<Sprite>::iterator SpritePtr;

struct Frame {
    struct Part {
        ushort     index = (ushort) -1;
        SpritePtr  sprite;
        PalettePtr palette;
        short      x_offset;
        short      y_offset;
        uchar      flip_mode;
    };

    struct HitBox {
        ushort index = (ushort) -1;
        ushort x_position;
        ushort y_position;
        ushort width;
        ushort height;
    };

    ushort  index = (ushort) -1;
    QString name;
    short   x_offset;
    short   y_offset;

    std::list<Part>   parts;
    std::list<HitBox> hitboxes;

    uint uses = 0;

    void initialize();
};
typedef std::list<Frame>::iterator FramePtr;

struct Animation {
    struct Frame {
        ushort   index = (ushort) -1;
        FramePtr data;
        ushort   delay;
        short    x_offset;
        short    y_offset;
    };

    ushort           index = (ushort) -1;
    QString          name;
    std::list<Frame> frames;
};

typedef std::list<Animation>::iterator        AnimationPtr;
typedef std::list<Animation::Frame>::iterator AnimationFramePtr;
typedef std::list<Frame::Part>::iterator      FramePartPtr;
typedef std::list<Frame::HitBox>::iterator    HitBoxPtr;

class Invalid {
  public:
    static const ushort            index = (ushort) -1;
    static const AnimationPtr      animation;
    static const FramePtr          frame;
    static const AnimationFramePtr animation_frame;
    static const FramePartPtr      frame_part;
    static const HitBoxPtr         hitbox;
    static const SpritePtr         sprite;
    static const PalettePtr        palette;

  private:
    static std::list<Animation>        animations_d;
    static std::list<Frame>            frames_d;
    static std::list<Animation::Frame> animation_frames_d;
    static std::list<Frame::Part>      frame_parts_d;
    static std::list<Frame::HitBox>    hitbox_d;
    static std::list<Sprite>           sprite_d;
    static std::list<Palette>          palette_d;
};

#define define_invalid(type, type_ptr, name, dummy_list)                       \
    inline std::list<type> Invalid::dummy_list { {} };                         \
    inline const type_ptr  Invalid::name = dummy_list.begin();

define_invalid(Animation, AnimationPtr, animation, animations_d);
define_invalid(Frame, FramePtr, frame, frames_d);
define_invalid(Animation::Frame, AnimationFramePtr, animation_frame, animation_frames_d);
define_invalid(Frame::Part, FramePartPtr, frame_part, frame_parts_d);
define_invalid(Frame::HitBox, HitBoxPtr, hitbox, hitbox_d);
define_invalid(Sprite, SpritePtr, sprite, sprite_d);
define_invalid(Palette, PalettePtr, palette, palette_d);

#define check_if_valid(opd_type)                                               \
    if (opd_type->index == Invalid::index) return

#endif // __OPD_TYPES_H__
