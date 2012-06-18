#!/usr/bin/python3

import sys

if __name__ == '__main__':

  delta = 5
  steps = 3

  if len(sys.argv) != 2:
    print("Error")
    sys.exit()

  fileName = sys.argv[1]
  fin = open(fileName, 'r')
  lines = str.split(fin.read(), "\n")
  fin.close()
  
  fout = open(fileName+".new", 'w')
  
  print("There are",len(lines),"lines in the file")
  
  k = 1
  for i in range(delta * (steps - 1), len(lines), delta * steps):
    for j in range(0,delta):
      fout.write(" ".join([str(k), str.split(lines[i + j])[1]])+"\n")
      k+=1
    
  fout.close()
