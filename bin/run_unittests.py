#!/usr/bin/env python

import os
import optparse
import sys

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
  testdir = os.path.join(bindir, 'test')
  
  if not os.path.isdir(testdir):
    print 'bin/test is missing, is gtest configured properly?'
    sys.exit(1)      
  
  # run the tests ending in "-db"
  if options.debug:
    for f in os.listdir(os.path.join(bindir, 'test') ):
      name, ext = os.path.splitext(f)
      if name.endswith('-db'):
        os.system(os.path.join(bindir, 'test', f) )
  
  # run the tests not ending with "-db"
  if options.release:
    for f in os.listdir(os.path.join(bindir, 'test') ):
      if not f.endswith('-db'):
        os.system(os.path.join(bindir, 'test', f) )
      
if __name__ == "__main__":
  main()