#include "opd/opd.h"

#include "file_utils.h"
#include "util.h"

const QString Opd::_original_type = QString("original");

// Constructor & Destructor
Opd::Opd(
    const QString&                    file_name,
    const QString&                    path,
    std::list<GFXPage>&               gfx_pages,
    std::list<Sprite>&                sprites,
    std::list<Frame>&                 frames,
    std::list<Animation>&             animations,
    std::array<Palette, palette_max>& palettes,
    uchar                             palette_count
)
    : file_name(file_name), file_path(path), gfx_pages(gfx_pages),
      sprites(sprites), frames(frames), animations(animations),
      palettes(palettes), palette_count(palette_count) {}
Opd::~Opd() {}

// ////////////////// //
// OPD PUBLIC METHODS //
// ////////////////// //

// Helper functions
std::array<Palette, Opd::palette_max>* read_palettes(QString const& path);

Opd* Opd::open(const QString& path) {
    const QString file_name = path.split('/').last().toLower();
    const QString file_dir  = path.chopped(file_name.size());
    std::ifstream opd_file { path.toStdString(),
                             std::ios::binary | std::ios::in };
    if (!opd_file.is_open()) throw std::runtime_error("File not found.");

    // === Sections ===
    opd_file.seekg(0x24);
    const auto gibberish_offs  = read_type<uint>(opd_file);
    const auto gibberish_size  = read_type<uint>(opd_file);
    const auto palette_offs    = read_type<uint>(opd_file);
    const auto palette_size    = read_type<uint>(opd_file);
    const auto gfx_pages_offs  = read_type<uint>(opd_file);
    const auto gfx_pages_size  = read_type<uint>(opd_file);
    const auto frames_offs     = read_type<uint>(opd_file);
    const auto frames_size     = read_type<uint>(opd_file);
    const auto animations_offs = read_type<uint>(opd_file);
    const auto animations_size = read_type<uint>(opd_file);

    // === Gibberish section (SKIP) ===
    // opd_file.seekg(gibberish_offs);

    // === Palette section ===
    // Read name
    opd_file.seekg(palette_offs + 5);
    const auto palette_name = read_type<QString>(opd_file).toLower();
    const auto palette_path = file_dir + palette_name;

    // Read palettes
    const auto palettes = read_palettes(palette_path);

    // === GFX page section ===
    // Parse header
    opd_file.seekg(gfx_pages_offs);
    const auto gfx_count = read_type<ushort>(opd_file);
    opd_file.seekg(gfx_pages_offs + 16);

    // Read pages
    const auto gfx_pages = new std::list<GFXPage> {};
    for (auto i = 0; i < gfx_count; i++) {
        GFXPage gfx {};
        gfx.index  = i;
        gfx.name   = read_type<QString>(opd_file);
        gfx.dir    = file_dir;
        gfx.width  = read_type<ushort>(opd_file);
        gfx.height = read_type<ushort>(opd_file);
        read_type<uchar>(opd_file); // Unknown
        gfx.initialize();
        gfx_pages->push_back(gfx);
    }

    // === Frame section ===
    // Parse header
    opd_file.seekg(frames_offs);
    const auto frame_count = read_type<ushort>(opd_file);
    opd_file.seekg(frames_offs + 16);

    // Keeping track of all sprites
    const auto sprites = new std::list<Sprite> {};

    // And of palette count
    auto palette_count = 0;

    // Read frames
    const auto frames = new std::list<Frame> {};
    for (auto i = 0; i < frame_count; i++) {
        Frame frame {};
        frame.index    = i;
        frame.name     = read_type<QString>(opd_file);
        frame.x_offset = read_type<short>(opd_file);
        frame.y_offset = read_type<short>(opd_file);
        read_type<ushort>(opd_file); // Unknown

        // Read counts
        const auto hit_box_count = read_type<ushort>(opd_file);
        const auto part_count    = read_type<ushort>(opd_file);
        read_type<ushort>(opd_file); // Unknown

        // Read hit boxes
        for (auto j = 0; j < hit_box_count; j++) {
            Frame::HitBox hit_box {};
            hit_box.index      = j;
            hit_box.x_position = read_type<short>(opd_file);
            hit_box.y_position = read_type<short>(opd_file);
            hit_box.width      = read_type<ushort>(opd_file);
            hit_box.height     = read_type<ushort>(opd_file);
            frame.hitboxes.push_back(hit_box);
        }

        // Read parts
        for (auto j = 0; j < part_count; j++) {
            Frame::Part part {};
            part.index    = j;
            part.x_offset = read_type<short>(opd_file);
            part.y_offset = read_type<short>(opd_file);

            // Read sprite
            Sprite sprite {};
            sprite.gfx_page =
                get_it_at(*gfx_pages, read_type<ushort>(opd_file));
            sprite.gfx_x_pos = read_type<ushort>(opd_file);
            sprite.gfx_y_pos = read_type<ushort>(opd_file);
            sprite.width     = read_type<ushort>(opd_file);
            sprite.height    = read_type<ushort>(opd_file);

            // Handling of invalid gfx pages
            if (sprite.gfx_page == gfx_pages->end()) {
                sprite.gfx_page = Invalid::gfx_page;
                sprite.width = sprite.height = 0;
            }

            // Set sprite
            auto sprite_i = std::find(sprites->begin(), sprites->end(), sprite);
            if (sprite_i == sprites->end()) {
                sprite.index = (ushort) sprites->size();
                sprite.uses  = 0;
                sprite.initialize();
                sprites->push_back(sprite);
                sprite_i = sprites->end();
                sprite_i--;
            }
            sprite_i->uses++;
            part.sprite = sprite_i;

            // read palette
            const auto palette_index = read_type<uchar>(opd_file);
            if (palette_index >= palette_count)
                palette_count = palette_index + 1;
            part.palette = &(*palettes)[palette_index];

            // Read rest
            part.flip_mode = read_type<uchar>(opd_file);
            // Unknown
            read_type<ushort>(opd_file);
            read_type<ushort>(opd_file);
            read_type<ushort>(opd_file);
            frame.parts.push_back(part);
        }
        frame.initialize();
        frames->push_back(frame);
    }

    // === Palette size ===
    // Set palette color count for unused palettes
    // These "in between" palettes are most likely used for recolors, and thus
    // just copy the count of last used palette
    auto palette_running_size = 0;
    for (auto& palette : *palettes) {
        if (palette.size == 0) palette.size = palette_running_size;
        else palette_running_size = palette.size;
    }

    // === Animation section ===
    // Parse header
    opd_file.seekg(animations_offs);
    const auto animation_count = read_type<ushort>(opd_file);
    opd_file.seekg(animations_offs + 16);

    // Read animations
    const auto animations = new std::list<Animation> {};
    for (auto i = 0; i < animation_count; i++) {
        Animation animation {};
        animation.index = i;
        animation.name  = read_type<QString>(opd_file);

        // Read count
        const auto anim_frame_count = read_type<ushort>(opd_file);
        read_type<uint>(opd_file); // Unknown

        // Read frames
        for (auto j = 0; j < anim_frame_count; j++) {
            Animation::Frame frame {};
            frame.index = j;
            frame.data  = get_it_at(*frames, read_type<ushort>(opd_file));
            frame.delay = read_type<ushort>(opd_file);
            read_type<uchar>(opd_file); // Unknown
            frame.x_offset = read_type<short>(opd_file);
            frame.y_offset = read_type<short>(opd_file);
            read_type<uint>(opd_file); // Unknown
            read_type<uint>(opd_file); // Unknown
            read_type<uint>(opd_file); // Unknown

            // Mark frame as used
            frame.data->uses++;

            animation.frames.push_back(frame);
        }
        animations->push_back(animation);
    }

    opd_file.close();

    return new Opd(
        file_name,
        path,
        *gfx_pages,
        *sprites,
        *frames,
        *animations,
        *palettes,
        palette_count
    );
}

