#ifndef __OPD_H__
#define __OPD_H__

#include "opd_types.h"

class Opd {
  public:
    ~Opd();

    constexpr static const uchar palette_max = UCHAR_MAX;
    constexpr static const uchar gfx_w       = 128;
    constexpr static const uchar gfx_h       = 128;

    const QString file_name;
    const QString file_path;

    std::list<GFXPage>&   gfx_pages;
    std::list<Sprite>&    sprites;
    std::list<Frame>&     frames;
    std::list<Animation>& animations;

    std::array<Palette, palette_max>& palettes;
    uchar                             palette_count = 0;

    static Opd* open(QString const& path);
    static Opd* copy(const Opd* other);
    void        save();

    AnimationPtr      add_new_animation();
    FramePtr          add_new_frame();
    SpritePtr         add_new_sprite();
    AnimationFramePtr add_new_animation_frame(
        const AnimationPtr animation, const FramePtr frame = Invalid::frame
    );
    FramePartPtr add_new_frame_part(
        const FramePtr   frame,
        const SpritePtr  sprite  = Invalid::sprite,
        const PalettePtr palette = Invalid::palette
    );
    HitBoxPtr add_new_hitbox(const FramePtr frame);

  private:
    static const QString _original_type;

    QString _palette_file;
    QString _gfx_page_file;

    Opd(const QString&                    file_name,
        const QString&                    path,
        std::list<GFXPage>&               gfx_pages,
        std::list<Sprite>&                sprites,
        std::list<Frame>&                 frames,
        std::list<Animation>&             animations,
        std::array<Palette, palette_max>& palettes,
        uchar                             palette_count);

    void save_palettes(const QString& palette_dir);
    void recompute_gfx_pages();

    bool insert_sprite_into_gfx(
        const Sprite& sprite, std::list<std::list<uchar>>& alignment_rows
    );
};

#endif // __OPD_H__