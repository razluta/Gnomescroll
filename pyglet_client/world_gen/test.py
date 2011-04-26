import libtcodpy as libtcod
import random

#actual size of the window
SCREEN_WIDTH = 80
SCREEN_HEIGHT = 50
viewer_top_x = 0
viewer_top_y = 0
viewer_bot_x = SCREEN_WIDTH
viewer_bot_y = SCREEN_HEIGHT
MAP_WIDTH = 200
MAP_HEIGHT = 200
map_viewer = libtcod.console_new(MAP_WIDTH, MAP_HEIGHT)
mouse_on_drag_start = None
LEVELS_ABOVE = 64
LEVELS_BELOW = 64
levels_to_display = 10
z_to_display = (LEVELS_ABOVE + LEVELS_BELOW) - levels_to_display - 1

#For creating noise for heightmaps:
#First number = degrees of noise 1-4, should always be 2.
#Second number needs to decrease as the map size increases, adjusts "zoom"
#Third number is seems to adjust variance - a value around 20 makes a
#detailed noise model, but the noise in general is smoother, larger, and farther apart.

LIMIT_FPS = 20  #20 frames-per-second maximum

local_detail_random=libtcod.random_new_from_seed(random.randint(1, 1000000))
local_detail_noise=libtcod.noise_new(2, 0.5, 2, local_detail_random)

roughness_random = libtcod.random_new_from_seed(random.randint(1, 1000000))
roughness_noise=libtcod.noise_new(2, 0.5, 20, roughness_random)

elevation_random = libtcod.random_new_from_seed(random.randint(1, 1000000))
elevation_noise=libtcod.noise_new(2, 0.5, 20, elevation_random)

# 3x3 kernel for smoothing operations
smoothKernelSize=9
smoothKernelDx=[-1,0,1,-1,0,1,-1,0,1]
smoothKernelDy=[-1,-1,-1,0,0,0,1,1,1]
smoothKernelWeight=[1.0,2.0,1.0,2.0,20.0,2.0,1.0,2.0,1.0]

def get_color(element):
    #TODO: make this a dictionary
    if element <= 0:
        return libtcod.darkest_blue
    elif element <= 0.05:
        return libtcod.dark_blue
    elif element <= 0.1:
        return libtcod.blue
    elif element <= 0.15:
        return libtcod.light_blue
    elif element <= 0.2:
        return libtcod.lighter_blue
    elif element <= 0.25:
        return libtcod.lightest_blue
    elif element <= 0.3:
        return libtcod.lighter_amber
    elif element <= 0.35:
        return libtcod.lightest_green
    elif element <= 0.4:
        return libtcod.lighter_green
    elif element <= 0.45:
        return libtcod.light_green
    elif element <= 0.5:
        return libtcod.green
    elif element <= 0.55:
        return libtcod.dark_green
    elif element <= 0.6:
        return libtcod.darker_green
    elif element <= 0.65:
        return libtcod.darkest_green
    elif element <= 0.7:
        return libtcod.darker_sepia
    elif element <= 0.75:
        return libtcod.darkest_sepia
    elif element <= 0.8:
        return libtcod.darker_grey
    elif element <= 0.85:
        return libtcod.grey
    elif element <= 0.9:
        return libtcod.light_grey
    elif element <= 0.95:
        return libtcod.lighter_grey
    else:
        return libtcod.white

# function building the heightmap
def build_local_detail(hm) :
    libtcod.heightmap_add_fbm(hm,local_detail_noise,5,5,0,0,6,0.5,1)
    #for i in range(2,-1,-1):
        #libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    #libtcod.heightmap_normalize(hm,0,1)
    print "done with local detail map"

# function building the heightmap
def build_roughness(hm) :
    libtcod.heightmap_add_fbm(hm,roughness_noise,5,5,0,0,6,0.5,1)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    #libtcod.heightmap_normalize(hm,0,1)
    print "done with roughness map"

# function building the heightmap
def build_elevation(hm) :
    libtcod.heightmap_add_fbm(hm,elevation_noise,5,5,0,0,6,0.5,1)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    for i in range(2,-1,-1):
        libtcod.heightmap_kernel_transform(hm,smoothKernelSize,smoothKernelDx,smoothKernelDy,smoothKernelWeight,-0.5,3.26)
    #libtcod.heightmap_normalize(hm,0,1)
    print "done with elevation map"

def move_screen(dx, dy):
    global viewer_top_x, viewer_top_y, viewer_bottom_x, viewer_bottom_y
    #moves the screen if that wouldn't cause the edge of the map to be exceeded.
    #If it would, it moves as much as it can without passing the edge of the map.
    viewer_bot_x = viewer_top_x + SCREEN_WIDTH
    viewer_bot_y = viewer_top_y + SCREEN_HEIGHT

    if viewer_top_x + dx < 0:
            dx = viewer_top_x * -1

    elif viewer_bot_x + dx >= MAP_WIDTH:
            dx = MAP_WIDTH - viewer_bot_x

    if  viewer_top_y + dy < 0:
            dy = viewer_top_y * -1

    elif viewer_bot_y + dy >= MAP_HEIGHT:
            dy = MAP_HEIGHT - viewer_bot_y

    viewer_top_x = viewer_top_x + dx
    viewer_top_y = viewer_top_y + dy

