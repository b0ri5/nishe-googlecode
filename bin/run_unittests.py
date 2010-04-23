#!/usr/bin/env python

import os
import optparse

def main():
  parser = optparse.OptionParser()
  
  parser.add_option('-d', '--debug', dest='debug', action="store_true",
          help='run the debug tests',
          default=True)
  
  parser.add_option('-r', '--release', dest='release', action="store_true",
          help='run the release tests',
          default=False)
  
  options, args = parser.parse_args()
  
  bindir = os.path.dirname(__file__)
  
  # run the tests ending in "-db"
  if options.debug:
    for f in os.listdir(os.path.join(bindir, 'test') ):
      if f.endswith('-db'):
        os.system(os.path.join(bindir, 'test', f) )
  
  # run the tests not ending with "-db"
  if options.release:
    for f in os.listdir(os.path.join(bindir, 'test') ):
      if not f.endswith('-db'):
        os.system(os.path.join(bindir, 'test', f) )
      
if __name__ == "__main__":
  main()