void Opd::save() {
    const QString file_name = file_path.split('/').last().toLower().chopped(4);
    const QString file_dir  = file_path.chopped(file_name.size() + 4);
    const QString file_type = file_name.split('_').last().toLower();

    // Separate original from modded
    QString path     = file_path;
    QString opd_name = file_name;
    opd_name         = opd_name.remove(0, 2); // remove p_
    if (file_type.compare(Opd::_original_type) == 0) {
        // Remove _original.opd (1 + original + 4) modifier
        path.chop(Opd::_original_type.size() + 5);
        path += ".opd";
        // Remove same from opd name (1 + original)
        opd_name.chop(Opd::_original_type.size() + 1);
    } else {
        // Save original
        const QString original_path_str =
            file_dir + "/" + file_name + "_" + Opd::_original_type + ".opd";

        const auto original_path =
            std::filesystem::path(original_path_str.toStdString());

        // Save only if original opd doesn't exist already
        if (std::filesystem::exists(original_path) == false)
            std::filesystem::copy(
                std::filesystem::path(path.toStdString()), original_path
            );
    }

    // Compute file prefixes
    _palette_file  = "c_" + opd_name + "_mod";
    _gfx_page_file = "f_" + opd_name + "_mod";

    // This will create a file if one doesn't exist
    std::fstream opd_file { path.toStdString(),
                            std::ios::binary | std::ios::out | std::ios::in };
    if (!opd_file.is_open()) throw std::runtime_error("File not found.");

    // === Palette section ===
    // Skip gibberish section
    opd_file.seekg(0x2C);
    // Get palette offset
    const uint palette_offs = read_type<uint>(opd_file);
    opd_file.seekp(palette_offs);

    // Write header
    skip_over(opd_file, 5);                       // Unknown
    write_type(opd_file, _palette_file + ".col"); // Name
    skip_over(opd_file, 194);                     // Gibberish

    // Get palette size
    const uint palette_size = (uint) opd_file.tellp() - palette_offs;

    // === GFX section ===
    // Get gfx page offset
    const uint gfx_pages_offs = opd_file.tellp();

    // Compute new gfx pages
    recompute_gfx_pages();

    // GFX page header
    write_type(opd_file, (ushort) gfx_pages.size()); // Gfx page count
    write_type(opd_file, (uchar) 38);                // Gfx page size
    skip_over(opd_file, 13);                         // Padding

    // Write Gfx pages
    for (const auto& page : gfx_pages) {
        write_type(opd_file, page.name);   // Csr file name
        write_type(opd_file, page.width);  // Width
        write_type(opd_file, page.height); // Height
        skip_over(opd_file, 1);            // Unknown
    }

    // Get gfx page size
    const uint gfx_pages_size = (uint) opd_file.tellp() - gfx_pages_offs;

    // === Frame section ===
    // Get fame offset
    const uint frames_offs = opd_file.tellp();

    // Write header
    write_type(opd_file, (ushort) frames.size()); // Frame Count
    skip_over(opd_file, 14);                      // Padding

    // Write frames
    for (const auto& frame : frames) {
        write_type(opd_file, frame.name);                     // Name
        write_type(opd_file, frame.x_offset);                 // X offset
        write_type(opd_file, frame.y_offset);                 // Y offset
        skip_over(opd_file, 2);                               // Unknown
        write_type(opd_file, (ushort) frame.hitboxes.size()); // Hitbox count
        write_type(opd_file, (ushort) frame.parts.size());    // Part count
        skip_over(opd_file, 2);                               // Unknown

        // Write hitboxes
        for (const auto& hitbox : frame.hitboxes) {
            write_type(opd_file, hitbox.x_position); // Y offset
            write_type(opd_file, hitbox.y_position); // X offset
            write_type(opd_file, hitbox.width);      // Width
            write_type(opd_file, hitbox.height);     // Height
        }

        // Write parts
        for (const auto& part : frame.parts) {
            write_type(opd_file, part.x_offset); // X offset
            write_type(opd_file, part.y_offset); // y offset

            // Sprite
            write_type(opd_file, part.sprite->gfx_page->index);
            write_type(opd_file, part.sprite->gfx_x_pos);
            write_type(opd_file, part.sprite->gfx_y_pos);
            write_type(opd_file, part.sprite->width);
            write_type(opd_file, part.sprite->height);

            write_type(opd_file, (uchar) part.palette->index); // Palette
            write_type(opd_file, part.flip_mode);              // Flip mode
            skip_over(opd_file, 6);                            // Unknown
        }
    }

    // Get frame size
    const uint frames_size = (uint) opd_file.tellp() - frames_offs;

    // === Animation section ===
    // Get animation offset
    const uint animations_offs = opd_file.tellp();

    // Write Animation header
    write_type(opd_file, (ushort) animations.size());
    skip_over(opd_file, 14); // Unknown + padding

    // Write animations
    for (const auto& animation : animations) {
        write_type(opd_file, animation.name);                   // Name
        write_type(opd_file, (ushort) animation.frames.size()); // Frame count
        skip_over(opd_file, 4);                                 // Unknown

        // Write animation frames
        for (const auto& frame : animation.frames) {
            write_type(opd_file, frame.data->index); // Frame index
            write_type(opd_file, frame.delay);       // Delay
            skip_over(opd_file, 1);                  // Unknown
            write_type(opd_file, frame.x_offset);    // X offset
            write_type(opd_file, frame.y_offset);    // Y offset
            write_type(opd_file, (ushort) 1);        // Always one
            skip_over(opd_file, 2);                  // Unknown
            write_type(opd_file, (ushort) 1);        // Always one
            skip_over(opd_file, 6);                  // Unknown
        }
    }

    // Get animation size
    const uint animations_size = (uint) opd_file.tellp() - animations_offs;

    // === Save header ===
    // Write file size
    const uint file_size = opd_file.tellp();
    opd_file.seekp(0x20);
    write_type(opd_file, file_size);

    // Skip gibberish section
    skip_over(opd_file, 8);

    // Write other sections
    write_type(opd_file, palette_offs);
    write_type(opd_file, palette_size);
    write_type(opd_file, gfx_pages_offs);
    write_type(opd_file, gfx_pages_size);
    write_type(opd_file, frames_offs);
    write_type(opd_file, frames_size);
    write_type(opd_file, animations_offs);
    write_type(opd_file, animations_size);

    opd_file.close();

    // === Write other files ===
    // Save csrs
    for (const auto& gfx_page : gfx_pages)
        gfx_page.save();

    // Save col
    save_palettes(file_dir);
}

