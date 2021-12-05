// Datastructures.cc

#include "datastructures.hh"

#include <random>

#include <cmath>

std::minstd_rand rand_engine; // Reasonably quick pseudo-random generator

template <typename Type>
Type random_in_range(Type start, Type end)
{
    auto range = end-start;
    ++range;

    auto num = std::uniform_int_distribution<unsigned long int>(0, range-1)(rand_engine);

    return static_cast<Type>(start+num);
}

// Modify the code below to implement the functionality of the class.
// Also remove comments from the parameter names when you implement
// an operation (Commenting out parameter name prevents compiler from
// warning about unused parameters on operations you haven't yet implemented.)

Datastructures::Datastructures()
{
    // Replace this comment with your implementation
    place_container = {};
    area_container = {};
    place_name_container = {};
    place_distance_container = {};

}

Datastructures::~Datastructures()
{
    // Replace this comment with your implementation
    place_container.clear();
    area_container.clear();
    place_name_container.clear();
    place_distance_container.clear();
}

int Datastructures::place_count()
{
    return place_container.size();
}

void Datastructures::clear_all()
{
    place_container.clear();
    area_container.clear();
    place_name_container.clear();
    place_distance_container.clear();
}

std::vector<PlaceID> Datastructures::all_places()
{
    std::vector<PlaceID> all_place_id;
    all_place_id.reserve(place_count());
    for (auto const &item : place_container) {
        all_place_id.push_back(item.first);
    }
    return all_place_id;
}

bool Datastructures::add_place(PlaceID id, const Name& name, PlaceType type, Coord xy)
{
    if (place_container.find(id) != place_container.end()) return false;

    std::shared_ptr<Place> temp_place = std::make_shared<Place>(id, name, type, xy);
    place_container.insert({id, temp_place});

    place_name_container.insert({name, id});

    long long int distance_from_00 = distance_calculator({0,0}, xy);
    place_distance_container.insert({distance_from_00, id});
    return true;
}

std::pair<Name, PlaceType> Datastructures::get_place_name_type(PlaceID id)
{
    if (place_container.find(id) == place_container.end()) return {NO_NAME, PlaceType::NO_TYPE};
    Place_ptr found_place = place_container[id];
    return {found_place->place_name, found_place->place_type};
}

Coord Datastructures::get_place_coord(PlaceID id)
{
    if (place_container.find(id) == place_container.end()) return NO_COORD;
    return place_container[id]->place_coord;
}

bool Datastructures::add_area(AreaID id, const Name &name, std::vector<Coord> coords)
{
    if (area_container.find(id) != area_container.end()) return false;

    Area_ptr temp_area = std::make_shared<Area>(id, name, coords);
    area_container.insert({id, temp_area});
    return true;
}

Name Datastructures::get_area_name(AreaID id)
{
    if (area_container.find(id) == area_container.end()) return NO_NAME;
    return area_container[id]->area_name;
}

std::vector<Coord> Datastructures::get_area_coords(AreaID id)
{
    auto area = area_container.find(id);
    if (area == area_container.end()) return {NO_COORD};
    return area->second->area_coords;
}

void Datastructures::creation_finished()
{
    // Replace this comment with your implementation
    // NOTE!! It's quite ok to leave this empty, if you don't need operations
    // that are performed after all additions have been done.
}


std::vector<PlaceID> Datastructures::places_alphabetically()
{
    std::vector< PlaceID > result;
    result.reserve(place_count());
    for (const auto &item : place_name_container) {
        result.push_back(item.second);
    }
    return result;
}

std::vector<PlaceID> Datastructures::places_coord_order()
{
    std::vector< PlaceID > result;
    result.reserve(place_count());
    for (const auto &item : place_distance_container) {
        result.push_back(item.second);
    }
    return result;
}

std::vector<PlaceID> Datastructures::find_places_name(Name const& name)
{
    std::vector<PlaceID> place_with_name;
    auto found_range_name = place_name_container.equal_range(name);
    for (auto i = found_range_name.first; i != found_range_name.second; i++) {
        place_with_name.push_back(i->second);
    }
    return place_with_name;
}

