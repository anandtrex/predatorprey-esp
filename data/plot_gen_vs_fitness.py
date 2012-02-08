#!/usr/bin/python2

import sys
import matplotlib.pyplot as plt
import string

if __name__ == '__main__':
        base = []
        others = []
        # For each file that is given in the input
        for i in xrange(1, len(sys.argv)):
                fileName = sys.argv[i]
                
                
                f = open(fileName, 'r')
                lines = string.split(f.read(), "\n")
                
                if "monolithic" in fileName:
                        plot_no = "monolithic"
                        base = lines
                else:
                        plot_no = fileName.split('-')[len(fileName.split('-')) - 1].split('.')[0]
                        lines.append(plot_no)
                        others.append(lines)
                        
                generations = []
                fitnesses = []
                for i in xrange(0,100):
                        l = lines[i]
                        if l == "":
                                continue
                        l = l.strip()
                        pair = l.split(" ")
                        generations.append(pair[0])
                        fitnesses.append(pair[1])
                plt.plot(generations, fitnesses, label=plot_no)
                
        plt.xlabel('generations')
        plt.ylabel('average fitness')
        plt.legend(loc='lower right')

        plt.savefig("fitness-vs-generations.png")
        plt.show()
        plt.close()
        
        if "prop" in fileName:
                sys.exit(0)
                
        for o in others:
                per_improvement = [(float(base[i].split(" ")[1]) - float(o[i].split(" ")[1])) * 100 /float(base[i].split(" ")[1]) for i in xrange(0,100)]
                plt.plot(xrange(0,100), per_improvement, label=o[101])
        plt.xlabel('generations')
        plt.ylabel('Percentage improvement over monolithic network')
        plt.legend(loc='upper left')

        plt.savefig("improvement-vs-generations.png")
        plt.show()
        plt.close()
            
