

class Mouse(object):

    def __init__(self, camera):
        self.camera = camera

    def on_mouse_drag(self, x, y, dx, dy, buttons, modifiers):
		print 'dy, dy = ' +  str(dx) + ' ' + str(dy)
		#camera_x_angle = camera_x_angle + dx/100
		#camera_z_angle = camera_y_angle + dx/100
