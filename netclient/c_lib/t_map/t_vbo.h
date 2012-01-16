#pragma once


    struct ColorElement
    {
        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };
    };

    struct AOElement
    {
        union
        {
            struct
            {
                unsigned char ao[4]; //32
            };
            unsigned int AO;
        };
    };

    //normal is a uniform variable when rendering by sides
    struct Vertex {
        float x,y,z;
        float tx,ty,tz; //12

        //unsigned char r,g,b,a;
        union
        {
            struct
            {
                unsigned char r,g,b,a;
            };
            unsigned int color;
        };

        //char normal[4];
        union
        {
            char normal[4]; //16
            unsigned int n;
        };

        union
        {
            unsigned char ao[4]; //32
            unsigned int AO;
        };
    };



int _init_draw_terrain();

int _update_chunks();

int create_vbo(struct VBO* q_VBO, struct Vertex* vlist, int v_num);
void delete_vbo(struct VBO* q_VBO);

int start_vbo_draw();
int draw_quad_vbo(struct VBO* q_VBO);
int end_vbo_draw();

inline int chunk_render_check(int x_off, int y_off, int tolerance);
int _set_camera(float x, float y, float z, float vx, float vy, float vz, float ux, float uy, float uz, float ratio, float viewangle);
int _set_fulstrum_culling(int value);
int _set_view_distance(int vd);


int* _chunk_request();

//internal
//int update_column_VBO(struct vm_column* column);
//inline void add_quad2(float x,float y,float z,int side, int tile_id);
//static void _set_quad_local_ambient_occlusion(struct Vertex* v_list, int offset, int x, int y, int z, int side);

int _draw_terrain();

struct Vertex* _get_quad_cache();

void _toggle_terrain_map_blend_mode();
void _refresh_map_vbo();
void _toggle_z_buffer();

void DRAW_VBOS1();
void DRAW_VBOS1a();
void DRAW_VBOS2(); 
void DRAW_VBOS3(); //shader version 
void DRAW_VBOS4();
void DRAW_VBOS5();
