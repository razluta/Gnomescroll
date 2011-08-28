#ifndef camera_h
#define camera_h

#include <compat_gl.h>

struct Camera {
float fov;
float x_size,y_size;
float ratio;
float z_near, z_far;

float x,y,z;
float xl, yl, zl;
float xu, yu, zu;
float x_angle, y_angle;
};

int _world_projection(struct Camera* camera);
int _hud_projection(struct Camera* camera);

int _set_camera(struct Camera* c);
struct Camera* _get_camera();

void set_model_view_matrix(float *a);

#endif
