from __future__ import division
from random import uniform

from pyglet import clock, font, image, window
from pyglet.gl import *

def draw(self):
    glLoadIdentity()
    glTranslatef(self.x, self.y, 0.0)
    glRotatef(self.rot, 0, 0, 1)
    glScalef(self.size, self.size, 1.0)
    glBegin(GL_TRIANGLES)
    glColor4f(1.0, 0.0, 0.0, 0.0)
    glVertex2f(0.0, 0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(0.2, -0.5)
    glColor4f(0.0, 0.0, 1.0, 1.0)
    glVertex2f(-0.2, -0.5)
    glEnd()

def convert_index(index, height, width):
    index = int(index)
    height = int(height)
    width = int(width)
    x_ = index % width
    y_ = int((index - x_) / width)
    y = height - y_ -1
    rvalue =  x_ + y*width
    #print "rvalue= " + str(rvalue)
    return rvalue


class CubeProperties(object):

    def __init__(self):
        self.cubes = {
        0 : {
            'id' : 0,
            'occludes' : False,
            'active' : False,
            },
        1 : {
            'id' : 1,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (0, []),  #top
            (0, []),  #bottom
            (0, []), #west
            (0, []), #east
            (0, []), #north
            (0, []), #south
            ],
            },
        2 : {
            'id' : 2,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (0, [0,1,2,3]),  #top
            (6, [0,1,2,3]),  #bottom
            (7, [0,1,2,3]), #west
            (3, [0,1,2,3]), #east
            (4, [0,1,2,3]), #north
            (5, [0,1,2,3]), #south
            ],
        },
        3 : {
            'id' : 3,
            'occludes' : True,
            'active' : True,

            'texture' : [ #t, b, w, e, n, s
            (1, [0,1,2,3]),  #top
            (1, [0,1,2,3]),  #bottom
            (1, [0,1,2,3]), #west
            (1, [0,1,2,3]), #east
            (1, [0,1,2,3]), #north
            (1, [0,1,2,3]), #south
            ],
        },
     }

    def getTexture(self, tile_id, side_num):
        if self.cubes.has_key(tile_id):
            tex_a = self.cubes[tile_id]['texture']
            return tex_a[side_num]

        else:
            return 0

    def isActive(self, tile_id):
        if self.cubes.has_key(tile_id):
            return self.cubes[tile_id]['active']
        else:
            print "Error, cube type does not exist"
            return False

    def isOcclude(self, tile_id):
        if self.cubes.has_key(tile_id):
            return self.cubes[tile_id]['occludes']
        else:
            print "Error, cube type does not exist"
            return False

class CubeRenderCache(object):

    def __init__(self, cubeProperties, textureGrid):
        self.cubeProperties = cubeProperties
        self.textureGrid = textureGrid
        self.c4b_cache = {}
        self.t4f_cache = {}

        self.v_index = [
        [ 0,1,1 , 0,0,1 , 1,0,1 , 1,1,1 ], #top
        [ 1,0,0 , 0,0,0 , 0,1,0 , 1,1,0 ], #bottom
        [ 0,1,1 , 1,1,1 , 1,1,0 , 0,1,0 ], #north
        [ 0,0,1 , 0,0,0 , 1,0,0 , 1,0,1 ], #south
        [ 0,1,1 , 0,1,0 , 0,0,0 , 0,0,1 ], #west
        [ 1,0,1 , 1,0,0 , 1,1,0 , 1,1,1 ], #east
    ]

        north_side = [ [0,1,1],[1,1,1],[1,1,0],[0,1,0] ]
        south_side = [[0,0,1],[0,0,0],[1,0,0],[1,0,1]]
        west_side = [[0,1,1],[0,1,0],[0,0,0],[0,0,1]]
        east_side = [[1,0,1],[1,0,0],[1,1,0],[1,1,1]]
        top_side = [[0,1,1],[0,0,1],[1,0,1],[1,1,1]]
        bottom_side = [[1,0,0],[0,0,0],[0,1,0],[1,1,0]]

    ## t, b, n, s, w, e
    def get_side(self, x, y, z, tile_id, side_num):
        ta = self.v_index[side_num]
        #v_list = (GLfloat * 12) [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ya[9]+x,ta[10]+y,ta[11]+z ]
        v_list = [ta[0]+x,ta[1]+y,ta[2]+z , ta[3]+x,ta[4]+y,ta[5]+z , ta[6]+x,ta[7]+y,ta[8]+z , ta[9]+x,ta[10]+y,ta[11]+z ]
        c4B_list = self._get_c4B(tile_id, side_num)
        t4f_list = self._get_t4f(tile_id, side_num)
        return(v_list, c4B_list, t4f_list)

    def _get_c4B(self, tile_id, side_num):
        if self.c4b_cache.has_key((tile_id, side_num)):
            return self.c4b_cache[(tile_id, side_num)]
        else:
            ##compute from dict!
            #temp = (GLbyte * 4)[255, 255, 255, 255] * 4
            temp = [255, 255, 255, 255] * 4
            self.c4b_cache[(tile_id, side_num)] = temp
            return temp

    def _get_t4f(self, tile_id, side_num):
        if self.t4f_cache.has_key((tile_id, side_num)):
            return self.t4f_cache[(tile_id, side_num)]
        else:
            (texture_id, rotation) = self.cubeProperties.getTexture(tile_id, side_num)
            tex_tuple = self.textureGrid[convert_index(texture_id, 16, 16)].tex_coords
            if True:
                self.t4f_cache[(tile_id, side_num)] = list(tex_tuple)
                return list(tex_tuple)