std::vector<PlaceID> Datastructures::find_places_type(PlaceType type)
{
    std::vector<PlaceID> place_with_type;
    for (const auto &item : place_container) {
        if (item.second->place_type == type) {
            place_with_type.push_back(item.first);
        }
    }
    return place_with_type;
}

bool Datastructures::change_place_name(PlaceID id, const Name& newname)
{
    if (place_container.find(id) == place_container.end()) return false;

    remove_name_multimap(place_container[id]->place_name, id);

    place_name_container.insert({newname, id});
    place_container[id]->place_name = newname;
    return true;
}

bool Datastructures::change_place_coord(PlaceID id, Coord newcoord)
{
    if (place_container.find(id) == place_container.end()) return false;

    remove_distance_multimap(place_container[id]->place_coord, id);

    long long int new_distance = distance_calculator({0,0}, newcoord);
    place_distance_container.insert({new_distance, id});
    place_container[id]->place_coord = newcoord;
    return true;
}

std::vector<AreaID> Datastructures::all_areas()
{
    std::vector<AreaID> all_area_id;
    for (const auto &item : area_container) {
        all_area_id.push_back(item.first);
    }
    return all_area_id;
}

bool Datastructures::add_subarea_to_area(AreaID id, AreaID parentid)
{
    auto it = area_container.find(id);
    auto parent_it = area_container.find(parentid);
    if (it == area_container.end() ||
        parent_it == area_container.end()) return false;
    if (it->second->parent_area) return false;

    area_container[id]->parent_area = area_container[parentid];
    area_container[parentid]->subareas.push_back(it->second);

    return true;
}

std::vector<AreaID> Datastructures::subarea_in_areas(AreaID id)
{
    std::vector<AreaID> parent_area = {};
    if (area_container.find(id) == area_container.end()) return {NO_AREA};

    Area_ptr parent= area_container[id]->parent_area;
    while (parent) {
        AreaID tempID = parent->area_id;
        parent_area.push_back(tempID);
        parent = area_container[tempID]->parent_area;
    }
    return parent_area;
}

std::vector<PlaceID> Datastructures::places_closest_to(Coord xy, PlaceType type)
{
    std::vector<Place_ptr> place_v;
    place_v.reserve(place_count());

    for (const auto &place: place_container) {
        if (type == PlaceType::NO_TYPE || place.second->place_type == type) {
            place_v.push_back(place.second);
        }
    }

    if (place_container.size()  <= 3) {
        std::vector<PlaceID> place_nearest;
        place_nearest.reserve(3);
        std::sort(place_v.begin(), place_v.end(), [xy, this] (const Place_ptr& a, const Place_ptr& b) {
            long int dis_a = distance_calculator(a->place_coord, xy);
            long int dis_b = distance_calculator(b->place_coord, xy);
            if (dis_a == dis_b) return a->place_coord < b->place_coord;
            return (dis_a < dis_b);
        });

        for (auto const &item : place_v) {
            place_nearest.push_back(item->place_id);
        }
        return place_nearest;
    }

    std::nth_element(place_v.begin(), place_v.begin() + 2, place_v.end(), [xy, this](Place_ptr a, Place_ptr b) {
        long int dis_a = distance_calculator(a->place_coord, xy);
        long int dis_b = distance_calculator(b->place_coord, xy);
        if (dis_a == dis_b) return a->place_coord < b->place_coord;
        return (dis_a < dis_b);
    });

    std::sort(place_v.begin(), place_v.begin() + 2, [xy, this] (const Place_ptr& a, const Place_ptr& b) {
        long int dis_a = distance_calculator(a->place_coord, xy);
        long int dis_b = distance_calculator(b->place_coord, xy);
        if (dis_a == dis_b) return a->place_coord < b->place_coord;
        return (dis_a < dis_b);
    });

    std::vector<PlaceID> place_nearest;

    for (const auto &place: place_v) {
        if (place_nearest.size() < 3) {
            place_nearest.push_back(place->place_id);
        }
        else return place_nearest;
    }
    return place_nearest;
}

