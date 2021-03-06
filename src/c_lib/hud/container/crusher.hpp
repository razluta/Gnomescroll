/* Gnomescroll, Copyright (c) 2013 Symbolic Analytics
 * Licensed under GPLv3 */
#pragma once

#include <item/properties.hpp>

namespace HudContainer
{

class CrusherUI : public UIElement
{
    public:

        static const int cell_size = ITEM_ICON_RENDER_SIZE + 5;
        int xdim;    // grid cell size
        int ydim;
        int alt_xdim;
        int alt_ydim;

        // size of texture/render area
        float render_width;
        float render_height;

        static const int slot_size = ITEM_ICON_RENDER_SIZE;
        static const int cell_offset_x = 3;
        static const int cell_offset_y = 3;
        static const int cell_offset_x_right = 2;
        static const int cell_offset_y_bottom = 2;

        // hardcoded sprite indices. multiply by cell size to get pixel index
        static const int input_sprite_x = 0;
        static const int input_sprite_y = 0;
        static const int input_overlay_x = 2;
        static const int input_overlay_y = 0;
        static const int button_inactive_x = 0;
        static const int button_inactive_y = 1;
        static const int button_inactive_hover_x = 0;
        static const int button_inactive_hover_y = 2;
        static const int button_available_x = 1;
        static const int button_available_y = 1;
        static const int button_available_hover_x = 1;
        static const int button_available_hover_y = 2;
        static const int button_unavailable_x = 2;
        static const int button_unavailable_y = 1;
        static const int button_unavailable_hover_x = 2;
        static const int button_unavailable_hover_y = 2;
        static const int button_overlay_x = 3;
        static const int button_overlay_y = 1;
        static const int button_overlay_pressed_x = 3;
        static const int button_overlay_pressed_y = 2;

        static const struct Dim texture_dim;

        float texture_offset_x;
        float texture_offset_y;

        bool centered;

    void draw();

    int width()
    {
        return render_width;
    }

    int height()
    {
        return render_height;
    }

    int get_grid_at(int px, int py)
    {
        //pixels from upper left
        px -= xoff;
        py -= _yresf - yoff;

        if (px < 0 || px >= render_width)  return NULL_SLOT;
        if (py < 0 || py >= render_height) return NULL_SLOT;

        int xslot = px / cell_size;
        int yslot = py / cell_size;
        int slot = xslot + yslot * xdim;

        return slot;
    }

    bool in_button_region(int px, int py)
    {
        return (this->get_grid_at(px,py) == 1);
    }

    int get_slot_at(int px, int py)
    {
        int slot = this->get_grid_at(px,py);
        if (slot == 0) return 0;
        return NULL_SLOT;
    }

    bool point_inside(int px, int py)
    {
        return (this->get_grid_at(px,py) != NULL_SLOT);
    }

    void init()
    {
        this->init_item_labels(this->xdim * this->ydim);
        this->refresh_render_size();
        if (this->centered) this->center();
        this->texture = &CrusherTexture;
    }

    void center()
    {
        int x = _xresf - this->width();
        float off = 0;
        if (x % 2) off = 1;
        this->xoff = x/2 + off;
    }

    void refresh_render_size()
    {
        this->render_width = this->cell_size * this->xdim;
        this->render_height = this->cell_size * (this->ydim + this->alt_ydim + 1);
    }

    void set_container_type(ItemContainerType container_type)
    {
        if (this->container_type == container_type) return;

        this->container_type = container_type;

        this->xdim = ItemContainer::get_container_xdim(container_type);
        this->ydim = ItemContainer::get_container_ydim(container_type);
        this->alt_xdim = ItemContainer::get_container_alt_xdim(container_type);
        this->alt_ydim = ItemContainer::get_container_alt_ydim(container_type);

        this->init();
    }

    virtual void draw_name()
    {
        glColor4ub(255, 255, 255, 255);
        HudFont::start_font_draw();
        HudFont::reset_default();
        HudFont::set_texture();
        this->name.set_position(this->xoff + (this->width() - this->name.get_width())/2, this->yoff + this->name.get_height() + 2);
        this->name.draw();
        HudFont::end_font_draw();
    }

    CrusherUI() :
        xdim(1), ydim(1), alt_xdim(0), alt_ydim(0),
        render_width(1.0f), render_height(1.0f),
        texture_offset_x(0.0f), texture_offset_y(0.0f),
        centered(false)
    {}

