#pragma once

#include <t_gen/twister.hpp>
#include <t_map/t_map.hpp>


namespace t_gen 
{

const int ruins_across_world = 8;
const int cubes_across_room = 16;
const int cubes_going_up = cubes_across_room / 2;
const int rooms_across_ruins = XMAX / ruins_across_world / cubes_across_room;
const int rooms_going_up = 5; // levels/floors
const int fixed_hall_wid = cubes_across_room / 4;
const int fixed_hall_offs = (cubes_across_room - fixed_hall_wid) / 2; // hall offset
const int min_lip = 2; // minimum lip

void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, int tile_id); 



enum DirIs {
    OPEN_AIR,
    HALL,
    DOOR,
	STAIRS,
    // if editing this, keep in mind code makes random openings assuming 0-2 are valid openings
    BLOCKED_BY_ROOM, // ..... and this is used as the excluded max for openings
    BLOCKED_BY_OUTSIDE,
    BLOCKED_FOREVER, // stops connecting to upper part of large room like Boss Room, or treating stairs same as lateral connections
};

enum Direction {
    DIR_NORTH, DIR_SOUTH,
    DIR_EAST, DIR_WEST,
    DIR_UP, DIR_DOWN
};
        
struct Room {
    int dirs[6];

    int x_offs;
    int y_offs;
    int wid; // width
    int dep; // depth
    int hei; // height
    int e_hall_hei;
    int e_hall_wid;
    int e_hall_offs; // east hall offset
    int n_hall_hei;
    int n_hall_wid;
    int n_hall_offs; // north hall offset
	//int air_x; // pos of a region that guarantees airspace, atm only used for stairways
	//int air_y;
	//int air_w; // width of air region
	//int air_d; // depth
};

Room rooms[rooms_going_up][rooms_across_ruins][rooms_across_ruins];



void set_region(int i_x, int i_y, int i_z, int i_w, int i_dep, int i_h, int tile_id = 1) {
	for (int z = i_z; z < i_z + i_h; z++) {
		for (int y = i_y; y < i_y + i_dep; y++) {
			for (int x = i_x; x < i_x + i_w; x++) {
				t_map::set(x, y, z, tile_id);
			}
		}
	}
}

bool far_north_cube(int y) {
    return y == cubes_across_room - 1;
}
bool far_south_cube(int y) {
    return y == 0;
}
bool far_east_cube(int x) {
    return x == cubes_across_room - 1;
}
bool far_west_cube(int x) {
    return x == 0;
}



bool w_of_n_opening(int rx, int ry, int rz, int cx) {
    return
        cx < rooms[rz][ry][rx].n_hall_offs;
}
bool e_of_n_opening(int rx, int ry, int rz, int cx) {
    return
        cx >= rooms[rz][ry][rx].n_hall_offs
        /**/ + rooms[rz][ry][rx].n_hall_wid;
}
bool s_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy < rooms[rz][ry][rx].e_hall_offs;
}
bool n_of_e_opening(int rx, int ry, int rz, int cy) {
    return
        cy >= rooms[rz][ry][rx].e_hall_offs
        /**/ + rooms[rz][ry][rx].e_hall_wid;
}

bool not_in_n_hall(int rx, int ry, int rz, int cx) {
    return
        w_of_n_opening(rx, ry, rz, cx) ||
        e_of_n_opening(rx, ry, rz, cx);
}
bool not_in_e_hall(int rx, int ry, int rz, int cy) {
    return
        s_of_e_opening(rx, ry, rz, cy) ||
        n_of_e_opening(rx, ry, rz, cy);
}

bool opens_to(int dir, int rx, int ry, int rz) {
    return rooms[rz][ry][rx].dirs[dir] < BLOCKED_BY_ROOM;
}

//bool in_air_region(Room r, int x, int y) {
//	if (x < r.air_x || x >= r.air_x + r.air_w) return false;
//	if (y < r.air_y || y >= r.air_y + r.air_y) return false;
//    return true;
//}



