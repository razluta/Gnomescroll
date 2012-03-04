#include "font.hpp"

#include <compat_gl.h>

namespace HudFont
{

static const char font_path[] = "./media/fonts/";
static int n_fonts = 0;
static Font** fonts = NULL;
static Font* default_font = NULL;
Font* font = NULL;

void Font::load_font_png()
{

    char path[strlen(font_path) + strlen(data.png) + 1];
    sprintf(path, "%s%s", font_path, data.png);

    SDL_Surface *surface = IMG_Load(path);

    if(!surface)
    {
        printf("Failed to IMG_Load surface %s. Error: %s\n", path, IMG_GetError());
        return;
    }

    if(surface->format->BytesPerPixel != 4)
    {
        printf("surface Image File: image is missing alpha channel \n");
        this->alpha = 0;
    }

    this->tex_width = surface->w;
    this->tex_height = surface->h;

    glEnable(GL_TEXTURE_2D);
    glGenTextures(1,&this->texture);
    glBindTexture(GL_TEXTURE_2D,this->texture);

    if (this->alpha)
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, //rgb
                     GL_UNSIGNED_BYTE, surface->pixels);
    else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, surface->w, surface->h, 0, GL_RGB, //rgb
                     GL_UNSIGNED_BYTE, surface->pixels);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glDisable(GL_TEXTURE_2D);

    SDL_FreeSurface(surface);

    printf("Loaded surface %s\n", data.png);
}

void Font::parse_font_file()
{
    int size = 0;
    char path[strlen(font_path) + strlen(data.file) + 1];
    sprintf(path, "%s%s", font_path, data.file);
    char* buff = read_file_to_buffer(path, &size);

    if (buff == NULL)
    {
        printf("Error opening file %s for reading\n", path);
        return;
    }

    enum LINE_MODE
    {
        INFO = 0,
        COMMON,
        PAGE,
        CHARS,
        CHAR
    } line_mode;

    char c;
    int i = 0;
    char line_name[10];
    int line_name_index = 0;
    bool have_line_name = false;
    bool processed_line = false;
    
    while ((c = buff[i++]) != '\0')
    {
        if (!have_line_name)
        {   // process line name
            if (!isspace(c))
                line_name[line_name_index++] = c;
            else
            {
                line_name[line_name_index] = '\0';
                have_line_name = true;

                if (!strcmp(line_name, (char*)"info"))
                    line_mode = INFO;
                else
                if (!strcmp(line_name, (char*)"common"))
                    line_mode = COMMON;
                else
                if (!strcmp(line_name, (char*)"page"))
                    line_mode = PAGE;
                else
                if (!strcmp(line_name, (char*)"chars"))
                    line_mode = CHARS;
                else
                if (!strcmp(line_name, (char*)"char"))
                    line_mode = CHAR;
            }
            continue;
        }

        if (c == '\n')
        {   // reset line name counter
            have_line_name = false;
            processed_line = false;
            line_name_index = 0;
            continue;
        }

        if (processed_line) // skip to the end of the line, if its been processed
            continue;

        int dummy;  // dummy var to work with sscanf
        int g,x,y,w,h,xoff,yoff,xadvance;   // glyph data
        switch (line_mode)
        {
            case INFO:
                sscanf(&buff[i-1],
                    "face=\"%32[^\"]\" size=%d bold=%d italic=%d",
                    data.name, &data.size, &data.bold, &data.italic);
                processed_line = true;
                break;
            case COMMON:
                sscanf(&buff[i-1],
                    "lineHeight=%d base=%d scaleW=%d scaleH=%d",
                    &data.line_height, &dummy, &data.scaleW, &data.scaleH);
                processed_line = true;
                break;
            case PAGE:
                sscanf(&buff[i-1], "id=%d file=\"%32[^\"]\"", &dummy, data.png);
                processed_line = true;
                break;
            case CHARS:
                sscanf(&buff[i-1], "count=%d", &data.num_glyphs_defined);
                processed_line = true;
                break;
            case CHAR:
                sscanf(&buff[i-1],
                    "id=%d x=%d y=%d width=%d height=%d xoffset=%d yoffset=%d xadvance=%d",
                    &g, &x, &y, &w, &h, &xoff, &yoff, &xadvance
                );
                add_glyph(g, x, y, xoff, yoff, w, h, xadvance);
                processed_line = true;
                break;
            default: break;
        }
    }
    
    free(buff);
}


void Font::add_glyph(
    int c,
    float x, float y,
    float xoff, float yoff,
    float w, float h,
    float xadvance
) {
    glyphs[c].x = x;
    glyphs[c].y = y;
    glyphs[c].w = w;
    glyphs[c].h = h;
    glyphs[c].xoff = xoff;
    glyphs[c].yoff = yoff;
    glyphs[c].xadvance = xadvance;
    glyphs[c].available = 1;
}

void Font::update_glyphs()
{
    for (int i=0; i<128; i++)
    {
        if (i == '\t' && glyphs[' '].available)
        {
            const int TABS_IN_SPACES = 4;
            glyphs[i].w = glyphs[' '].w * TABS_IN_SPACES;
            glyphs[i].xoff = glyphs[' '].xoff * TABS_IN_SPACES;
            glyphs[i].xadvance = glyphs[' '].xadvance * TABS_IN_SPACES;
            glyphs[i].available = true;
        }
        
        if (!glyphs[i].available)
            continue;

        glyphs[i].x = glyphs[i].x / tex_width;
        glyphs[i].y = glyphs[i].y / tex_height;
        glyphs[i].tw = glyphs[i].w / tex_width;
        glyphs[i].th = glyphs[i].h / tex_height;
    }
}