class TerrainMap(object):

    def __init__(self):
        map = {}

    def get(x,y,z):
        pass

    def set(x,y,z,value):
        pass

class MapChunkManager(object):

    def __init__(self, terrainMap, cubeProperties):
        MapChunk.terrainMap = terrainMap
        MapChunk.cubeProperties = cubeProperties
        self.terrainMap = terrainMap
        self.cubeProperties = cubeProperties

    def set_map(x,y,z,tile_id):
        pass

import ctypes

class MapChunk(object):

    terrainMap = None
    cubeProperties = None
    cubeRenderCache = None

    x_chunk_size = 8
    y_chunk_size = 8
    z_chunk_size = 64

    def __init__(self, x_offset, y_offset):
        int_array_type = ctypes.c_int * (self.x_chunk_size * self.y_chunk_size *self.z_chunk_size) #map storage
        self.map_array = int_array_type()

        self.vertexList = None
        self.x_offset = x_offset
        self.y_offset = y_offset
        self.update = True
        self.empty = True

    def update_vertex_buffer(self, batch = None):
        draw_list = []
        active_cube_number = 0
        culled_quads = 0
        for x in range(0, self.x_chunk_size):
            for y in range(0, self.y_chunk_size):
                for z in range(0, self.z_chunk_size):
                    tile_id = self.get_tile(x,y,z)
                    if self.cubeProperties.isActive(tile_id): #non-active tiles are not draw
                        active_cube_number += 1
                        for side_num in [0,1,2,3,4,5]:
                            if not self._is_occluded(x,y,z,side_num):
                                draw_list.append((x,y,z,tile_id, side_num))
                            else:
                                culled_quads += 1

        if False:
            print "quads in chunk=" + str(active_cube_number*6)
            print "culled quads=" + str(culled_quads)
            print "draw_list= "
            for xa in draw_list:
                print str(xa)

        v_list = []
        c_list = []
        tex_list = []
        v_num = 0
        for (x,y,z,tile_id, side_num) in draw_list:
            rx = self.x_offset + x
            ry = self.y_offset + y
            rz = z

            (tv_list, tc_list, ttex_list) = self.cubeRenderCache.get_side(rx, ry, rz, tile_id, side_num)
            v_list += tv_list
            c_list += tc_list
            tex_list += ttex_list
            v_num += 4


        #print str(v_list)
        #print str(c_list)
        #print str(tex_list)
        #print str((len(v_list), len(c_list), len(tex_list)))

        if v_num == 0:
            self.empty = True
            self.update = False
            return

        if batch == None:
            self.vertexList = pyglet.graphics.vertex_list(v_num,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
            ("t3f\static", tex_list)
        )
        else:
            self.vertexList = batch.add(v_num, pyglet.gl.GL_QUADS, None,
            ('v3f\static', v_list),
            ('c4B\static', c_list),
            ("t3f\static", tex_list)
            )

        self.update = False

    def _is_occluded(self,x,y,z,side_num):
        s_array = [(0,0,1), (0,0,-1), (0,1,0), (0,-1,0), (-1,0,0),(1,0,0)]
        temp = s_array[side_num]
        _x = temp[0] + x
        _y = temp[1] + y
        _z = temp[2] + z

        #print str((x,y,z,self.get_tile(_x,_y,_z), side_num))

        if _x < 0 or _y < 0 or _z < 0:
            return False
        elif _x >= self.x_chunk_size or _y >= self.y_chunk_size or _z >= self.z_chunk_size:
            return False

        tile_id = self.get_tile(_x,_y,_z)
        #print str((x,y,z,tile_id, side_num))
        return self.cubeProperties.isOcclude(tile_id)


 #           return False
 #       if x+1 == self.s_chunk_size or y+1 == self.y_chunk_size or z+1 ==

    def get_tile(self, x,y,z):
       index = x+y*self.y_chunk_size+z*self.x_chunk_size * self.y_chunk_size
       if index < 0 or index >= self.x_chunk_size * self.y_chunk_size*self.z_chunk_size:
           print "invalid map index"
           return 0
       else:
           return self.map_array[index]

    def set_tile(self,x,y,z, value):
        index = x+y*self.y_chunk_size+z*self.x_chunk_size * self.y_chunk_size
        self.map_array[index] = c_int(value)

