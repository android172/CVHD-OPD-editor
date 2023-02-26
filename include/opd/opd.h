#ifndef __OPD_H__
#define __OPD_H__

#include "opd_types.h"

class Opd {
  public:
    ~Opd();

    constexpr static const uchar palette_max = 128;

    std::list<GFXPage>&   gfx_pages;
    std::list<Sprite>&    sprites;
    std::list<Frame>&     frames;
    std::list<Animation>& animations;

    std::array<Palette, palette_max>& palettes;
    uchar                             palette_count = 0;

    static Opd* open(QString const& path);

    AnimationPtr      add_new_animation();
    FramePtr          add_new_frame();
    SpritePtr         add_new_sprite();
    PalettePtr        add_new_palette();
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
    Opd(std::list<GFXPage>&               gfx_pages,
        std::list<Sprite>&                sprites,
        std::list<Frame>&                 frames,
        std::list<Animation>&             animations,
        std::array<Palette, palette_max>& palettes,
        uchar                             palette_count);
};

#endif // __OPD_H__