/*
 * Copyright 2009 Greg Tener
 * Released under the Lesser General Public Liscense version 3.
 */

#include "nishe/Util.h"

#include <cstdlib>
#include <cstdio>

#include <sstream>
#include <string>
#include <vector>


using std::vector;
using std::string;

namespace nishe {

// expand a string like "0:3 5 4" --> [ 0 1 2 3 5 4 ]
vector<int> expand_range(string s) {
  stringstream ss(s);
  string token;
  vector<int> v;

  ss >> token;

  while (!ss.fail()) {
    if (token.find(':') == string::npos) {
      v.push_back(atoi(token.c_str()));
    } else {
      int low = 0;
      int high = 0;

      sscanf(token.c_str(), "%d:%d", &low, &high);  // NOLINT

      for (int i = low; i <= high; i++) {
        v.push_back(i);
      }
    }

    ss >> token;
  }

  return v;
}

vector<vector<int> > string2cells(string s) {
  vector<vector<int> > cells;

  // remove the [ and ]
  s[0] = ' ';
  s[s.length() - 1] = ' ';

  vector<string> cell_strs = split(s, "|");

  for (int i = 0; i < cell_strs.size(); i++) {
    vector<int> cell = expand_range(cell_strs[i]);

    if (cell.size() > 0) {
      cells.push_back(cell);
    }
  }

  return cells;
}

vector<string> split(string s, string delim) {
  vector<string> v;
  int pos = 0;
  int start = 0;

  pos = s.find(delim, start);

  while (pos != string::npos) {
    v.push_back(s.substr(start, pos - start));
    pos += delim.length();
    start = pos;
    pos = s.find(delim, start);
  }

  if (start != 0) {
    v.push_back(s.substr(start, s.length() - start));
  } else {  // delim was not found
    v.push_back(s);
  }

  return v;
}

string runlength_compress(const vector<int> &v, string sExp) {
  int i = 0;
  stringstream ss;
  int nLength = 0;

  ss << v[0];
  nLength = 1;

  for (i = 1; i < v.size(); i++) {
    if (v[i - 1] == v[i]) {
      nLength += 1;
    } else if (nLength > 1) {
      ss << sExp << nLength;
      ss << " " << v[i];
      nLength = 1;
    } else {
      ss << " " << v[i];
      nLength = 1;
    }
  }

  if (nLength > 1) {
    ss << sExp << nLength;
  }

  return ss.str();
}

string vector2str(const vector<int> &v, bool brackets) {
  stringstream ss;
  int i = 0;

  if (brackets) {
    ss << "[ ";
  }

  for (i = 0; i < v.size(); i++) {
    ss << v[i] << " ";
  }

  if (brackets) {
    ss << "]";
  }

  return ss.str();
}

string intarray2str(int *an, int n) {
  stringstream ss;
  int i = 0;

  ss << "[";

  for (i = 0; i < n; i++) {
    ss << an[i] << " ";
  }

  ss << "]";

  return ss.str();
}

/*
 * Factors the permutation 'an' into a disjoint union of cycles where a negative value indicates
 * the end of a cycle. For instance (1 2 3) (4 5) would be [1 2 -3 4 -5]
 */
int factor_cycles(int *anCycles, int n, int *an) {
  int i = 0;
  int nSize = 0;  // size of cycle
  int nTotal = 0;  // sum of sizes of cycles
  int nCurr = 0;

  // currently goes from 0...n-1
  // map onto -1...-n if they're not fixed points
  for (i = 0; i < n; i++) {
    if (an[i] != i) {
      an[i] = -(an[i] + 1);
    }
  }

  for (i = 0; i < n; i++) {
    // if we're negative we've never been seen yet
    if (an[i] < 0) {
      nCurr = i;

      // go until we find the beginning
      while (-an[nCurr] - 1 != i) {
        anCycles[nTotal] = nCurr;
        an[nCurr] = -an[nCurr] - 1;
        nCurr = an[nCurr];

        nTotal += 1;
      }

      // finish off the cycle with a negative value
      // and add the last element of it
      anCycles[nTotal] = -nCurr;
      an[nCurr] = -an[nCurr] - 1;
      nTotal += 1;
    }
  }

  return nTotal;
}

}  // namespace nishe