import random

class World(object):

    def __init__(self):
        self.batch = pyglet.graphics.Batch()
        #texture loading
        tile_image = pyglet.image.load('./texture/textures_01.png')
        tile_image_grid = pyglet.image.ImageGrid(tile_image, 16, 16)
        tile_texture_grid = pyglet.image.TextureGrid(tile_image_grid)
        self.texture_grid = tile_texture_grid

        self.cubeProperties = CubeProperties()
        self.cubeRenderCache = CubeRenderCache(self.cubeProperties, self.texture_grid) #needs texture grid
        #test
        self.terrainMap = TerrainMap()
        #MapChunkManager(terrainMap, cubeProperties)
        self.players = []

    def tick(self):
        pass


    def test_chunk(self):
        MapChunk.cubeProperties = self.cubeProperties
        MapChunk.cubeRenderCache = self.cubeRenderCache

        print "Start chunk generation"
        self.mct_array = {}
        for xa in range(0, 8):
            for ya in range(0, 8):
                self.mct_array[(xa,ya)] = MapChunk(8*xa,8*ya)
                for x in range(0, 8):
                    for y in range(0, 8):
                        for z in range(0, 8):
                            rnd = random.randint(0,64)
                            #rnd = 3
                            if rnd < 16:
                                rnd = rnd = random.randint(0,3)
                                self.mct_array[(xa,ya)].set_tile(x,y,z,rnd)

    def draw_chunk(self):
        for mct in self.mct_array.values(): #update only one buffer per frame
            if mct.update == True:
                mct.update_vertex_buffer(self.batch)
                break

        glEnable(GL_CULL_FACE);
        glEnable(self.texture_grid.target)
        glBindTexture(self.texture_grid.target, self.texture_grid.id)

        #for mct in self.mct_array.values():
        #    if mct.empty == False:
        #        mct.vertexList.draw(pyglet.gl.GL_QUADS)
        self.batch.draw()

        glDisable(GL_CULL_FACE);
        glDisable(self.texture_grid.target)

    def draw(self):
        if False:
            for x in range(-20, 20):
                self.draw_point(x, 0, 0, 255, 0)
            for y in range(-20, 20):
                self.draw_point(0, y, 0, 0, 255)


        self.draw_chunk()
        self.draw_players()
        #self.draw_players()

    def draw_point(self, x, y, r, g, b):
        z=0
        pyglet.graphics.draw(1, pyglet.gl.GL_POINTS,
            ('v3f', (x, y, z)),
            ('c3B', (r, g, b)) )

    def add_player(self, player =None):
        if Player == None:
            self.players.append(Player())
        else:
            self.players.append(player)

    def draw_players(self):
        for p in self.players:
            p.draw()

