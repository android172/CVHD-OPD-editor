#include "opd/opd_types.h"

#define define_invalid(type, name, dummy_list)                                 \
    std::list<type>                 Invalid::dummy_list { {} };                \
    const std::list<type>::iterator Invalid::name = dummy_list.begin();

define_invalid(GFXPage, gfx_page, gfx_pages_d);
define_invalid(Animation, animation, animations_d);
define_invalid(Frame, frame, frames_d);
define_invalid(Animation::Frame, animation_frame, animation_frames_d);
define_invalid(Frame::Part, frame_part, frame_parts_d);
define_invalid(Frame::HitBox, hitbox, hitboxes_d);
define_invalid(Sprite, sprite, sprites_d);
define_invalid(Palette, palette, palettes_d);