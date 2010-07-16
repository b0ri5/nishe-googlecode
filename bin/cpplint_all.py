#!/usr/bin/env python

import sys
import os
import subprocess as sp

src_dirs = [os.path.join('include', 'nishe'), 'src', 'test']

def main():
  bindir = os.path.dirname(__file__)
  cpplint = os.path.join(bindir, 'cpplint.py')
  root = os.path.split(bindir)[0]
  
  for src_dir in src_dirs:
    
    for src_file in os.listdir(os.path.join(root, src_dir) ):
      # only process C++ files and ignore the virtual warning
      if os.path.splitext(src_file)[1] in ['.cc', '.h']:
        cmd = [sys.executable, cpplint]
        filter_flags = ['-readability/streams']
        
        if src_file.startswith('GraphIO'):
          filter_flags.append('-runtime/references')
          
        if src_file.startswith('PartitionNest_unittest.cc'):
          filter_flags.append('-runtime/threadsafe_fn')
        
        cmd.append('--filter=%s' % ','.join(filter_flags))
        cmd.append(os.path.join(root, src_dir, src_file))

        p = sp.Popen(cmd, stdout=sp.PIPE, stderr=sp.PIPE)
        out, err = p.communicate()
        
        if not err.strip().endswith('Total errors found: 0'):
          print err
  
  print 'done'

if __name__ == "__main__":
  main()