// -----------------------------------------------------------------------------
// Add new
// -----------------------------------------------------------------------------

AnimationPtr Opd::add_new_animation() {
    animations.push_back({ (uchar) animations.size(), "", {} });
    auto new_animation = animations.end();
    new_animation--;
    return new_animation;
}
FramePtr Opd::add_new_frame() {
    frames.push_back({ (ushort) frames.size(), "", 0, 0, {}, {} });
    auto new_frame = frames.end();
    new_frame--;
    return new_frame;
}
SpritePtr Opd::add_new_sprite() {
    Sprite new_spr {};
    new_spr.index = sprites.size();

    sprites.push_back(new_spr);
    auto new_sprite = sprites.end();
    new_sprite--;
    return new_sprite;
}
AnimationFramePtr Opd::add_new_animation_frame(
    const AnimationPtr animation, const FramePtr frame
) {
    animation->frames.push_back(
        { (ushort) animation->frames.size(), frame, 0, 0, 0 }
    );
    auto new_animation_frame = animation->frames.end();
    new_animation_frame--;
    return new_animation_frame;
}
FramePartPtr Opd::add_new_frame_part(
    const FramePtr frame, const SpritePtr sprite, const PalettePtr palette
) {
    frame->parts.push_back(
        { (ushort) frame->parts.size(), sprite, palette, 0, 0, 0 }
    );
    auto new_frame_part = frame->parts.end();
    new_frame_part--;
    return new_frame_part;
}
HitBoxPtr Opd::add_new_hitbox(const FramePtr frame) {
    frame->hitboxes.push_back({ (uchar) frame->hitboxes.size(), 0, 0, 0 });
    auto new_hitbox = frame->hitboxes.end();
    new_hitbox--;
    return new_hitbox;
}

