$.extend(globals, {
    world_id   : 0,
    new_client : !!localStorage.client_id,
    client_id  : localStorage.client_id || globals.create_client_id(),
    player_id  : 0,
    session_id : null,
    server_out : '',
    api_path   : '/api',
});

dispatcher.listen('register', function (event_name, msg) {
    globals.session_id = msg.session_id;
    globals.new_client = false;
});


var send = function (data) {
    data = JSON.stringify(data);
    $.post(globals.server_out + globals.api_path, { json: data });
    return true;
};

var hello = function () {
    return send({
        cmd       : 'hello',
        world_id  : globals.world_id,
        client_id : globals.client_id
    });
};

var admin, info, action;

admin = {

    type: 'admin',
            
    create_agent: function (x, y, z, location) {
        var position;
    
        if (location === undefined) {
            location = 0;
        }
    
        if (x.constructor.name === 'Array') {
            if (x.length === 3) {
                x.splice(0,0,location);
            }
            position = x;
        } else {
            position = [location, x, y, z];
        }
    
        var data = { 
            cmd: 'create_agent',
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            player_id: globals.player_id,
            position: position,
        };
        return send(data);
    },
    
    create_object: function (x, y, z, location, object_type, template) {
        var position;
    
        if (location === undefined) {
            location = 0;
        }
    
        if (x.constructor.name === 'Array') {
            if (x.length === 3) {
                x.splice(0,0,location);
            }
            position = x;
        } else {
            position = [location, x, y, z];
        }
        
        var data = {
            cmd: 'create_item',
            type: this.type,
            object_type: object_type,
            template: template,
            client_id: globals.client_id,
            world_id: globals.world_id,
            position: position,
        };
        return send(data);
    },
    
    set_map: function (x, y, z, value, debug) {
        var position;
    
        if (value === undefined) {
            value = 1;
        }
    
        if (x.constructor.name === 'Array') {
            position = x;
            if (position.length < 4 && value !== undefined) {
                value = y;
                position.splice(0,0,value);
            }
        } else {
            position = [value, x, y, z];
        }

        var data = { 
            cmd: "set_map",
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            position: position,
            value: value
        };
        return send(data);
    }
};

info = {
    
    type: 'info',
    consts: {
        type: 'info',
        client_id: globals.client_id,
        world_id: globals.world_id
    },
        
    tileset: function () {
        var data = $.extend({}, this.consts, { cmd: 'get_tiles' });
        return send(data);
    },
        
    map: function (z) {
        var data = $.extend({}, this.consts, { cmd: 'get_map' });
               
        if (z !== undefined) {
            data.z = z;
        }
        return send(data);
    },

    agent: function (id) {
        var data = $.extend({}, this.consts, { cmd: 'get_agent',
                                               agent_id: id });
        return send(data); 
    },

    object: function (id) {
        var data = $.extend({}, this.consts, { cmd: 'get_object',
                                               object_id: id, });
        return send(data);
    },
    
    agents: function () {
        var data = $.extend({}, this.consts, { cmd: 'get_agent_list' });
        return send(data);
    },

    objects: function() {
        var data = $.extend({}, this.consts, { cmd: 'get_object_list' });
        return send(data);
    }
};

action = {

    type: 'agent',
        
    move: function (agent_id, dx, dy, dz) {
        var data = { 
            cmd: "move_0",
            type: this.type,
            client_id: globals.client_id,
            world_id: globals.world_id,
            agent_id: agent_id,
            dp: [dx, dy, dz]
        };
        
        return send(data);
    },
    
    till: function (agent_id) {
        var data = {
            cmd: 'till_soil',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
        };
        return send(data);
    },

    plant: function (agent_id) {
        var data = {
            cmd: 'plant_crop',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
        };
        return send(data);
    },

    harvest: function (agent_id, crop_id) {
        //temporary
        if (crop_id === undefined) {
            crop_id = 1;
        }
        
        var data = {
            cmd: 'harvest_crop',
            type: this.type,
            world_id: globals.world_id,
            agent_id: agent_id,
            crop_id: crop_id,
        };
        return send(data);
    }
};
