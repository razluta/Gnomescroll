#!/usr/bin/python

'''
Client network incoming
'''
import opts
opts = opts.opts

import json
import zlib
import random

import init_c_lib
import stats

from dat_loader import dat_loader

class NetEventGlobal:
    messageHandler = None
    clientMessageHandler = None
    mapMessageHandler = None
    miscMessageHandler = None
    datMessageHandler = None

    @classmethod
    def init(cls):
        cls.messageHandler = MessageHandler()
        cls.clientMessageHandler = ClientMessageHandler()
        cls.chatMessageHandler = ChatMessageHandler()
        cls.miscMessageHandler = MiscMessageHandler()

        cls.mapMessageHandler = MapMessageHandler()

        cls.datMessageHandler = DatMessageHandler()

    @classmethod
    def register_json_events(cls, events):
        for string, function in events.items():
            cls.messageHandler.json_events[string] = function

class MessageHandler:
    def __init__(self):
        self.json_events = {} #map strings to functions

    def process_net_event(self, msg_type, datagram):
        if msg_type == 1:       #json message
            self.process_json_event(msg_type, datagram)
        elif msg_type == 3:                   #create a process json message
            self.process_binary_event(msg_type, datagram)
        elif msg_type == 4:
            self.process_json_compressed_event(msg_type, datagram)
        else:
            print "WARNING: MessageHandler:: unknown msg_type %d" % (msg_type,)

#binary events
    def process_binary_event(self, msg_type, datagram):
        if msg_type == 3:
            NetEventGlobal.mapMessageHandler._map_chunk(datagram)
        elif msg_type == 4:
            raise Exception, "msg_type 4 not implemented"
        else:
            print "MessageHandler.process_binary_event: message type unknown, " + str(msg_type)

#message events
    def process_json_event(self, msg_type, datagram):
        try:
            msg = json.loads(datagram)
            #print msg
        except Exception, e:
            print "MessageHandler.process_json_event error"
            print e
            #import sys
            #sys.exit()
            print datagram
            return
        cmd = msg.get('cmd', None)
        if cmd is None:
            return
        #use json_events when possible
        cmd = str(cmd)
        #print "Received python message: %s" % (cmd,)
        if cmd in self.json_events:
            #print msg
            self.json_events[cmd](**msg)
        else:
            print "Error, received command %s that client cannot handle" % (cmd,)
            print 'msg %s' % (msg,)
            print self.json_events.keys()
            assert False

    def process_json_compressed_event(self, msg_type, datagram):
        try:
            sizea = len(datagram)
            msg = zlib.decompress(datagram)
            sizeb = len(msg)
            print "Compressed Json: compressed size= %i, uncompressed size= %i " % (sizea, sizeb)
            #f = open('./tmp', 'w')
            #f.write(msg)
        except Exception, e:
            print "MessageHandler zlib decompression failed"
            print e
            return
        self.process_json_event(msg_type, msg)

class GenericMessageHandler:

    events = {}

    def register_events(self):
        NetEventGlobal.register_json_events(self.events)

    def _assign_events_to_methods(self):
        for event, name in self.events.items():
            ev = getattr(self, str(name), None)
            if ev is not None:
                self.events[event] = ev

    def __init__(self):
        self._assign_events_to_methods()
        self.register_events()


class ChatMessageHandler(GenericMessageHandler):

    events = {
        'chat' : '_chat',
        'you_died' : '_you_died',
        'you_killed' : '_you_killed',
    }

    def _chat(self, **msg):
        ChatClientGlobal.chatClient.receive(msg)

    def _you_died(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])

    def _you_killed(self, **msg):
        ChatClientGlobal.chatClient.system_notify(msg['msg'])


class MiscMessageHandler(GenericMessageHandler):
    events = {
        'ping' : '_ping',
    }

    def _ping(self, timestamp, **msg):
        stats.last_ping = init_c_lib.get_ticks() - timestamp

class MapMessageHandler(GenericMessageHandler):

    events = {
        'chunk_list' : '_chunk_list',
        'map_chunk' : '_map_chunk',
    }

    def _chunk_list(self, list, **msg):
        print "chunk list"
        MapControllerGlobal.mapController.process_chunk_list(list)

    def _map_chunk(self, datagram):
        #print "Map Chunk Received"
        (x,y,z) = terrainMap.set_packed_chunk(datagram)
        MapControllerGlobal.mapController.incoming_map_chunk(x,y,z)

class ClientMessageHandler(GenericMessageHandler):

    events = {
        'identified' : '_identified',
        #'identify_fail' : '_identify_fail',
    }

    def _identified(self, **msg):
        note = msg.get('msg', '')

        name = msg.get('name', None)
        if name is None:
            print 'msg::identified - player missing name'
            return False
        name = str(name)
        if not name:
            print 'msg identified :: name is empty'
            return False

        NetClientGlobal.name = name
        print 'Identified: name is %s' % (name,)
        ChatClientGlobal.on_identify(note)
        return True

    #used_alt = False
    #MAX_NAME_LENGTH = 15
    #def _identify_fail(self, msg, **arg):
        ## send system notification
        #if self.used_alt:

            ## add some numbers to the name
            #new_name = opts.name
            #ext = "%03d" % (random.randrange(0,1000),)
            #new_name = new_name[:self.MAX_NAME_LENGTH-len(ext)]
            #new_name += ext
            #NetOut.sendMessage.identify(name=new_name)

            ## alternate UI: REQUEST NEW NAME
            ##ChatClientGlobal.chatClient.system_notify('/identify_fail '+msg)
            ##ChatClientGlobal.chatClient.system_notify('/identify_fail Use /nick to set name.')
            ### activate chat, insert /nick
            ##init_c_lib.cy_input_state.chat = True
            ##ChatClientGlobal.chatClient.insert_string('/nick ')
        #else:
            #NetOut.sendMessage.identify(name=opts.alt_name)
            #self.used_alt = True

class DatMessageHandler(GenericMessageHandler):
    events = {
        'dat'   :   '_load_dat',
    }

    def _load_dat(self, **msg):
        err_msg = None
        try:
            dat = msg['dat']
        except KeyError:
            err_msg = 'dat missing'

        if err_msg is not None:
            print 'msg dat :: %s' % (err_msg,)
            return

        name, type, key = None, None, None
        if 'name' in msg:
            name = msg['name']
        if 'type' in msg:
            name = msg['type']
        if 'key' in msg:
            key = msg['key']

        if name is not None:
            if type is not None and key is not None:
                self._load_type_key(self, name, type, key, dat)
            else:
                self._load_name(self, name, dat)
        else:
            self._load_all(dat)

        if dat_loader.fully_loaded:
            print "Dats are loaded"
            NetOut.datMessage.loaded()

    def _load_all(self, dat):
        dat_loader.load_all(dat)
        NetOut.datMessage.loaded()

    def _load_name(self, name, dat):
        dat_loader.load(name, dat)

    def _load_type_key(self, name, type, key, val):
        dat_loader.set_property(name, type, key, val)

from net_client import NetClientGlobal
from net_out import NetOut
from chat_client import ChatClientGlobal
from map_controller import MapControllerGlobal

import c_lib.terrain_map as terrainMap