// /////////////////// //
// OPD PRIVATE METHODS //
// /////////////////// //

void Opd::save_palettes(const QString& palette_dir) {
    // Create / Open col file
    const auto    path = palette_dir + _palette_file + ".col";
    std::ofstream col_file { path.toStdString(),
                             std::ios::out | std::ios::binary };
    if (!col_file.is_open())
        throw std::runtime_error("Couldn't create a palette file.");

    // Unknown header
    write_type(col_file, (uchar) 0x03);
    write_type(col_file, (uchar) 0x74);
    // 6 unknown bytes
    fill_zeros(col_file, 6);
    // Unknown header
    write_type(col_file, (uchar) 0x06);
    // 22 unknown bytes
    fill_zeros(col_file, 22);

    // Compute multiplier
    // TODO: Give more then one option (For now always one)
    uchar const col_multiplier = 1;
    write_type(col_file, (uchar) 0);

    // Compute current palette hex
    std::string palette_hex {};
    for (const auto& palette : palettes) {
        for (const auto& color : palette) {
            palette_hex += color.b / col_multiplier;
            palette_hex += color.g / col_multiplier;
            palette_hex += color.r / col_multiplier;
            palette_hex += '\000';
        }
    }

    // Replace color bytes
    col_file.write(palette_hex.data(), palette_hex.size());
    col_file.close();
}

