#ifndef INCLUDE_NISHE_UTIL_H_
#define INCLUDE_NISHE_UTIL_H_

/*
  Copyright 2010 Greg Tener
  Released under the Lesser General Public Liscense version 3.
*/

#include <string>
#include <vector>
#include <cassert>
#include <sstream>
#include <map>
#include <deque>
#include <set>

using std::string;
using std::vector;
using std::stringstream;
using std::ostream;
using std::map;
using std::set;
using std::deque;
using std::pair;

namespace nishe {

// Converts a vector to a string of 1**k for every 1, 1, ... 1 (k times)
string runlength_compress(const vector<int> &v, string sExp = "**");

string vector2str(const vector<int> &v, bool brackets = true);
string intarray2str(int *an, int n);

int factor_cycles(int *anCycles, int n, int *an);

// splits the string at delim and returns the pieces
vector<string> split(string s, string delim);

// converts a string representation of a partition into a
// vector<vector<int> > one
vector<vector<int> > string2cells(string s);

// Convert gets a string of form 1-n for every 1, 2, ..., n for compact output.
template<typename T>
string range_compress(const vector<T> &v) {
  stringstream ss;
  int i = 0;
  int nRun = 0;

  if (v.size() == 0) {
    return "";
  }

  ss << v[0];
  nRun = 1;

  for (i = 1; i < v.size(); i++) {
    if (v[i - 1] + 1 == v[i]) {  // increase by 1
      nRun += 1;
    } else if (nRun >= 3) {  // end of run, but compressable
      ss << ":" << v[i - 1] << " ";

      ss << v[i];
      nRun = 1;
    } else {  // end of run, but not enough to compress, nRun == 1 or nRun == 2
      ss << " ";

      if (nRun == 2) {
        ss << v[i - 1] << " ";
      }

      ss << v[i];
      nRun = 1;
    }
  }

  if (nRun >= 3) {
    ss << ":" << v[i - 1];
  } else if (nRun == 2) {
    ss << " " << v[i - 1];
  }

  return ss.str();
}

}  // namespace nishe


/*
 * These I/O operators need to be outside of the namespace as far as I know.
 */

template<typename T>
ostream &operator<<(ostream &os, const vector<T> &v) {
  int i = 0;

  os << "[ ";

  for (i = 0; i < v.size(); i++) {
    os << v[i] << " ";
  }

  os << "]";

  return os;
}

template<typename K, typename V>
ostream &operator<<(ostream &os, const map<K, V> &m) {
  typename map<K, V>::const_iterator it;

  os << "{";

  for (it = m.begin(); it != m.end(); it++) {
    if (it != m.begin()) {
      os << ", ";
    }

    os << (*it).first << ": " << (*it).second;
  }

  os << "}";

  return os;
}

template<typename T>
ostream &operator<<(ostream &os, const set<T> &c) {
  typename set<T>::const_iterator it;

  os << "{";

  for (it = c.begin(); it != c.end(); it++) {
    if (it != c.begin()) {
      os << ", ";
    }

    os << (*it);
  }

  os << "}";

  return os;
}

template<typename T, class C>
ostream &operator<<(ostream &os, const pair<T, C> &c) {
  os << "(" << c.first << ", " << c.second << ")";

  return os;
}

template<typename T>
ostream &operator<<(ostream &os, const deque<T> &c) {
  int i = 0;

  os << "[ ";

  for (i = 0; i < c.size(); i++) {
    os << c[i] << " ";
  }

  os << "]";

  return os;
}

template<typename T>
string str(const T &x) {
  stringstream ss;

  ss << x;

  return ss.str();
}

#endif  // INCLUDE_NISHE_UTIL_H_
