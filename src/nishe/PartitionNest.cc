/*
    Copyright 2010 Greg Tener
    Released under the Lesser General Public License v3.
*/

#include <nishe/PartitionNest.h>
#include <nishe/Util.h>

#include <ostream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <vector>
#include <set>
#include <string>

using std::make_pair;
using std::cout;
using std::endl;

namespace nishe {

PartitionNest::PartitionNest()
{
}

int PartitionNest::unit(int n)
{
    int i = 0;

    if (n == 0)
    {
        return 0;
    }

    // set the elements to the integers from 0 to n - 1
    // the index containing to 0 for all
    // and the cell sizes to n for the 0th index
    vElements.resize(n);
    vIndexContaining.resize(n);

    for (i = 0; i < n; i++)
    {
        vElements[i] = i;
        vIndexContaining[i] = 0;
    }

    vCellSizes.resize(n);
    vCellSizes[0] = n;

    // clear all pending new indices and the stack
    vNewIndexQueue.clear();
    vNewIndices.clear();
    vNewIndicesAtLevel.clear();

    // start off with no new indices at level 0
    vNewIndicesAtLevel.push_back(0);

    // reset the nontrivial list
    // the first one points to the last element, and vice versa,
    // this is okay because n - 1 cannot ever be a nontrivial index
    vNontrivialList.resize(n);
    vNontrivialList.back() = make_pair(0, 0);
    vNontrivialList[0] = make_pair(n - 1, n - 1);

    return n;
}

int PartitionNest::size() const
{
    return vElements.size();
}

int PartitionNest::length() const
{
    if (size() == 0)
    {
        return 0;
    }

    return vNewIndices.size() + 1;
}

int PartitionNest::cell_size(int k) const
{
    return vCellSizes[k];
}

int PartitionNest::index_containing(int u) const
{
    return vIndexContaining[u];
}

int *PartitionNest::elements()
{
    return &vElements[0];
}

int PartitionNest::level()
{
    return vNewIndicesAtLevel.size() - 1;
}

bool PartitionNest::is_index(int k) const
{
    return vIndexContaining[vElements[k]] == k;
}

bool PartitionNest::is_nontrivial_index(int k) const
{
    return is_index(k) && vCellSizes[k] > 1;
}

bool PartitionNest::is_discrete() const
{
    return length() == size();
}

int PartitionNest::next_index(int k) const
{
    return k + vCellSizes[k];
}

int PartitionNest::terminal_index() const
{
    return size();
}

int PartitionNest::first_nontrivial_index() const
{
    int second = vNontrivialList[size() - 1].second;

    if (second < size() - 1)
    {
        return second;
    }

    return terminal_index();
}

int PartitionNest::next_nontrivial_index(int k) const
{
    int second = vNontrivialList[k].second;

    if (second < size() - 1)
    {
        return second;
    }

    return terminal_index();
}

bool PartitionNest::add_index(int k)
{
    bool rv = enqueue_new_index(k);
    commit_pending_indices();

    return rv;
}

bool PartitionNest::enqueue_new_index(int k)
{
    if (!is_index(k) )
    {
        vNewIndexQueue.push_back(k);

        return true;
    }

    return false;
}

void PartitionNest::commit_pending_indices()
{
    int i = 0;

    while (i < vNewIndexQueue.size() )
    {
        int k_next = 0;  // the next place to add a split
        int k = vNewIndexQueue[i];
        int u = vElements[k];

        // the cell containing the run of splits
        int first = vIndexContaining[u];
        int first_size = vCellSizes[first];

        // a moving starting point of the current cell we're adding a split to
        int start = first;

        while (i + 1 < vNewIndexQueue.size() )
        {
            k_next = vNewIndexQueue[i + 1];
            u = vElements[k_next];

            // check that this index is in the same cell as all of the others
            if (vIndexContaining[u] != first)
            {
                break;
            }

            commit_index(start, k, k_next);

            start = k;
            k = k_next;
            i += 1;
        }

        k_next = first + first_size;
        commit_index(start, k, k_next);

        i += 1;
    }

    vNewIndexQueue.clear();
}

void PartitionNest::commit_index(int nStart, int k, int k_next)
{
    int i = 0;
    int nStartSize = 0;

    // set the elements to be members of the new cell
    for (i = k; i < k_next; i++)
    {
        vIndexContaining[vElements[i]] = k;
    }

    // update the cell sizes
    nStartSize = vCellSizes[nStart];
    vCellSizes[nStart] = k - nStart;
    vCellSizes[k] = nStartSize - vCellSizes[nStart];

    // record this split
    vNewIndices.push_back(make_pair(nStart, k) );

    // increase the number of splits at this level
    vNewIndicesAtLevel.back() += 1;

    // updates the vNontrivialList structure
    insert_index(nStart, k, nStartSize);
}

void PartitionNest::insert_index(int nStart, int k, int nStartSize)
{
    pair<int, int> &pk = vNontrivialList[k];
    pair<int, int> &pnStart = vNontrivialList[nStart];
    pair<int, int> &pnNext = vNontrivialList[pnStart.second];
    pair<int, int> &pnPrev = vNontrivialList[pnStart.first];

    // case 1:
    // if k isn't nStart + 1 and k isn't at the end
    // (it's < nStart + nStartSize - 1)
    if (k > nStart + 1 && k < nStart + nStartSize - 1)
    {
        // join k and nStart's next
        pk.second = pnStart.second;
        pnNext.first = k;

        // join nStart and k
        pnStart.second = k;
        pk.first = nStart;
    }
    // case 2:
    // k is nStart + 1, and k is not at the end of the cell
    else if (k == nStart + 1 && k < nStart + nStartSize - 1)
    {
        // k takes the place of nStart in the list
        pk = pnStart;
        pnPrev.second = k;
        pnNext.first = k;
    }
    // case 3:
    // k is nStart + 1, and k is at the end of the cell (nSizetSize == 2)
    else if (k == nStart + 1 && k == nStart + nStartSize - 1)
    {
        // neither nStart nor k will be non-trivial after this
        // bypass them altogether
        pnPrev.second = pnStart.second;
        pnNext.first = pnStart.first;
    }
    // case 4:
    // k > nStart + 1, and k == nStart + nStartSize - 1
    // do nothing!!!
}


int PartitionNest::advance_level()
{
    vNewIndicesAtLevel.push_back(0);
}

int PartitionNest::recover_level(int m)
{
    int nSplits = 0;
    set<int> cStartIndices;
    vector<int> vCellEnd;
    vector<int> vOriginalSizes;
    int k = 0;
    int k_size = 0;
    int k_temp = 0;
    int nStart = 0;
    int nStartSize = 0;
    int nEnd = 0;
    int i = 0;

    // the cell ends are 0
    vCellEnd.resize(size() );
    vNewIndexQueue.clear();

    while (level() > m)
    {
        nSplits += vNewIndicesAtLevel.back();
        vNewIndicesAtLevel.pop_back();
    }

    for (i = 0; i < nSplits; i++)
    {
        nStart = vNewIndices.back().first;
        k = vNewIndices.back().second;
        vNewIndices.pop_back();

        // record the sizes
        k_size = vCellSizes[k];
        nStartSize = vCellSizes[nStart];

        // update the nontrivial linked list
        erase_index(nStart, k, nStartSize, k_size);

        // update the sizes (k is removed, nStart is enlarged)
        vCellSizes[k] = 0;
        vCellSizes[nStart] += k_size;

        // manage keeping track of where nStart ends
        nEnd = vCellEnd[k];

        // if k has no end yet, end where k ended
        if (nEnd == 0)
        {
            nEnd = k + k_size - 1;
        }
        else  // otherwise k isn't a start anymore
        {
            vCellEnd[k] = 0;
            cStartIndices.erase(k);
        }

        cStartIndices.insert(nStart);
        vCellEnd[nStart] = nEnd;
    }

    // run through cStartIndices and update index containing
    set<int>::iterator it;

    for (it = cStartIndices.begin(); it != cStartIndices.end(); it++)
    {
        nStart = (*it);
        nEnd = vCellEnd[nStart];

        for (i = nStart; i <= nEnd; i++)
        {
            vIndexContaining[vElements[i] ] = nStart;
        }
    }

    return level();
}

int PartitionNest::backup_level()
{
    return recover_level(level() - 1);
}

bool PartitionNest::breakout(int u)
{
    int k = 0;
    int *anElements = elements();

    k = index_containing(u);

    // don't do anything if there's no need to
    if (cell_size(k) <= 1)
    {
        fprintf(stderr, "Error Error Examine: %s %d %s %d: %s\n",
                "attempting to breakout",
                u,
                "from cell of size",
                cell_size(k),
                str().c_str());
        exit(1);
    }

    int i = k;

    for (; i < k + cell_size(k); i++)
    {
        if (anElements[i] == u)
        {
            break;
        }
    }

    // u should always be found, if not foul play is involved
    if (i == k + cell_size(k) )
    {
        fprintf(stderr, "Error Error Examine: %d %s %d of %s\n",
                u,
                "was not found at index",
                k,
                str().c_str() );
        exit(1);
    }

    std::swap(anElements[k], anElements[i]);

    add_index(k + 1);

    return true;
}


void PartitionNest::erase_index(int nStart, int k, int nStartSize, int k_size)
{
    int nStartPrev = 0;
    int nStartNext = 0;
    int k_next = 0;

    if (nStartSize == 1)
    {
        nStartPrev = vNontrivialList[nStart].first;
        nStartNext = vNontrivialList[nStart].second;
        vNontrivialList[nStartPrev].second = nStart;
        vNontrivialList[nStartNext].first = nStart;
    }
    else if (nStartSize > 1 && k_size > 1)
    {
        k_next = vNontrivialList[k].second;
        vNontrivialList[nStart].second = k_next;
        vNontrivialList[k_next].first = nStart;
    }
}

vector<int> PartitionNest::operator[](int k) const
{
    vector<int> v;
    int i = 0;
    int k_next = 0;

    k_next = next_index(k);

    for (i = k; i < k_next; i++)
    {
        v.push_back(vElements[i]);
    }

    sort(v.begin(), v.end() );

    return v;
}

string PartitionNest::str() const
{
    stringstream ss;
    ss << (*this);
    return ss.str();
}

ostream &operator<< (ostream &out, const PartitionNest &pi)
{
    int k = 0;
    vector<int> v;

    k = 0;
    out << "[";

    // special case for n = 0
    if (pi.size() > 0)
    {
        out << " ";
    }

    while (k < pi.terminal_index() )
    {
        v = pi[k];
        out << range_compress(v);
        k = pi.next_index(k);

        if (k != pi.terminal_index() )
        {
            out << " | ";
        }
    }

    out << " ]";

    return out;
}

void process_token(string sToken, vector<int> &vElements,
        vector<int> &vIndices, set<int> &cElementSet)
{
    stringstream ss(sToken);

    if (sToken == "|")
    {
        // check that that we're not adding index 0, which is always there
        if (vElements.size() == 0)
        {
            fprintf(stderr,
                    "Error Error Examine: cannot add 0 as an index\n");
            exit(1);
        }

        if (vIndices.size() > 0 && vIndices.back() == vElements.size() )
        {
            fprintf(stderr,
                "%s: cannot add an index twice (| | occurred)\n",
                "Error Error Examine");
            exit(1);
        }

        vIndices.push_back(vElements.size() );
    }
    else if (sToken.find(':') != string::npos)
    {
        int nLow = 0;
        int nHigh = 0;
        char c = 0;

        int nFields = sscanf(sToken.c_str(), "%d:%d", &nLow, &nHigh);

        // check that we read two ints,
        // that they're both nonnegative and increasing
        if (nFields < 2 || nLow < 0 || nHigh < 0 || nHigh <= nLow)
        {
            fprintf(stderr,
                    "%s: %s but got %s instead\n",
                    "Error Error Examine",
                    "expected <m>:<n> (increasing, nonnegative)",
                    sToken.c_str() );
            exit(1);
        }

        for (int i = nLow; i <= nHigh; i++)
        {
            vElements.push_back(i);
            cElementSet.insert(i);
        }
    }
    else  // otherwise we should be reading in an integer
    {
        int m = 0;

        int nFields = sscanf(sToken.c_str(), "%d", &m);

        if (nFields == 0 || m < 0)
        {
            fprintf(stderr,
                    "%s: expected a nonnegative integer, got %d instead\n",
                    "Error Error Examine",
                    m);
            exit(1);
        }

        vElements.push_back(m);
        cElementSet.insert(m);
    }
}

// read in a partition like [ 1 | 0:3 | 4 5 ]
istream &operator>> (istream &in, PartitionNest &pi)
{
    char c = 0;

    // read in the [
    in >> c;

    if (in.fail() )
    {
        fprintf(stderr,
                "Error Error Examine: couldn't read character from stream\n");
        exit(1);
    }

    if (c != '[')
    {
        fprintf(stderr,
                "Error Error Examine: expected [ but got %c instead\n", c);
        exit(1);
    }

    string sToken = "[";
    vector<int> vIndices;
    vector<int> vElements;
    set<int> cElementSet;  // to check that the elements are 0 ... n - 1
    int nIndex = 0;

    in >> sToken;

    // process the stream one token at a time until ] is found
    while (sToken != "]")
    {
        process_token(sToken, vElements, vIndices, cElementSet);
        in >> sToken;

        if (in.fail() )
        {
            fprintf(stderr,
                    "Error Error Examine: error reading token\n");
            exit(1);
        }
    }

    // check that the elements are 0 ... n - 1
    for (int i = 0; i < vElements.size(); i++)
    {
        if (cElementSet.find(i) == cElementSet.end() )
        {
            fprintf(stderr,
                    "%s: %s {0, ..., %d} but %d is missing\n",
                    "Error Error Examine",
                    "element set should be",
                    vElements.size() - 1, i);
            exit(1);
        }
    }

    // check that n - 1 is not an index
    if (vIndices.size() > 0 && vIndices.back() == vElements.size() )
    {
        fprintf(stderr,
                "%s: n == %d cannot be an index\n",
                "Error Error Examine",
                vElements.size() - 1, vElements.size() );
        exit(1);
    }

    // now set pi
    pi.unit(vElements.size() );

    for (int i = 0; i < vElements.size(); i++)
    {
        pi.elements()[i] = vElements[i];
    }

    for (int i = 0; i < vIndices.size(); i++)
    {
        pi.enqueue_new_index(vIndices[i]);
    }

    pi.commit_pending_indices();
}

}  // namespace nishe
