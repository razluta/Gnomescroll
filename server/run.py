#!/c/Python27/python.exe

#!/usr/bin/python

# boot hack
import sys
import os
sys.path.insert(0, './ext/')
print "Working Directory: %s" % (os.getcwd())

# set up arguments and settings
import args_server
import opts
opts.opts = args_server.get_args()
opts = opts.opts

import random
import time
import platform
OS = platform.system()
if OS == 'Linux':
    import linux_terminal
    
import init_c_lib
import c_lib.map_gen
import map_recipes
import cube #dat

from init_c_lib import NetServerDispatchNetworkEvents, NetServerFlushToNet
from init_c_lib import START_CLOCK, GET_TICK

class Main:

    def __init__(self):
        init_c_lib.load_options(opts)
        init_c_lib.reset_seed(int(time.time()))   # reset seed (opts.seed only meant for map gen)

        init_c_lib.init_terrain()
        init_c_lib.init_cube_properties(cube.dat)

    def init_net(self):
        START_CLOCK()

        try:
            a,b,c,d = map(int, opts.ip_address.split('.'))
            print "Ip address: %s" % (opts.ip_address,)
        except ValueError, e:
            print "Invalid ip address %s" % (opts.ip_address,)
            print e
            sys.exit(1)            
        NetServerInit(a,b,c,d, opts.port)

    def run2(self):
        print "Server Started"

        init_c_lib.init()
        
        #map_recipes.gen_map_simple()
        for x in range(0,512):
            for y in range(0,512):
                init_c_lib.set(x,y,2, 1)

        init_c_lib.start()
        

        tick = 0

        init_c_lib.slime_test(2)
        while True:

            if OS == 'Linux':
                if linux_terminal.check_stdin():
                    break
            
            sl_c =0

            while True: #physics loop
                tc = GET_TICK()
                if tc == 0 or sl_c > 1: #net out
                    sl_c+=1
                    break

                init_c_lib.tick_server_state()
                init_c_lib.tick()

                tick+=1

                NetServerFlushToNet()
                init_c_lib.slime_tick()
                init_c_lib.check_agent_proximities()

            if sl_c > 0:
                while GET_TICK() != 0:
                    sl_c+=1
            if sl_c > 1:
                print "Physics: %i ticks this frame" % (sl_c)
            

            NetServerDispatchNetworkEvents()
            

            time.sleep(0.001)

        init_c_lib.close()
            
if __name__ == "__main__":
    print "starting server"
    main = Main()

    if OS == 'Linux':
        # for reading character input
        # need to switch terminal to character mode,
        # and restore it when done
        # it is inside a try loop, to catch control-c and make sure teardown is complete
        termios = linux_terminal.termios
        tty = linux_terminal.tty
        old_settings = termios.tcgetattr(sys.stdin)
        try:
            tty.setcbreak(sys.stdin.fileno())
            main.run2()
        finally:
            termios.tcsetattr(sys.stdin, termios.TCSADRAIN, old_settings)
    else:
        main.run2()
