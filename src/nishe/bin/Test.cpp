#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>

#include <nishe/PartitionNest.h>
#include <nishe/Graphs.h>
#include <nishe/Util.h>

using namespace std;


void graphs(int argc, char **argv)
{
	BasicGraph g;
	IntegerWeightedGraph weighted_graph;
	IntegerWeightedGraph::nbhr_sum nbhr_sum;
	IntegerWeightedGraph::nbhr_sum a;

	cout << typeid(weighted_graph).name() << endl;
	cout << typeid(nbhr_sum).name() << endl;

	nbhr_sum < a;
}

void sb(int argc, char **argv)
{
    PartitionNest pi;
    stringstream ss;

    ss.str("[ 0 | |");
    ss >> pi;
}

void util(int argc, char **argv)
{
    vector<string> v = split("[ 0 | 1 2 | 3 4 ]", "|");

    for (int i = 0; i < v.size(); i++)
    {
        cout << '"' << v[i] << '"' << endl;
    }

    vector<vector<int> > cells = string2cells("[ 0 | 2 3 | 1 ]");

    cout << cells << endl;

    cells = string2cells("[ 0:3 | 4:6 | 7 ]");
    cout << cells << endl;
}

void partitions(int argc, char **argv)
{
	PartitionNest pi;
	int n = 0;
	int i = 0;

	n = atoi(argv[1]);

	pi.unit(n);
	cout << pi.length() << endl;

	cout << pi << endl;

	for (i = 0; i < n - 1; i++)
	{
		pi.advance_level();
		pi.breakout(n - 1 - i);
		cout << pi << endl;
	}

	while (pi.level() > 0)
	{
		pi.backup_level();
		cout << pi << endl;
	}
}

/*
 * This is the testbed for the components of Nishe, making sure basic functions work.
 */
int main(int argc, char **argv)
{
	map<string, void (*)(int, char **)> test_lookup;
	map<string, void (*)(int, char **)>::iterator it;
	int i = 0;

	test_lookup["partitions"] = partitions;
	test_lookup["graphs"] = graphs;
	test_lookup["sb"] = sb;
	test_lookup["util"] = util;

	if (argc < 2)
	{
		cout << "usage: ./Test <test name> <args>" << endl;
		cout << "valid <test name>s are:" << endl;

		for (it = test_lookup.begin(); it != test_lookup.end(); it++)
		{
			cout << "\t" << (*it).first << endl;
		}

		exit(1);
	}

	for (it = test_lookup.begin(); it != test_lookup.end(); it++)
	{
		string name = (*it).first;

		if (name.find(argv[1]) == 0)
		{
			cout << "calling " << name << "(";

			for (i = 2; i < argc; i++)
			{
				cout << argv[i];

				if (i < argc - 1)
				{
					cout << ", ";
				}
			}

			cout << ")" << endl;

			test_lookup[name](argc - 1, argv + 1);

			return 0;
		}
	}

	cout << argv[1] << " not found" << endl;

	return 0;
}
