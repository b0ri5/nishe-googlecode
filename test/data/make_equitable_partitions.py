#!/usr/bin/env sage-python

import os
import sys
import string
import math
import re

import bgraph
import bnishe

def main():
	filename = sys.argv[1]
	
	base, ext = os.path.splitext(filename)
	filename_equitable = '%s_equitable%s' % (base, ext)

	f = open(filename)
	f_equitable = open(filename_equitable, 'w')
	G = bgraph.io.load_file(f)
	
	while G.vertex_count() > 0:
		refiner = bnishe.refiner()
		pi = refiner.refine(G)
		f_equitable.write(str(pi) + "\n\n")
		
		G = bgraph.io.load_file(f)

	f.close()
	f_equitable.close()
	
if __name__ == "__main__":
	main()