void make_walls_or_airspace(int rx, int ry, int rz, int ox, int oy) { // room indexes, origin
	int wall_block = randrange(33, 40);

	for (int cx = 0; cx < cubes_across_room; cx++) {
    for (int cy = 0; cy < cubes_across_room; cy++) {
    for (int cz = 0; cz < cubes_going_up - 2; cz++) {
		Room r = rooms[rz][ry][rx];
		int need_airspace = true;

        // do east
        int mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_SOUTH, rx, ry, rz))
            mid = rooms[rz][ry - 1][rx].n_hall_offs + rooms[rz][ry - 1][rx].n_hall_wid;
        if (cx >= mid && cy < fixed_hall_offs) {
            switch (r.dirs[DIR_EAST]) {
                case HALL:
                    if (s_of_e_opening(rx, ry, rz, cy)) need_airspace = false; break;
                case DOOR:
                    if (far_east_cube(cx) || far_south_cube(cy))
                        if (s_of_e_opening(rx, ry, rz, cy)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cx >= r.x_offs + r.wid /*far_east_cube(cx)*/ || far_south_cube(cy))
                        need_airspace = false; break;
            }
        }

        // do west
        mid = fixed_hall_offs; // cubes_across_room / 2;
        if (opens_to(DIR_NORTH, rx, ry, rz))
            mid = r.n_hall_offs;
        if (cx < mid && cy >= fixed_hall_offs + fixed_hall_wid) {
            switch (r.dirs[DIR_WEST]) {
                case HALL:
                    if (n_of_e_opening(rx - 1, ry, rz, cy)) need_airspace = false; break;
                case DOOR:
                    if (far_west_cube(cx) || far_north_cube(cy))
                        if (n_of_e_opening(rx - 1, ry, rz, cy)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cx < r.x_offs /*far_west_cube(cx)*/ || far_north_cube(cy))
                        need_airspace = false; break;
            }
        }

        // do north
        mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_EAST, rx, ry, rz))
            mid = r.e_hall_offs + r.e_hall_wid;
        if (cy >= mid && cy >= fixed_hall_offs + fixed_hall_wid) {
            switch (r.dirs[DIR_NORTH]) {
                case HALL:
                    if (e_of_n_opening(rx, ry, rz, cx)) need_airspace = false; break;
                case DOOR:
                    if (far_north_cube(cy) || far_east_cube(cx))
                        if (e_of_n_opening(rx, ry, rz, cx)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cy >= r.y_offs + r.dep /*far_north_cube(cy)*/ || far_east_cube(cx))
                        need_airspace = false; break;
            }
        }

        // do south
        mid = fixed_hall_offs + fixed_hall_wid; // cubes_across_room / 2;
        if (opens_to(DIR_WEST, rx, ry, rz))
            mid = rooms[rz][ry][rx - 1].e_hall_offs;
        if (cy < mid && cy < fixed_hall_offs) {
            switch (r.dirs[DIR_SOUTH]) {
                case HALL:
                    if (w_of_n_opening(rx, ry - 1, rz, cx)) need_airspace = false; break;
                case DOOR:
                    if (far_south_cube(cy) || far_west_cube(cx))
                        if (w_of_n_opening(rx, ry - 1, rz, cx)) need_airspace = false; break;
                default: // all blockers PLUS currently open air space connection
                    if (cy < r.y_offs /*far_south_cube(cy)*/ || far_west_cube(cx))
                        need_airspace = false; break;
            }
        }

		//if (in_air_region(r, cx, cy)) need_airspace = true; 

		// add 4 to all z values, to get above bedrock
		if (need_airspace) t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, 0);
		else               t_map::set(rx * cubes_across_room + cx + ox, ry * cubes_across_room + cy + oy, rz * cubes_going_up + cz + 4, wall_block);
	}
	}
    }
}

void make_stairs(int rx, int ry, int rz, int ox, int oy, int floor_block) { // room indexes, origin
		set_region(
			6 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 1,
			1, 2, 2, floor_block);
		set_region(
			7 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 2,
			1, 2, 3, floor_block);
		set_region(
			8 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 4,
			1, 2, 3, floor_block);
		set_region(
			9 + rx * cubes_across_room + ox,
			7 + ry * cubes_across_room + oy,
			rz * cubes_going_up + 3 + 6,
			1, 2, 3, floor_block);
}