from math import sin, cos, pi

from math import floor, ceil

class Player:

    def __init__(self, main=None):
        self.main = main

        self.x = -.5
        self.y = -.5
        self.z = -.5
        self.x_angle = 0
        self.y_angle = 0

    def draw(self):
        self.draw_aiming_direction()
        self.draw_selected_cube()
        self.draw_player_bounding_box()

    def draw_player_bounding_box(self):
        v_sets = [
        [ [0,1,1] , [0,0,1] , [1,0,1] , [1,1,1] ], #top
        [ [1,0,0] , [0,0,0] , [0,1,0] , [1,1,0] ], #bottom
        [ [0,1,1] , [1,1,1] , [1,1,0] , [0,1,0] ], #north
        [ [0,0,1] , [0,0,0] , [1,0,0] , [1,0,1] ], #south
        [ [0,1,1] , [0,1,0] , [0,0,0] , [0,0,1] ], #west
        [ [1,0,1] , [1,0,0] , [1,1,0] , [1,1,1] ], #east
    ]

        (x,y,z) = (floor(self.x), floor(self.y), floor(self.z))
        v_list = []
        v_num = 0
        for v_set in v_sets:
            for i in [0,1,2,3]:
                v_num += 2
                v_list += [v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z]
                v_list += [v_set[(i+1)%4][0]+x, v_set[(i+1)%4][1]+y, v_set[(i+1)%4][2]+z]

        pyglet.graphics.draw(v_num, GL_LINES,
        ("v3f", v_list),
        ("c3B", [255, 255, 0] *v_num)
        )

    def draw_aiming_direction(self):
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        ep = 0.33
        v_list = []
        v_num = 0
        cf = 0.
        for i in range(0,50):
            v_num += 1
            x= self.x + cf*dx
            y= self.y + cf*dy
            z= self.z + cf*dz
            cf += ep
            v_list += [x,y,z]
        #print str(v_list)
        #print str(v_num)
        pyglet.graphics.draw(v_num, GL_POINTS,
        ("v3f", v_list),
        ("c3B", [255, 0, 0]*v_num)
        )

    def pan(self, dx_angle, dy_angle):
        self.x_angle += dx_angle
        self.y_angle += dy_angle
        if self.y_angle < -0.499:
            self.y_angle = -0.499
        if self.y_angle > 0.499:
            self.y_angle = 0.499

    def draw_side(self, x,y,z,side):

        v_index = [
        [ [0,1,1] , [0,0,1] , [1,0,1] , [1,1,1] ], #top
        [ [1,0,0] , [0,0,0] , [0,1,0] , [1,1,0] ], #bottom
        [ [0,1,1] , [1,1,1] , [1,1,0] , [0,1,0] ], #north
        [ [0,0,1] , [0,0,0] , [1,0,0] , [1,0,1] ], #south
        [ [0,1,1] , [0,1,0] , [0,0,0] , [0,0,1] ], #west
        [ [1,0,1] , [1,0,0] , [1,1,0] , [1,1,1] ], #east
    ]
        v_set = v_index[side]
        v_list = []
        v_num = 0
        for i in [0,1,2,3]:
            v_num += 2
            v_list += [v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z]
            v_list += [v_set[(i+1)%4][0]+x, v_set[(i+1)%4][1]+y, v_set[(i+1)%4][2]+z]

        pyglet.graphics.draw(v_num, GL_LINES,
        ("v3f", v_list),
        ("c3B", [0, 255, 0] *v_num)
        )

    def draw_cube(self, x,y,z, side = None):
        v_sets = [
        [ [0,1,1] , [0,0,1] , [1,0,1] , [1,1,1] ], #top
        [ [1,0,0] , [0,0,0] , [0,1,0] , [1,1,0] ], #bottom
        [ [0,1,1] , [1,1,1] , [1,1,0] , [0,1,0] ], #north
        [ [0,0,1] , [0,0,0] , [1,0,0] , [1,0,1] ], #south
        [ [0,1,1] , [0,1,0] , [0,0,0] , [0,0,1] ], #west
        [ [1,0,1] , [1,0,0] , [1,1,0] , [1,1,1] ], #east
    ]

        v_set = [
            [0,0,0],
            [1,0,0],
            [1,1,0],
            [0,1,0],
            [0,0,1],
            [1,0,1],
            [1,1,1],
            [0,1,1]
        ]

        vertex_index = [
            [0,1],
            [1,2],
            [2,3],
            [3,0],

            [4,5],
            [5,6],
            [6,7],
            [7,4],

            [0,4],
            [1,5],
            [2,6],
            [3,7],
        ]

        side_v = [
        [4,5,6,7],   #top (z=1)
        [0,1,2,3],   #bottom (z=0)
        [1,5,9,10],  #north (y=1)
        [7,3,11,8],  #south (y=0)
        [6,2,10,11], #west (x=0)
        [4,0,9,8].   #east (x=1)


        ]

        #(x,y,z) = (floor(self.x), floor(self.y), floor(self.z))
        v_list = []
        c_list = []
        v_num = 0
        for [i,j] in vertex_index:
            v_num += 2
            v_list += [ v_set[i][0]+x, v_set[i][1]+y, v_set[i][2]+z ]
            v_list += [ v_set[j][0]+x, v_set[j][1]+y, v_set[j][2]+z ]

            if True:
                c_list += [155,0,0]*2
                #print str(j)
            else:
                c_list += [0,155,155]*2

        pyglet.graphics.draw(v_num, GL_LINES,
        ("v3f", v_list),
        ("c3B", c_list)
        )


    def draw_selected_cube(self):
        x = self.x
        y = self.y
        z = self.z
        dx = cos( self.x_angle * pi) * cos( self.y_angle * pi)
        dy = sin( self.x_angle * pi) * cos( self.y_angle * pi)
        dz = sin( self.y_angle)

        if dx > 0:
            pass #south side
            l_list = []
            for x in range (0, 5):
                l = (x-self.x)/dx
                (x_,y_,z_) = (dx*l+self.x, dy*l+self.y, dz*l+self.z)
                #print str((x_,y_,z_))
                #self.draw_side(x_, floor(y_), floor(z_), 3)

                if x < 0:
                    self.draw_cube(x_, floor(y_), floor(z_), 4)
                elif x>= 0:
                    self.draw_cube(x_, floor(y_), floor(z_), 5)

        if dx < 0:
            pass #north side


