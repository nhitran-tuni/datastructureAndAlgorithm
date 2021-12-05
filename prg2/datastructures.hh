// Datastructures.hh

#ifndef DATASTRUCTURES_HH
#define DATASTRUCTURES_HH

#include <string>
#include <vector>
#include <tuple>
#include <utility>
#include <limits>
#include <functional>
#include <memory>
#include <iostream>
#include <map>
#include <math.h>
#include <queue>
#include <stack>
#include <QDebug>
#include <QString>
#include <unordered_set>

// Types for IDs
using PlaceID = long long int;
using AreaID = long long int;
using Name = std::string;
using WayID = std::string;

// Return values for cases where required thing was not found
PlaceID const NO_PLACE = -1;
AreaID const NO_AREA = -1;
WayID const NO_WAY = "!!No way!!";

// Return value for cases where integer values were not found
int const NO_VALUE = std::numeric_limits<int>::min();

// Return value for cases where name values were not found
Name const NO_NAME = "!!NO_NAME!!";

// Enumeration for different place types
// !!Note since this is a C++11 "scoped enumeration", you'll have to refer to
// individual values as PlaceType::SHELTER etc.
enum class PlaceType { OTHER=0, FIREPIT, SHELTER, PARKING, PEAK, BAY, AREA, NO_TYPE };

// Type for a coordinate (x, y)
struct Coord
{
    int x = NO_VALUE;
    int y = NO_VALUE;
};

// Example: Defining == and hash function for Coord so that it can be used
// as key for std::unordered_map/set, if needed
inline bool operator==(Coord c1, Coord c2) { return c1.x == c2.x && c1.y == c2.y; }
inline bool operator!=(Coord c1, Coord c2) { return !(c1==c2); } // Not strictly necessary

struct CoordHash
{
    std::size_t operator()(Coord xy) const
    {
        auto hasher = std::hash<int>();
        auto xhash = hasher(xy.x);
        auto yhash = hasher(xy.y);
        // Combine hash values (magic!)
        return xhash ^ (yhash + 0x9e3779b9 + (xhash << 6) + (xhash >> 2));
    }
};

// Example: Defining < for Coord so that it can be used
// as key for std::map/set
inline bool operator<(Coord c1, Coord c2)
{
    if (c1.y < c2.y) { return true; }
    else if (c2.y < c1.y) { return false; }
    else { return c1.x < c2.x; }
}

// Return value for cases where coordinates were not found
Coord const NO_COORD = {NO_VALUE, NO_VALUE};

// Type for a distance (in metres)
using Distance = int;

// Return value for cases where Duration is unknown
Distance const NO_DISTANCE = NO_VALUE;



// This is the class you are supposed to implement

class Datastructures
{
public:
    Datastructures();
    ~Datastructures();

    // Estimate of performance: O(1)
    // Short rationale for estimate: using map.size() (O(1))
    int place_count();

    // Estimate of performance: O(n)
    // Short rationale for estimate: using map/multimap.clear() (O(n))
    void clear_all();

    // Estimate of performance: O(n)
    // Short rationale for estimate: loop through the map to retrieve all keys
    // which are PlaceID then push_back to the vector whose complexity is O(1)
    std::vector<PlaceID> all_places();

    // Estimate of performance: O(log(n))-average
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Otherwise, add element to the map (complexity O(log(n)))
    bool add_place(PlaceID id, Name const& name, PlaceType type, Coord xy);

    // Estimate of performance: O(1)-average
    // Short rationale for estimate:find in the map if the id is exist (complexity O(1))
    // Otherwise, find the value of the given id and return the name and place type (complexity O(1))
    std::pair<Name, PlaceType> get_place_name_type(PlaceID id);

    // Estimate of performance: O(1)-average
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Otherwise, find the value of the given id and return coord (complexity O(1))
    Coord get_place_coord(PlaceID id);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(n)
    // Short rationale for estimate: Initialize a vector containing only Place
    // loop through the multimap O(n) and push_back to vector (O(1))
    std::vector<PlaceID> places_alphabetically();

    // Estimate of performance: O(n)
    // Short rationale for estimate: Initialize a vector containing only Coord
    // loop through the multimap O(n) and push_back to vector (O(1))
    std::vector<PlaceID> places_coord_order();

    // Estimate of performance: O(logn)
    // Short rationale for estimate: find range of element in multimap having given name (O(logn))
    // Loop through that range to push_back to result vector (O(logn))
    std::vector<PlaceID> find_places_name(Name const& name);