struct Glyph Font::get_glyph(unsigned char c)
{
    struct Glyph glyph = glyphs[(unsigned int)c];
    if (!glyph.available)
        glyph = get_missing_glyph(c);
    return glyph;
}

struct Glyph Font::get_missing_glyph(unsigned char c)
{
    struct Glyph glyph;

    switch (c)
    {
        default:
            glyph = glyphs[(int)missing_glyph];
            break;
    }
    //printf("missing glyph: %d\n", c);
    return glyph;
}

void Font::get_string_pixel_dimension(char* str, int *length, int *height)
{
    char c;
    int i = 0;
    int len = 0;
    int miny = 0;
    int maxy = 0;
    Glyph g;
    while ((c = str[i++]) != '\0')
    {
        g = glyphs[(unsigned int)c];
        if (!g.available)
        {
            g = get_missing_glyph(c);
        }

        len += g.xadvance;

        if (i==1)
        {
            len += g.xoff;
        }
        if (g.yoff + g.h > maxy) maxy = g.yoff + g.h;
        if (g.yoff < miny) miny = g.yoff;
    }
    *length = len;
    if (len <= 0)
        *height = font->data.line_height;
    else
        *height = maxy - miny;
}

Font::Font(char* fn)
:
alpha(true),
missing_glyph('?')
{
    // init glyphs
    for (int i=0; i<128; i++)
    {
        glyphs[i].x = 0.0f;
        glyphs[i].y = 0.0f;
        glyphs[i].xoff = 0.0f;
        glyphs[i].yoff = 0.0f;
        glyphs[i].w = 0.0f;
        glyphs[i].h = 0.0f;
        glyphs[i].xadvance = 0.0f;
        glyphs[i].available = 0;
    }

    if (strlen(fn) > 32)
    {
        printf("WARNING: font file %s name length > 32\n",fn);
        fn[32+1] = '\0';
    }

    // save filename
    strcpy(data.file, fn);

    parse_font_file();  // gets all metadata (line height, png file) and glyphs
    load_font_png();    // load font png from acquired filename
    update_glyphs();    // post process glyphs (scaling texture coordinates; tab character)

    printf("Loaded font %s\n", data.file);
}

static int read_fonts_used()
{
    const char fonts_used[] = "fonts_used";
    char* fn = (char*)malloc(strlen(fonts_used) + strlen(font_path) - 4 + 1);
    sprintf(fn, "%s%s", font_path, fonts_used);

    int size = 0;
    char* buffer = read_file_to_buffer(fn, &size);
    if (buffer == NULL)
    {
        printf("ERROR: failed to read fonts file %s\n", fn);
        return 1;
    }

    int lines = 0;
    fonts = (Font**)malloc(sizeof(Font*) * lines);
    char** font_names = read_lines(buffer, &lines); // read file into lines
    if (!lines)
    {   // check there are lines
        printf("WARNING: no fonts found in fonts file %s\n", fn);
        return 1;
    }
    for (int i=0; i<lines; i++)
    {   // itearte lines, loading fonts
        if (font_names[i][0] == '\0' || font_names[i][0] == '#')    // skip blank lines, comments
            continue;
        fonts[n_fonts] = new Font(font_names[i]);
        if (strcmp(Options::font, font_names[i]) == 0)
        {   // default font as specified in settings
            printf("Selecting default font: %s\n", Options::font);
            font = fonts[n_fonts];
        }
        n_fonts++;
    }
    if (n_fonts == 0)
    {   // no fonts were really found in the file (all blank lines & comments)
        printf("WARNING: no fonts found in fonts file %s (Lines starting with # are considered comments)\n", fn);
        return 1;
    }
    if (font == NULL && lines && n_fonts)
    {   // the options font was not in fonts_used
        printf("Requested font %s not found in %s. Using font %s\n", Options::font, fn, font_names[0]);
        font = fonts[0];
    }
    default_font = font;

    free_read_lines(font_names, lines);
    free(buffer);
    free(fn);
    if (n_fonts < lines)
        fonts = (Font**)realloc(fonts, sizeof(Font*)*n_fonts);
    return 0;
}

void reset_default()
{
    font = default_font;
}

void set_properties(int size, int bold, int italic)
{
    for (int i=0; i<n_fonts; i++)
        if (fonts[i]->data.size == size
          && fonts[i]->data.bold == bold
          && fonts[i]->data.italic == italic)
        {
            font = fonts[i];
            return;
        }
    font = NULL;
}

void init()
{
    static int inited = 0;
    if (inited++)
    {
        printf("WARNING: Attempt to init fonts more than once\n");
        return;
    }

    if (read_fonts_used())
        font = NULL;
}

void teardown()
{
    if (fonts == NULL)
        return;
        
    for (int i=0; i<n_fonts; i++)
        if (fonts[i] != NULL)
            delete fonts[i];

    free(fonts);
}


}