from input import Mouse, Keyboard
from camera import Camera, Hud

from pyglet.window import key

class App(object):

    def __init__(self):

        self.world = World()
        self.win = window.Window(fullscreen=False, vsync=False)
        self.camera = Camera(self.win)
        self.camera = Camera(self.win)
        self.keyboard = Keyboard(self)
        self.player = Player() #for testing
        self.mouse = Mouse(self)
        self.hud = Hud(self.win)
        #clock.set_fps_limit(60)
        #setup events
        self.keyboard.key_handlers[key.ESCAPE] = self.win.close
        self.win.on_mouse_drag = self.mouse.on_mouse_drag
        self.win.on_key_press = self.keyboard.on_key_press

        print "App init finished"

    def mainLoop(self):
        clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        while not self.win.has_exit:
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()
            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()

    def mainLoop2(self):
        self.world.test_chunk()
        self.world.add_player(self.player)
        #clock.set_fps_limit(60)
        keyboard = key.KeyStateHandler()
        self.win.push_handlers(keyboard)
        #self.win.push_handlers(pyglet.window.event.WindowEventLogger())
        while not self.win.has_exit:
            self.win.dispatch_events()
            self.keyboard.stateHandler(keyboard)

            self.world.tick()
            self.win.clear() #?

            self.camera.worldProjection()


            self.world.draw()
            self.camera.hudProjection()
            self.hud.draw()

            clock.tick()
            self.win.flip()
            #return

app = App()
#cProfile.run('app.mainLoop2()')
app.mainLoop2()

