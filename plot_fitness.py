#!/usr/bin/python2

import sys, string
import matplotlib.pyplot as plt

if __name__ == "__main__":
    a_num_hunters = [1, 2, 3, 4, 5]
    a_ex_types = ["selection", "combiner", "monolithic"]
    num_generations = 300
    # coev = false

    for num_hunters in a_num_hunters:
        plt.figure()
        for ex_type in a_ex_types:
            dir = "exec_h_" + str(num_hunters) + "_" + ex_type[0]
            label = ex_type + " network"
            fmax = open(dir + "/max_fitness.log")
            lines = string.split(fmax.read(), "\n")
            if ex_type != "monolithic":
                lines = lines[-num_generations:]
                
            generations = [string.split(line, " ")[0] for line in lines if line.strip() !=""]
            fitness = [string.split(line, " ")[1] for line in lines if line.strip() !=""]
            plt.plot(generations, fitness, label = label)
        plt.title("Fitness with "+str(num_hunters)+" hunters")
        plt.xlabel('generations')
        plt.ylabel('maximum fitness')
        plt.legend(loc = 'best')
        plt.savefig("fitness_h_"+str(num_hunters)+".png")
        plt.close()