bool Datastructures::remove_place(PlaceID id)
{
    auto it = place_container.find(id);
    if (it == place_container.end()) return false;

    remove_name_multimap(it->second->place_name, id);
    remove_distance_multimap(it->second->place_coord, id);
    place_container.erase(it);

    return true;
}

std::vector<AreaID> Datastructures::all_subareas_in_area(AreaID id)
{
    if (area_container.find(id) == area_container.end()) return {NO_AREA};

    std::vector<AreaID> result;
    for (const auto &item : area_container) {
        Area_ptr p= item.second->parent_area;
        bool flag = true;
        while (p && flag) {
            AreaID tempID = p->area_id;
            if (tempID == id) {
                result.push_back(item.first);
                flag = false;
            }
            p = area_container[tempID]->parent_area;
        }
    }
    return result;
}

AreaID Datastructures::common_area_of_subareas(AreaID id1, AreaID id2)
{
    if (area_container.find(id1) == area_container.end()||
            area_container.find(id2) == area_container.end()) return NO_AREA;
    std::vector<AreaID> result = {};
    std::vector<AreaID> p_id1 = subarea_in_areas(id1);
    std::vector<AreaID> p_id2 = subarea_in_areas(id2);
    for (AreaID id : p_id1) {
        if (std::find(p_id2.begin(), p_id2.end(), id) != p_id2.end() ) return id;
    }
    return NO_AREA;
}

std::vector<WayID> Datastructures::all_ways()
{
    // Replace this comment with your implementation
    return {};
}

bool Datastructures::add_way(WayID id, std::vector<Coord> coords)
{
    // Replace this comment with your implementation
    return false;
}

std::vector<std::pair<WayID, Coord>> Datastructures::ways_from(Coord xy)
{
    // Replace this comment with your implementation
    return {{NO_WAY, NO_COORD}};
}

std::vector<Coord> Datastructures::get_way_coords(WayID id)
{
    // Replace this comment with your implementation
    return {NO_COORD};
}

void Datastructures::clear_ways()
{
    // Replace this comment with your implementation
}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_any(Coord fromxy, Coord toxy)
{
    // Replace this comment with your implementation
    return {{NO_COORD, NO_WAY, NO_DISTANCE}};
}

bool Datastructures::remove_way(WayID id)
{
    // Replace this comment with your implementation
    return false;
}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_least_crossroads(Coord fromxy, Coord toxy)
{
    // Replace this comment with your implementation
    return {{NO_COORD, NO_WAY, NO_DISTANCE}};
}

std::vector<std::tuple<Coord, WayID> > Datastructures::route_with_cycle(Coord fromxy)
{
    // Replace this comment with your implementation
    return {{NO_COORD, NO_WAY}};
}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_shortest_distance(Coord fromxy, Coord toxy)
{
    // Replace this comment with your implementation
    return {{NO_COORD, NO_WAY, NO_DISTANCE}};
}

Distance Datastructures::trim_ways()
{
    // Replace this comment with your implementation
    return NO_DISTANCE;
}

void Datastructures::remove_name_multimap(Name name, PlaceID id)
{
    auto found_range_name = place_name_container.equal_range(name);
    for (auto i = found_range_name.first; i != found_range_name.second; i++) {
        if (i->second == id) {
            place_name_container.erase(i);
            break;
        }
    }
}

void Datastructures::remove_distance_multimap(Coord xy, PlaceID id)
{
    long long int dis = distance_calculator({0,0}, xy);
    auto found_range_dis = place_distance_container.equal_range(dis);
    for (auto i = found_range_dis.first; i != found_range_dis.second; i++) {
        if (i->second == id) {
            place_distance_container.erase(i);
            break;
        }
    }
}

long long Datastructures::distance_calculator(Coord fromPoint, Coord toPoint)
{
    return static_cast<long long int>(toPoint.x - fromPoint.x)*static_cast<long long int>(toPoint.x - fromPoint.x) +
            static_cast<long long int>(toPoint.y - fromPoint.y)*static_cast<long long int>(toPoint.y - fromPoint.y);;
}
