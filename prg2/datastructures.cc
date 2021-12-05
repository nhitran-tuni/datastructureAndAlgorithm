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
    place_container = {};
    area_container = {};
    place_name_container = {};
    place_distance_container = {};

}

Datastructures::~Datastructures()
{
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
    std::vector<WayID> all_way_id;
    all_way_id.reserve(way_container.size());
    for (auto const &item : way_container) {
        all_way_id.push_back(item.first);
    }
    return all_way_id;
}

bool Datastructures::add_way(WayID id, std::vector<Coord> coords)
{   
    auto way = way_container.find(id);
    if (way != way_container.end()) return false;
    if (coords.size() == 0) return false;

    auto c_front = crossroad_container.find(coords.front());
    Crossroad_ptr c_1;

    if (c_front != crossroad_container.end()) {
        c_1 = c_front->second;
    } else {
        c_1 = std::make_shared<Crossroad>(coords.front());
        crossroad_container.insert({coords.front(), c_1});
    }

    auto c_back = crossroad_container.find(coords.back());
    Crossroad_ptr c_2;
    if (c_back != crossroad_container.end()) {
        c_2 = c_back->second;
    } else {
        c_2 = std::make_shared<Crossroad>(coords.back());
        crossroad_container.insert({coords.back(), c_2});
    }

    c_1->crossroad_way.insert({id, c_2});
    c_2->crossroad_way.insert({id, c_1});

    Way_ptr new_way = std::make_shared<Way>(id, coords, std::make_pair(c_1, c_2));
    way_container.insert({id, new_way});

    return true;
}

std::vector<std::pair<WayID, Coord>> Datastructures::ways_from(Coord xy)
{
    auto crossroad = crossroad_container.find(xy);
    if (crossroad == crossroad_container.end()) return {};

    std::vector<std::pair<WayID, Coord>> result;
    result.reserve(crossroad->second->crossroad_way.size());

    for (auto const& c : crossroad->second->crossroad_way) {
        result.push_back(std::make_pair(c.first, c.second->crossroad_coord));
    }

    return result;
}

std::vector<Coord> Datastructures::get_way_coords(WayID id)
{
    auto way = way_container.find(id);
    if (way == way_container.end()) return {NO_COORD};

    return way->second->way_coords;
}

void Datastructures::clear_ways()
{
    crossroad_container.clear();
    way_container.clear();
}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_any(Coord fromxy, Coord toxy)
{
    auto start_point = crossroad_container.find(fromxy);
    auto end_point = crossroad_container.find(toxy);
    if (start_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};
    if (end_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};

    if (fromxy == toxy) return {};

    std::queue<Crossroad_ptr> crossroad_queue;
    std::vector<Crossroad_ptr> visited;

    end_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);
    start_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);

    crossroad_queue.push(start_point->second);


    while (!crossroad_queue.empty()) {
        Crossroad_ptr current = crossroad_queue.front();
        visited.push_back(current);
        current->visited = true;
        crossroad_queue.pop();

        for (auto const& crossroad : current->crossroad_way) {
            if (!crossroad.second->visited) {
                crossroad_queue.push(crossroad.second);
                crossroad.second->pre_crossroad = std::make_pair(crossroad.first, current);
                if (crossroad.second->crossroad_coord == toxy) {
                    goto finish_loop;
                }
            }
        }
    }

    finish_loop:
    if (end_point->second->pre_crossroad.second == nullptr) return {};

    auto current_node = end_point->second;
    std::vector<Crossroad_ptr> temp_v;
    while (current_node != start_point->second) {
        temp_v.push_back(current_node);
        current_node = current_node->pre_crossroad.second;
    }
    temp_v.push_back(current_node);

    std::vector<std::tuple<Coord, WayID, Distance>> result;
    long long int distance = 0;
    for (auto iter = temp_v.end() - 1; iter != temp_v.begin(); iter--) {
        auto find_way = (*(iter-1))->pre_crossroad.first;
        result.push_back(std::make_tuple((*iter)->crossroad_coord, find_way, distance));
        distance += way_container[find_way]->way_distance;
    }
    result.push_back(std::make_tuple(temp_v.front()->crossroad_coord, NO_WAY, distance));
    for (const auto &cross: visited) {
        cross->visited = false;
    }
    return result;
}