void Opd::recompute_gfx_pages() {
    // Get sorted list of sprites (By height desc)
    std::list<Sprite> sorted_sprites;
    for (const auto& sprite : sprites) {
        // Skip unused sprites
        if (sprite.uses <= 0) continue;

        // Copy sprite
        Sprite sprite_copy { sprite.index,
                             sprite.uses,
                             Invalid::gfx_page,
                             0,
                             0,
                             sprite.x_pos,
                             sprite.y_pos,
                             sprite.width,
                             sprite.height,
                             sprite.pixels };
        sprite_copy.fill_margin();

        // Add sorted
        auto insert_pos = std::lower_bound(
            sorted_sprites.begin(),
            sorted_sprites.end(),
            sprite_copy,
            [](const Sprite& s1, const Sprite& s2) {
                // Sorted by height desc
                if (s1.height == s2.height) return s1.width > s2.width;
                return s1.height > s2.height;
            }
        );
        sorted_sprites.insert(insert_pos, sprite_copy);
    }

    // Clear previous GFX page list
    const auto dir = gfx_pages.begin()->dir;
    gfx_pages.clear();

    // List of rows at which addition of top left image pixels is allowed
    std::list<std::list<uchar>> alignment_rows {};

    // Add sprites
    for (auto& sprite : sorted_sprites) {
        // Check if the sprite can fit into the container
        if (sprite.width > gfx_w || sprite.height > gfx_h)
            throw std::runtime_error("Sprite can't excide then 128x128");

        // Try to fit the sprite into an existing container
        bool place_found = insert_sprite_into_gfx(sprite, alignment_rows);

        // If the image couldn't fit into any existing container create new
        if (!place_found) {
            // Add gfx
            const ushort index = gfx_pages.size();
            gfx_pages.push_back({ index,
                                  _gfx_page_file + "_" +
                                      QString::number(index) + ".csr",
                                  dir,
                                  gfx_w,
                                  gfx_h,
                                  {} });
            const auto gfx_page = get_it_at(gfx_pages, gfx_pages.size() - 1);

            // Initialize gfx page to empty
            gfx_page->pixels.resize(gfx_page->height);
            for (auto i = 0; i < gfx_page->width; i++) {
                gfx_page->pixels[i].resize(gfx_page->width);
                for (auto j = 0; j < gfx_page->height; j++)
                    gfx_page->pixels[i][j] = (uchar) -1;
            }

            // Add the image to the container
            const auto sprite_it = get_it_at(sprites, sprite.index);
            sprite_it->gfx_page  = gfx_page;
            sprite_it->gfx_x_pos = 0;
            sprite_it->gfx_y_pos = 0;
            for (auto i = 0; i < sprite.height; i++) {
                for (auto j = 0; j < sprite.width; j++)
                    gfx_page->pixels[i][j] = sprite.pixels[i][j];
            }

            // Add next available row as alignment row (together with first row)
            alignment_rows.push_back({ 0, (uchar) sprite.height });
        }
    }

    // Replace all empty pixes with background
    for (auto& gfx_page : gfx_pages) {
        for (auto& row : gfx_page.pixels) {
            for (auto& pixel : row)
                if (pixel == (uchar) -1) pixel = 0;
        }
    }
}