    // Estimate of performance: O(n)
    // Short rationale for estimate:loop through the map (complexity O(n))
    // then push id with given type to the vector (complexity O(1))
    std::vector<PlaceID> find_places_type(PlaceType type);

    // Estimate of performance: O(logn)
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Remove element in multimap O(logn) and insert new item to multimap (O(logn))
    bool change_place_name(PlaceID id, Name const& newname);

    // Estimate of performance: O(1)-average
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Otherwise, find the value of the given id and change the coord (complexity O(1))
    bool change_place_coord(PlaceID id, Coord newcoord);

    // We recommend you implement the operations below only after implementing the ones above

    // Estimate of performance: O(1)-average
    // Short rationale for estimate:find in the map if the id is exist (complexity O(1))
    // Otherwise, add element to the map (complexity O(1)))
    bool add_area(AreaID id, Name const& name, std::vector<Coord> coords);

    // Estimate of performance: O(1)-average
    // Short rationale for estimate:find in the map if the id is exist (complexity O(1))
    // Otherwise, find the value of the given id and return name (complexity O(1))
    Name get_area_name(AreaID id);

    // Estimate of performance: O(1)-average
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Otherwise, find the value of the given id and return vector of coords (complexity O(1))
    std::vector<Coord> get_area_coords(AreaID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: loop through the map to retrieve all keys
    // which are AreaID then push_back to the vector whose complexity is O(1)
    std::vector<AreaID> all_areas();

    // Estimate of performance: O(1)-average
    // Short rationale for estimate: find if id and parent id is exist (complexity O(1))
    // Check if the child is belong to another area (complexity O(1))
    // Access to the child area to change its propertie (complexity O(1))
    bool add_subarea_to_area(AreaID id, AreaID parentid);

    // Estimate of performance: O(n)
    // Short rationale for estimate: find if the given id exist (complexity O(1))
    // while loop to find all parent are of given area. In the loop push the AreaID to
    // result vector (O(1)) and find the parent of the present parens (O(1)).
    // The best case that the given has only one parent O(1). Average and worst is O(n)
    std::vector<AreaID> subarea_in_areas(AreaID id);

    // Non-compulsory operations

    // Estimate of performance: O(1)
    // Short rationale for estimate: nothing
    void creation_finished();

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // loop through the area map and find the parents of each area (O(n)) and then find if the given
    // id is in that vector (O(n))
    std::vector<AreaID> all_subareas_in_area(AreaID id);

    // Estimate of performance: O(nlogn)
    // Short rationale for estimate: Initialize a vector containing only Place (O(n))
    // Using sort build-in function with lambda function (O(nlogn))
    // Initialize the result vector containing the PlaceID according to the name (O(n))
    // If size of result is larger than 3, it will only take the three last element (O(1))
    // Total (2n+nlogn + const) -> O(nlogn)
    std::vector<PlaceID> places_closest_to(Coord xy, PlaceType type);

    // Estimate of performance: O(logn)-average
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // Remove element in multimap (O(logn))
    // Otherwise, erase element by position/iterator (complexity O(1))
    bool remove_place(PlaceID id);

    // Estimate of performance: O(n^2)
    // Short rationale for estimate: find in the map if the id is exist (complexity O(1))
    // get all parents of given ids (linear)
    // for loop to loop through one of the parents vector and find if the other vector contains (n^2)
    AreaID common_area_of_subareas(AreaID id1, AreaID id2);


    // Phase 2 operations

    // Estimate of performance: O(n)
    // Short rationale for estimate: loop through the map to retrieve all keys
    // which are WayID then push_back to the vector whose complexity is O(1)
    std::vector<WayID> all_ways();

    // Estimate of performance: O(1)
    // Short rationale for estimate: find and insert in unorder takes constant time.
    bool add_way(WayID id, std::vector<Coord> coords);

    // Estimate of performance: O(n)
    // Short rationale for estimate: reserve in vector takes O(n), loop through all another
    // crossroads of one crossroad takes O(n). Therefore, the total is O(n)
    std::vector<std::pair<WayID, Coord>> ways_from(Coord xy);

    // Estimate of performance: O(1)
    // Short rationale for estimate: find element in unorder_map takes O(n)
    std::vector<Coord> get_way_coords(WayID id);

    // Estimate of performance: O(n)
    // Short rationale for estimate: using clear takes O(n), linear to the size
    void clear_ways();

    // Estimate of performance:O(V+E)
    // Short rationale for estimate: Using BFS, where V is the vertices and E is the edges
    std::vector<std::tuple<Coord, WayID, Distance>> route_any(Coord fromxy, Coord toxy);

    // Non-compulsory operations

    // Estimate of performance: O(n)
    // Short rationale for estimate: using find takes O(1), using erase takes O(n)
    // so overall is O(n)
    bool remove_way(WayID id);

    // Estimate of performance:O(V+E)
    // Short rationale for estimate: Using BFS, where V is the vertices and E is the edges
    std::vector<std::tuple<Coord, WayID, Distance>> route_least_crossroads(Coord fromxy, Coord toxy);

    // Estimate of performance:O(V+E)
    // Short rationale for estimate: Using BFS, where V is the vertices and E is the edges
    std::vector<std::tuple<Coord, WayID>> route_with_cycle(Coord fromxy);

    // Estimate of performance: O((E+V)logV)
    // Short rationale for estimate: Using Dijkstra algorithm, where E is the edges and V is vertices
    std::vector<std::tuple<Coord, WayID, Distance>> route_shortest_distance(Coord fromxy, Coord toxy);

    // Estimate of performance: O(|E|^2 + |E||V|log|V|)
    // Short rationale for estimate: using Kruscal algorithm, where E is the edges and V is the vertices
    Distance trim_ways();

private:
    // Add stuff needed for your class implementation here

    struct Place {
        PlaceID place_id;
        Name place_name;
        PlaceType place_type;
        Coord place_coord;

        //Constructor
        Place(PlaceID id_, Name name_, PlaceType type_, Coord xy) {
            place_id = id_;
            place_name = name_;
            place_type = type_;
            place_coord = xy;
        }
    };

    struct Area {
        AreaID area_id;
        Name area_name;
        std::vector<Coord> area_coords;
        std::shared_ptr<Area> parent_area = nullptr;
        std::vector<std::shared_ptr<Area>> subareas;

        //Constructor
        Area(AreaID id_, Name name_, std::vector<Coord> coords_) {
            area_id = id_;
            area_name = name_;
            area_coords = coords_;
        }
    };

    using Place_ptr = std::shared_ptr <Place>;
    using Area_ptr =  std::shared_ptr<Area>;

    std::unordered_map<PlaceID, Place_ptr> place_container;
    std::multimap<Name, PlaceID> place_name_container;
    std::multimap<long long int, PlaceID> place_distance_container;

    std::unordered_map<AreaID, Area_ptr> area_container;

    void remove_name_multimap(Name name, PlaceID id);
    void remove_distance_multimap(Coord xy, PlaceID id);
    Distance distance_calculator(Coord fromPoint, Coord toPoint);

    //Phase 2
    const Distance INF_DISTANCE= 999999;

    struct Crossroad {
        Coord crossroad_coord;
        std::unordered_map<WayID, std::shared_ptr<Crossroad>> crossroad_way;
        bool visited = false;
        std::pair<WayID, std::shared_ptr<Crossroad>> pre_crossroad;
        Distance distance_for_dijkstra = 999999;

        //Constructor
        Crossroad(Coord xy_) {
            crossroad_coord = xy_;
        }
    };

    using Crossroad_ptr = std::shared_ptr<Crossroad>;
    using Coord_v = std::vector<Coord>;

    struct Way {
        WayID way_id;
        Coord_v way_coords;
        Distance way_distance = 0;
        std::pair<Crossroad_ptr, Crossroad_ptr> way_crossroads;

        //Constructor
        Way (WayID id_, Coord_v v_, std::pair<Crossroad_ptr, Crossroad_ptr> c_pair_){
            way_id = id_;
            way_coords = v_;
            way_crossroads = c_pair_;

            for (unsigned long i = 0; i < v_.size() - 1; i++) {
                Datastructures d;
                way_distance += static_cast<Distance>(floor(sqrt(d.distance_calculator(v_[i], v_[i+1]))));
            }
        }
    };

    using Way_ptr = std::shared_ptr<Way>;

    std::unordered_map<WayID, Way_ptr> way_container;
    std::unordered_map<Coord, Crossroad_ptr, CoordHash> crossroad_container;
};

#endif // DATASTRUCTURES_HH
