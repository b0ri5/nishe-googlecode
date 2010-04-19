#!/usr/bin/env sage-python

import os
import sys
import string
import math
import re

import bgraph

def main():
	n = 5

	if len(sys.argv) > 1:
		n = int(sys.argv[1])

	f_undirected = open('undirected-1-%d.txt' % (n), 'w')

	for G in bgraph.gen.nonisomorphic_graphs(range(1, n + 1) ):
		bgraph.io.save_file(G, f_undirected)
		f_undirected.write(os.linesep)

	f_undirected.close()

if __name__ == "__main__":
	main()