bool Opd::insert_sprite_into_gfx(
    const Sprite& sprite, std::list<std::list<uchar>>& alignment_rows
) {
    auto alignment_row = alignment_rows.begin();
    for (auto& gfx_page : gfx_pages) {
        // Find the top-left corner of the sprite within the container
        for (uchar y : *alignment_row) {
            if (y >= gfx_page.height - sprite.height)
                break; // Wont fit on this page
            for (uchar x = 0; x < gfx_page.width - sprite.width; x++) {
                // Check if the sprite overlaps with any existing sprite
                if (gfx_page.pixels[y][x] == (uchar) -1) {
                    // Add the sprite to the container
                    const auto sprite_it = get_it_at(sprites, sprite.index);
                    sprite_it->gfx_page  = get_it_at(gfx_pages, gfx_page.index);
                    sprite_it->gfx_x_pos = x;
                    sprite_it->gfx_y_pos = y;
                    for (auto i = 0; i < sprite.height; i++) {
                        for (auto j = 0; j < sprite.width; j++)
                            gfx_page.pixels[y + i][x + j] = sprite.pixels[i][j];
                    }

                    // Add new alignment row if applicable
                    if (x == 0) alignment_row->push_back(y + sprite.height);

                    return true;
                }
            }
        }
        alignment_row++;
    }
    return false;
}

// //////////////////// //
// OPD HELPER FUNCTIONS //
// //////////////////// //

std::array<Palette, Opd::palette_max>* read_palettes(QString const& path) {
    // Load palettes
    std::ifstream col_file { path.toStdString(),
                             std::ios::in | std::ios::binary };
    if (!col_file.is_open())
        throw std::runtime_error(
            "Couldn't load palette file from path: \"" + path.toStdString() +
            "\"."
        );
    col_file.seekg(0x1F);

    // Compute multiplier
    uchar const col_multiplier = std::max(4U * read_type<uchar>(col_file), 1U);

    // Compute palettes
    uchar index    = 0;
    auto  palettes = new std::array<Palette, Opd::palette_max> {};
    for (auto& palette : *palettes) {
        palette.index = index++;
        for (auto& color : palette) {
            // read palette color
            uchar b = col_multiplier * read_type<uchar>(col_file);
            uchar g = col_multiplier * read_type<uchar>(col_file);
            uchar r = col_multiplier * read_type<uchar>(col_file);
            color   = { r, g, b };
            // read separation byte
            read_type<uchar>(col_file);
        }
    }

    col_file.close();
    return palettes;
}