def render():
    x = 0;
    y = 0;
    libtcod.console_flush()

    for x in range(MAP_WIDTH):
        for y in range(MAP_HEIGHT):
            #If the current square is ground, and there is air above, color it
            if terrain[x][y][z_to_display] == 1 and terrain[x][y][z_to_display + levels_to_display] == 0:
                z = libtcod.heightmap_get_value(final_hm, x ,y)
                c = get_color(z)
            #if the current square is ground, and there is rock above it, it should be black, for the cross section
            elif terrain[x][y][z_to_display] == 1:
                c = libtcod.black
            #otherwise, it is air, it is blue
            else:
                c = libtcod.lightest_blue
            libtcod.console_set_char_background(map_viewer,x,y,c,libtcod.BKGND_SET)
    libtcod.console_blit(map_viewer, viewer_top_x, viewer_top_y, viewer_bot_x, viewer_bot_y, 0, 0, 0)

def handle_mouse(current_mouse):
    global mouse_on_drag_start
    if current_mouse.lbutton:
        dx = mouse_on_drag_start.cx - current_mouse.cx;
        dy = mouse_on_drag_start.cy - current_mouse.cy;
        move_screen(dx, dy);
        mouse_on_drag_start = current_mouse;
    else:
        mouse_on_drag_start = current_mouse;

def handle_keys():
    global z_to_display, elevation_hm
    key = libtcod.console_check_for_keypress()  #real-time

    if key.vk == libtcod.KEY_ENTER and libtcod.KEY_ALT:
        #Alt+Enter: toggle fullscreen
        libtcod.console_set_fullscreen(not libtcod.console_is_fullscreen())

    elif key.vk == libtcod.KEY_DOWN:
    #Down to the next z-level
        if  z_to_display != 0:
            z_to_display = z_to_display - 1
            print "Current z levels are", z_to_display, "to", z_to_display + levels_to_display

    elif key.vk == libtcod.KEY_UP:
    #Up to the next z-level
        if z_to_display != (LEVELS_ABOVE + LEVELS_BELOW) - levels_to_display - 1:
            z_to_display = z_to_display + 1
            print "Current z levels are", z_to_display, "to", z_to_display + levels_to_display

    elif key.vk == libtcod.KEY_ESCAPE:
        return True  #exit game

def translate_to_terrain(hm):
    terrain = [[[0 for z in range(LEVELS_ABOVE + LEVELS_BELOW)] for y in range(MAP_HEIGHT)] for x in range(MAP_WIDTH)]
    for x in range(MAP_WIDTH):
        for y in range(MAP_HEIGHT):
            height = (int(libtcod.heightmap_get_value(final_hm, x ,y)*64 + 64))
            for z in range(height):
                terrain[x][y][z] = 1
    return terrain

#############################################
# Initialization
#############################################

libtcod.console_init_root(SCREEN_WIDTH, SCREEN_HEIGHT, 'Gnomescroll Worldgen', False)
libtcod.sys_set_fps(LIMIT_FPS)

local_detail_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_local_detail(local_detail_hm)

roughness_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_roughness(roughness_hm)

elevation_hm=libtcod.heightmap_new(MAP_WIDTH,MAP_HEIGHT)
build_elevation(elevation_hm)

multiplied_hm = libtcod.heightmap_new(MAP_WIDTH, MAP_HEIGHT)
libtcod.heightmap_multiply_hm(local_detail_hm, roughness_hm, multiplied_hm)
libtcod.heightmap_delete(local_detail_hm)
libtcod.heightmap_delete(roughness_hm)


final_hm = libtcod.heightmap_new(MAP_WIDTH, MAP_HEIGHT)
libtcod.heightmap_add_hm(elevation_hm, multiplied_hm, final_hm)
libtcod.heightmap_delete(elevation_hm)
libtcod.heightmap_delete(multiplied_hm)

libtcod.heightmap_normalize(final_hm)

#translate the 2D hm to a 3D terrain map
terrain = translate_to_terrain(final_hm)

##WHICH MAP TO SHOW IN VIEWER


print "start render"
v_list = []
for x in range(MAP_WIDTH):
    for y in range(MAP_HEIGHT):
        z = libtcod.heightmap_get_value(final_hm, x ,y)
        v_list.append((x,y,z))

import display
app = display.App()
app.mainLoop(v_list)

#print str(v_list)


print "render finished?"

#while not libtcod.console_is_window_closed():
    #render();

    #handle_mouse(libtcod.mouse_get_status())
    ##handle keys and exit game if needed
    #exit = handle_keys()

    #if exit:
        #break
