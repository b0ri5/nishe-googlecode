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

	f_directed = open('directed-1-%d.txt' % (n), 'w')

	for G in bgraph.gen.nonisomorphic_graphs(range(1, n + 1) ):
		for H in G.directed_isomorphs():
			bgraph.io.save_file(H, f_directed)
			f_directed.write(os.linesep)

	f_directed.close()

if __name__ == "__main__":
	main()