    virtual ~CrusherUI()
    {
    }
};

const struct Dim CrusherUI::texture_dim = { 256, 256 };

void CrusherUI::draw()
{
    IF_ASSERT(this->texture == NULL || *this->texture == 0) return;

    this->draw_name();

    if (this->container_id == NULL_CONTAINER) return;

    // get data for rendering items
    int slot_max = ItemContainer::get_container_ui_slot_max(this->container_id);
    IF_ASSERT(slot_max <= 0) return;
    struct ItemContainer::SlotMetadata* slot_metadata =
        ItemContainer::get_container_ui_slot_metadata(container_id);
    if (slot_metadata == NULL) return;

    glDisable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBindTexture(GL_TEXTURE_2D, *this->texture);

    glColor4ub(255, 255, 255, 255);

    float sw = cell_size;
    float sh = cell_size;
    float tw = ((float)cell_size)/float(this->texture_dim.x);
    float th = ((float)cell_size)/float(this->texture_dim.x);

    float x = xoff;
    float y = yoff - sh;    // -sh because draw_bound_texture_sprite draws bottom up
    float z = -0.1f;

    // draw input slot
    draw_bound_texture_sprite(x,y, sw,sh, z, input_sprite_x*tw, input_sprite_y*th, tw, th);

    // draw button
    y -= cell_size;
    if (this->in_button_region(mouse_x, mouse_y))
    {   // draw hover sprite
        if (slot_metadata[0].type != NULL_ITEM_TYPE)
            draw_bound_texture_sprite(x,y, sw,sh, z, button_available_hover_x*tw, button_available_hover_y*th, tw, th);
        else
            draw_bound_texture_sprite(x,y, sw,sh, z, button_inactive_hover_x*tw, button_inactive_hover_y*th, tw, th);
        if (lm_down) // draw button
            draw_bound_texture_sprite(x,y, sw,sh, z, button_overlay_pressed_x*tw, button_overlay_pressed_y*th, tw,th);
        else
            draw_bound_texture_sprite(x,y, sw,sh, z, button_overlay_x*tw, button_overlay_y*th, tw,th);
    }
    else
    {
        if (slot_metadata[0].type != NULL_ITEM_TYPE)
            draw_bound_texture_sprite(x,y, sw,sh, z, button_available_x*tw, button_available_y*th, tw, th);
        else
            draw_bound_texture_sprite(x,y, sw,sh, z, button_inactive_x*tw, button_inactive_y*th, tw, th);
        draw_bound_texture_sprite(x,y, sw,sh, z, button_overlay_x*tw, button_overlay_y*th, tw,th);
    }

    glDisable(GL_TEXTURE_2D);

    // draw hover highlight
    if (!this->in_button_region(mouse_x, mouse_y))
    {
        int hover_slot = this->get_grid_at(mouse_x, mouse_y);
        if (hover_slot != NULL_SLOT)
        {
            int w = slot_size;
            int xslot = hover_slot % this->xdim;
            int yslot = hover_slot / this->xdim;

            const float x = xoff + cell_size*xslot + cell_offset_x;
            const float y = yoff - (cell_size*yslot + cell_offset_y);

            glColor4ub(160, 160, 160, 128);
            glBegin(GL_QUADS);
            glVertex2f(x,y);
            glVertex2f(x, y-w);
            glVertex2f(x+w, y-w);
            glVertex2f(x+w, y);
            glEnd();
        }
    }

    glColor4ub(255, 255, 255, 255);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, TextureSheetLoader::item_texture_sheet_loader->texture);

    glBegin(GL_QUADS);

    //draw items
    for (int slot=0; slot<slot_max; slot++)
    {
        int xslot = 0;
        int yslot = slot;

        ItemType item_type = slot_metadata[slot].type;

        if (item_type == NULL_ITEM_TYPE) continue;
        int tex_id = Item::get_sprite_index_for_type(item_type);

        const float x = xoff + cell_offset_x + cell_size*xslot;
        const float y = yoff - (cell_offset_y + cell_size*yslot);

        const float w = slot_size;
        const float iw = 16.0f; // icon_width
        const int iiw = 16; // integer icon width

        const float tx_min = (1.0f/iw)*(tex_id % iiw);
        const float ty_min = (1.0f/iw)*(tex_id / iiw);
        const float tx_max = tx_min + 1.0f/iw;
        const float ty_max = ty_min + 1.0f/iw;

        glTexCoord2f(tx_min, ty_min);
        glVertex2f(x, y);

        glTexCoord2f(tx_min, ty_max);
        glVertex2f(x,y-w);

        glTexCoord2f(tx_max, ty_max);
        glVertex2f(x+w, y-w);

        glTexCoord2f(tx_max, ty_min);
        glVertex2f(x+w, y);
    }
    glEnd();

    glBindTexture(GL_TEXTURE_2D, CrusherTexture);

    // draw input overlay
    draw_bound_texture_sprite(xoff, yoff-sw, sw, sh, -0.1f, input_overlay_x*tw, input_overlay_y*th, tw, th);

    glDisable(GL_TEXTURE_2D);

    // draw item_labels
    HudFont::start_font_draw(GL_ONE_MINUS_DST_COLOR);
    const int font_size = 12;
    HudFont::set_properties(font_size);
    HudFont::set_texture();

    for (int slot=0; slot<slot_max; slot++)
    {
        int xslot = 0;
        int yslot = slot;
        int stack = slot_metadata[slot].stack_size;
        int charges = slot_metadata[slot].charges;
        HudText::Text* text = &this->item_labels[slot];
        const float x = xoff + cell_size*(xslot+1) - cell_offset_x_right - text->get_width();
        const float y = yoff - (cell_size*(yslot+1) - cell_offset_y_bottom - text->get_height());
        draw_slot_numbers(text, x, y, stack, charges);
    }

    HudFont::reset_default();
    HudFont::end_font_draw();

    glEnable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);

    glDisable(GL_TEXTURE_2D);

    //draw_tracking_pixel(xoff,yoff);
    glColor4ub(255, 255, 255, 255);

}

}   // HudContainer
