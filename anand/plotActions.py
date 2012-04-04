#!/usr/bin/python2

import sys, string, random, math
import pylab as P

if __name__ == "__main__":
    if(len(sys.argv) < 1):
        print "File name should be passed as an argument"
    fileName = sys.argv[1]
    fin = open(fileName)
    lines = string.split(fin.read(), "\n")
    llines = []
    i = 0
    for line in lines:
        if line == "":
            continue
        llines.append(string.split(line, " "))
        i+=1

    l = len(llines[1])    
    
    # For prey
    prey_x = []
    prey_y = []
    prey_sel = []
    for i in xrange(1,len(llines)):
        prey_x.append(random.randint(0,9))
        prey_y.append(random.randint(0,9))
        prey_sel.append(random.randint(0,3))
        
    prey_dist = [x**2 + y**2 for x in prey_x for y in prey_y]
    
    d = [[] for x in xrange(0,4)]
    
    for i in xrange(0 ,len(prey_x)):
        d[prey_sel[i]].append(prey_dist[i])

    P.hist(d, xrange(0,100,10), color=['red', 'blue', 'green', 'orange'], label=['chase', 'evade1', 'evade2', 'evade3'])
    P.ylim([-1,5])
    P.title("Prey")
    P.legend(loc = 'best')
    P.xlabel("Distance from predator")
    P.ylabel("Frequency")
    P.show()