void setup_rooms() {
    for (int z = 0; z < rooms_going_up; z++) {
		// floors have 1 stairway up
		int stairway_up_x = randrange(0, rooms_across_ruins - 1);
		int stairway_up_y = randrange(0, rooms_across_ruins - 1);

		for (int x = 0; x < rooms_across_ruins; x++) {
		for (int y = 0; y < rooms_across_ruins; y++) {
			Room r;
			// spans refer to the AIRSPACE, and don't include outer shell of blocks
			// but offset, for cleaner comparisons, should actually be the absolute offset from the corner of the room (including shell)
			int malleable_x_span = cubes_across_room - 2 /* shell of 2 walls */;
			int malleable_y_span = cubes_across_room - 2 /* shell of 2 walls */;
			int malleable_z_span = cubes_going_up - 2 /* shell of 2 walls */;
			r.wid = randrange(malleable_x_span / 2, malleable_x_span);
			r.dep = randrange(malleable_y_span / 2, malleable_y_span);
			malleable_x_span -= r.wid;
			malleable_y_span -= r.dep;
			r.x_offs = 1 /* shell */ + randrange(0, malleable_x_span);
			r.y_offs = 1 /* shell */ + randrange(0, malleable_y_span);

			r.e_hall_hei = randrange(2, malleable_z_span);
			r.n_hall_hei = randrange(2, malleable_z_span);

			// now that i chose my offset, it could have eaten into MALLEABLE span, and i don't think i'm considering that here!
			//.... shouldn't even be using that var?  i'm working within the WID/DEP space when doing the hallways right?!

			// reset malleables, for working INSIDE AIRSPACE
			malleable_x_span = r.wid - min_lip * 2;
			malleable_y_span = r.dep - min_lip * 2;
			r.e_hall_wid = /*fixed_hall_wid; */ randrange(2 /* min opening */, malleable_y_span);
			r.n_hall_wid = /*fixed_hall_wid; */ randrange(2 /* min opening */, malleable_x_span);
			malleable_x_span -= r.n_hall_wid;
			malleable_y_span -= r.e_hall_wid;
			r.e_hall_offs = /*fixed_hall_offs; */ r.y_offs + min_lip + randrange(0, malleable_y_span);
			r.n_hall_offs = /*fixed_hall_offs; */ r.x_offs + min_lip + randrange(0, malleable_x_span);

			// connections in directions
			for (int i = 0; i < 6; i++) {
				if /* lateral dir */ (i < 4) 
					r.dirs[i] = randrange(1, 2); // randomly choose door or hall
				else if /* stairway up should be here */ (i == 4 && x == stairway_up_x && y == stairway_up_y)
					r.dirs[i] = STAIRS;
				else if /* stairs going upwards in room below */ (i == 5 && z > 0 && rooms[z - 1][y][x].dirs[DIR_UP] == STAIRS)
					r.dirs[i] = STAIRS;
				else
					r.dirs[i] = BLOCKED_FOREVER;
			}

			if (y == 0)
				r.dirs[DIR_SOUTH] = BLOCKED_BY_OUTSIDE;
			if (y == rooms_across_ruins - 1)
				r.dirs[DIR_NORTH] = BLOCKED_BY_OUTSIDE;
			if (x == 0)
				r.dirs[DIR_WEST] = BLOCKED_BY_OUTSIDE;
			if (x == rooms_across_ruins - 1)
				r.dirs[DIR_EAST] = BLOCKED_BY_OUTSIDE;
                
			rooms[z][y][x] = r;
		}
		}
	}
}
	
void make_ruins(int x, int y) {
	setup_rooms();

	for (int rx = 0; rx < rooms_across_ruins; rx++) {
    for (int ry = 0; ry < rooms_across_ruins; ry++) {
    for (int rz = 0; rz < rooms_going_up; rz++) {
		int floor_block = randrange(33, 40);
		int ceil_block = randrange(33, 40);

		// make floor 
		set_region(
			rx * cubes_across_room + x,
			ry * cubes_across_room + y,
			rz * cubes_going_up + 3,
			cubes_across_room, cubes_across_room, 1, floor_block);
		
		// make ceiling
		set_region(
			rx * cubes_across_room + x,
			ry * cubes_across_room + y,
			rz * cubes_going_up + 3 + cubes_going_up - 1,
			cubes_across_room, cubes_across_room, 1, ceil_block);
		
		make_walls_or_airspace(rx, ry, rz, x, y);
		
		if (opens_to(rooms[rz][ry][rx].dirs[DIR_UP], rx, ry, rz) ) 
			make_stairs(rx, ry, rz, x, y, floor_block);

		if (opens_to(rooms[rz][ry][rx].dirs[DIR_DOWN], rx, ry, rz) ) 
			// clear well in floor of this room, and ceiling of room underneath
			set_region(
				rx * cubes_across_room + x + 6,
				ry * cubes_across_room + y + 7,
				rz * cubes_going_up + 3 - 1,
				4, 2, 2, 0);
    }
    }
	}
}

void start_dungeon_generator()
{
    printf("Making ruins\n");

    for (int x = 0; x < ruins_across_world; x++)
    for (int y = 0; y < ruins_across_world; y++)
		if (x % 2 == 0  &&  y % 2 == 0)
			if (randrange(0, 1) == 0)
				make_ruins(
					x * cubes_across_room * rooms_across_ruins, 
					y * cubes_across_room * rooms_across_ruins);
}

//bool not_in_hall(int i, int z) {
//	if (z >= fixed_hall_wid ||
//		i < fixed_hall_offs || 
//		i >= fixed_hall_offs + fixed_hall_wid) return true;
//	return false;
//}

}   // t_gen