bool Datastructures::remove_way(WayID id)
{
    auto way = way_container.find(id);
    if (way == way_container.end()) return false;

    Crossroad_ptr c_1 = way->second->way_crossroads.first;
    Crossroad_ptr c_2 = way->second->way_crossroads.second;

    auto removed_way_1 = c_1->crossroad_way.find(id);
    if (removed_way_1 != c_1->crossroad_way.end()) {
        c_1->crossroad_way.erase(removed_way_1);
        if (c_1->crossroad_way.empty()) {
            auto coord_1 = crossroad_container.find(c_1->crossroad_coord);
            if (coord_1 != crossroad_container.end()) {
                c_2->crossroad_way[id] = nullptr;
                crossroad_container.erase(coord_1);
            }
        }
    }

    auto removed_way_2 = c_2->crossroad_way.find(id);
    if (removed_way_2 != c_2->crossroad_way.end()) {
        c_2->crossroad_way.erase(removed_way_2);
        if (c_2->crossroad_way.empty()) {
            auto coord_2 = crossroad_container.find(c_2->crossroad_coord);
            if (coord_2 != crossroad_container.end()) {
                crossroad_container.erase(coord_2);
            }
        }
    }

    way_container.erase(way);
    return true;

}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_least_crossroads(Coord fromxy, Coord toxy)
{
    auto start_point = crossroad_container.find(fromxy);
    auto end_point = crossroad_container.find(toxy);
    if (start_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};
    if (end_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};

    if (fromxy == toxy) return {};

    std::queue<Crossroad_ptr> crossroad_queue;
    std::vector<Crossroad_ptr> visited;

    end_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);
    start_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);

    crossroad_queue.push(start_point->second);


    while (!crossroad_queue.empty()) {
        Crossroad_ptr current = crossroad_queue.front();
        visited.push_back(current);
        current->visited = true;
        crossroad_queue.pop();

        for (auto const& crossroad : current->crossroad_way) {
            if (!crossroad.second->visited) {
                crossroad_queue.push(crossroad.second);
                crossroad.second->pre_crossroad = std::make_pair(crossroad.first, current);
                if (crossroad.second->crossroad_coord == toxy) {
                    goto finish_loop;
                }
            }
        }
    }

    finish_loop:
    if (end_point->second->pre_crossroad.second == nullptr) return {};

    auto current_node = end_point->second;
    std::vector<Crossroad_ptr> temp_v;
    while (current_node != start_point->second) {
        temp_v.push_back(current_node);
        current_node = current_node->pre_crossroad.second;
    }
    temp_v.push_back(current_node);

    std::vector<std::tuple<Coord, WayID, Distance>> result;
    long long int distance = 0;
    for (auto iter = temp_v.end() - 1; iter != temp_v.begin(); iter--) {
        auto find_way = (*(iter-1))->pre_crossroad.first;
        result.push_back(std::make_tuple((*iter)->crossroad_coord, find_way, distance));
        distance += way_container[find_way]->way_distance;
    }
    result.push_back(std::make_tuple(temp_v.front()->crossroad_coord, NO_WAY, distance));
    for (const auto &cross: visited) {
        cross->visited = false;
    }
    return result;
}

std::vector<std::tuple<Coord, WayID> > Datastructures::route_with_cycle(Coord fromxy)
{
    auto start_point = crossroad_container.find(fromxy);
    if (start_point == crossroad_container.end()) return {{NO_COORD, NO_WAY}};

    std::stack<Crossroad_ptr> visit_crossroad;
    Crossroad_ptr last_point, second_last_point;
    bool way_found = false;
    std::vector<Crossroad_ptr> reset_crossroad;

    start_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);

    visit_crossroad.push(start_point->second);

    while (!visit_crossroad.empty()) {
        auto current = visit_crossroad.top();
        current->visited = true;
        reset_crossroad.push_back(current);
        bool available = false;
        for (auto const& crossroad : current->crossroad_way) {
            if (crossroad.second) {
                if (crossroad.second->visited &&
                        current->pre_crossroad.first != crossroad.first &&
                        crossroad.second->pre_crossroad.second != current) {
                    way_found = true;
                    last_point = crossroad.second;
                    second_last_point = current;
                    goto finish_loop;
                } else if (!crossroad.second->visited) {
                    visit_crossroad.push(crossroad.second);
                    available = true;
                    crossroad.second->pre_crossroad = std::make_pair(crossroad.first, current);
                }
            }
        }
        if (!available) {
            visit_crossroad.pop();
        }
    }
    finish_loop:
    if (!way_found) {
        for (const auto &cross: reset_crossroad) {
            cross->visited = false;
            cross->pre_crossroad = std::make_pair(NO_WAY, nullptr);
        }
        return {};
    };

    auto current_node = second_last_point;
    std::vector<Crossroad_ptr> temp_v;
    while (current_node != start_point->second) {
        temp_v.push_back(current_node);
        current_node = current_node->pre_crossroad.second;
    }
    temp_v.push_back(current_node);
    std::reverse(temp_v.begin(), temp_v.end());

    std::vector<std::tuple<Coord, WayID>> result;
    for (auto iter = temp_v.begin(); iter != temp_v.end() - 1; iter++) {
        auto find_way = (*(iter+1))->pre_crossroad.first;
        result.push_back(std::make_tuple((*iter)->crossroad_coord, find_way));
    }
    auto last_way = std::find_if(std::begin(last_point->crossroad_way), std::end(last_point->crossroad_way),
                                 [second_last_point](auto && pair) {
                                          return pair.second->crossroad_coord == second_last_point->crossroad_coord;
                                  });
    result.push_back(std::make_tuple(temp_v.back()->crossroad_coord, last_way->first));
    result.push_back(std::make_tuple(last_point->crossroad_coord, NO_WAY));
    for (const auto &cross: reset_crossroad) {
        cross->visited = false;
        cross->pre_crossroad = std::make_pair(NO_WAY, nullptr);
    }
    return result;
}

