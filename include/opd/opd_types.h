#ifndef __OPD_TYPES_H__
#define __OPD_TYPES_H__

#include "animation.h"
#include "sprite.h"

class Invalid {
  public:
    static const ushort            index = (ushort) -1;
    static const GFXPagePtr        gfx_page;
    static const AnimationPtr      animation;
    static const FramePtr          frame;
    static const AnimationFramePtr animation_frame;
    static const FramePartPtr      frame_part;
    static const HitBoxPtr         hitbox;
    static const SpritePtr         sprite;
    static const PalettePtr        palette;

  private:
    static std::list<GFXPage>          gfx_pages_d;
    static std::list<Animation>        animations_d;
    static std::list<Frame>            frames_d;
    static std::list<Animation::Frame> animation_frames_d;
    static std::list<Frame::Part>      frame_parts_d;
    static std::list<Frame::HitBox>    hitboxes_d;
    static std::list<Sprite>           sprites_d;
};

#define check_if_valid(opd_type)                                               \
    if (opd_type->index == Invalid::index) return

#endif // __OPD_TYPES_H__
