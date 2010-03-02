#ifndef _MAP_DEGREE_SUM_H_
#define _MAP_DEGREE_SUM_H_

/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <sstream>
#include <map>

using std::map;

/*
 * A generic degree sum for weighted graphs
 * this is a generalization of InOutBoth used in directed graphs
 * because the weights can be arbitrarily large instead of just 0, 1, or 2.
 */
template <typename weight_t>
struct MapDegreeSum
{
    // increment the weight k's counter
    MapDegreeSum<weight_t> &operator+=(int k)
    {
        weight_map[k] += 1;

        return *this;
    }

    // only valid when k == 0, in which case clear
    MapDegreeSum<weight_t> &operator=(int k)
    {
        if (k == 0)
        {
            weight_map.clear();
        }
        else
        {
            std::stringstream ss;
            ss << k << " used instead of 0 to clear " << typeid(*this).name();
            fail(ss.str() );
        }

        return *this;
    }

    // compare two mappings lexicographically
    bool operator<(const MapDegreeSum<weight_t> &a) const
    {
        typename map<weight_t, size_t>::const_iterator this_it;
        typename map<weight_t, size_t>::iterator a_it;

        // compare elementwise
        for (this_it = weight_map.begin();
            this_it != weight_map.end(); this_it++)
        {
            // if a_it is exhausted but this_it isn't
            if (a_it == a.weight_map.end() )
            {
                return false;
            }

            // now just compare elementwise via the (weight, count) pairs
            if ((*this_it) < (*a_it) )
            {
                return true;
            }
            else if ((*this_it) > (*a_it) )
            {
                return false;
            }
        }

        // if there's still some things left over in a_it, this is smaller
        if (a_it != a.weight_map.end() )
        {
            return true;
        }

        return false;
    }

    bool operator==(const MapDegreeSum<weight_t> &a) const
    {
        return weight_map == a.weight_map;
    }


    map<weight_t, size_t> weight_map;
};


#endif  // _MAP_DEGREE_SUM_H_