std::vector<std::tuple<Coord, WayID, Distance> > Datastructures::route_shortest_distance(Coord fromxy, Coord toxy)
{
    auto start_point = crossroad_container.find(fromxy);
    auto end_point = crossroad_container.find(toxy);
    if (start_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};
    if (end_point == crossroad_container.end()) return {{NO_COORD, NO_WAY, NO_DISTANCE}};

    if (fromxy == toxy) return {};

    struct compare_distance {
        bool operator()(const Crossroad_ptr& ptr1,const Crossroad_ptr& ptr2)
        {
            return ptr1->distance_for_dijkstra > ptr2->distance_for_dijkstra;
        }
    };

    std::priority_queue<Crossroad_ptr, std::vector<Crossroad_ptr>, compare_distance> queue;
    std::vector<std::tuple<Coord, WayID, Distance>> result;
    std::vector<Crossroad_ptr> visited_crossroad;
    std::vector<Crossroad_ptr> distanceModified;

    queue.push(start_point->second);

    start_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);
    end_point->second->pre_crossroad = std::make_pair(NO_WAY, nullptr);

    start_point->second->distance_for_dijkstra = 0;

    while (!queue.empty() && !end_point->second->visited) {
        Crossroad_ptr current = queue.top();
        queue.pop();
        current->visited = true;
        visited_crossroad.push_back(current);
        for (auto const& cross: current->crossroad_way) {
            if (!cross.second->visited) {
                Distance temp = current->distance_for_dijkstra + way_container[cross.first]->way_distance;
                if (temp < cross.second->distance_for_dijkstra) {
                    cross.second->distance_for_dijkstra = temp;
                    cross.second->pre_crossroad = std::make_pair(cross.first,current);
                    distanceModified.push_back(cross.second);
                }
                queue.push(cross.second);
            }
        }
    }

    for (const auto &cross: distanceModified) {
        cross->distance_for_dijkstra = INF_DISTANCE;
    }

    if (end_point->second->pre_crossroad.second == nullptr) {
        for (const auto &cross: visited_crossroad) {
            cross->visited = false;
            cross->pre_crossroad = std::make_pair(NO_WAY, nullptr);

        }
        return {};
    }

    Crossroad_ptr current = end_point->second;
    WayID prevID = NO_WAY;
    while (current->pre_crossroad.second) {
        result.push_back({current->crossroad_coord, prevID, way_container[current->pre_crossroad.first]->way_distance});
        prevID = current->pre_crossroad.first;
        current = current->pre_crossroad.second;
    }
    result.push_back({start_point->second->crossroad_coord, prevID, 0});

    std::reverse(result.begin(), result.end());
    for (unsigned int i = 1; i < result.size(); i++) {
        std::get<2>(result[i]) += std::get<2>(result[i-1]);
    }
    for (const auto &cross: visited_crossroad) {
        cross->visited = false;
        cross->pre_crossroad = std::make_pair(NO_WAY, nullptr);

    }
    return result;
}

Distance Datastructures::trim_ways()
{
    std::vector<WayID> way_sorted_v;
    way_sorted_v.reserve(way_container.size());
    way_sorted_v = all_ways();

    std::sort(way_sorted_v.begin(), way_sorted_v.end(),
              [this](WayID a, WayID b){
        return way_container[a]->way_distance < way_container[b]->way_distance;
    });

    std::unordered_map<WayID, Way_ptr> temp_way_container = way_container;
    unsigned int temp_crossroad_container_size = crossroad_container.size();

    clear_ways();

    for (const WayID& way : way_sorted_v) {
        auto coords = temp_way_container[way]->way_coords;
        add_way(way, coords);
        auto cycle_front = route_with_cycle(coords.front());
        if (!cycle_front.empty()) {
            remove_way(way);
        }
        if (way_container.size() == (temp_crossroad_container_size - 1)) {
            goto finish_loop;
        }
    }

    finish_loop:
    Distance total_distance = 0;
    for (const auto& way: way_container) {
        total_distance += way.second->way_distance;
    }
    for (const auto &coord : crossroad_container) {
        auto cycle = route_with_cycle(coord.first);
        if (!cycle.empty()) {
        }
    }
    return total_distance;
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

Distance Datastructures::distance_calculator(Coord fromPoint, Coord toPoint)
{
    return static_cast<Distance>(toPoint.x - fromPoint.x)*static_cast<long long int>(toPoint.x - fromPoint.x) +
            static_cast<Distance>(toPoint.y - fromPoint.y)*static_cast<long long int>(toPoint.y - fromPoint.y);;
}
