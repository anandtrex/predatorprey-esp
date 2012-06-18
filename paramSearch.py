#!/usr/bin/python2

from time import strftime, localtime, time
import os, sys, stat, subprocess
from shutil import copy

def generateConfig(dir, size, num_eval_trials = -1, num_generations = -1, coevolve = "false", \
                   num_hunters = 3, num_hidden_neurons = -1, population_size = -1):
    if(size == "big"):
        max_steps = 100;
        width = 10
        height = 10
        if(num_generations == -1):
            num_generations = 300
        if(population_size == -1):
            population_size = 100
        if(num_hidden_neurons == -1):
            num_hidden_neurons = 10
        if(num_eval_trials == -1):
            num_eval_trials = 10
            
    elif(size == "small"):
        max_steps = 50;
        width = 5
        height = 5
        if(num_generations == -1):
            num_generations = 5
        if(population_size == -1):
            population_size = 50
        if(num_hidden_neurons == -1):
            num_hidden_neurons = 5
        if(num_eval_trials == -1):
            num_eval_trials = 5


    fout = open(dir+"/esp_predprey.cfg", 'w')
    cfgContent = "version = \"1.0\";\n\n\
# Autogenerated by paramSearch.py at "+strftime("%a, %d %b %Y %H:%M:%S", localtime())+"\n\
# NOTE: Multiple words are always separated by an '_' and not camel cased\n\n\
experiment:\n\
{\n\
    max_steps = " + str(max_steps) + ";\n\
    grid:\n\
    {\n\
        width = " + str(width) + ";\n\
        height = " + str(height) + ";\n\
    }\n\
    \n\
    esp:\n\
    {\n\
        num_generations = " + str(num_generations) + ";\n\
        population_size = " + str(population_size) + ";\n\
        num_hidden_neurons = " + str(num_hidden_neurons) + ";\n\
        num_eval_trials = " + str(num_eval_trials) + ";\n\
        net_type = 0; # 0 - FF\n\
        coevolve = " + coevolve + ";\n\
    }\n\
}\n\
\n\
agents:\n\
{\n\
    predator:\n\
    {\n\
        number = 1;\n\
        predators = (\n\
                        {\n\
                            move_probability = 1.0; # Equivalent to speed\n\
                            type = 1; # 0 - random, 1 - naive, 2 - esp, 3 - neat?\n\
                        }\n\
                    );\n\
    }\n\
    prey:\n\
    {\n\
        number = 1;\n\
        preys = (\n\
                    {\n\
                        move_probability = 0.9; # Equivalent to speed\n\
                    }\n\
                );\n\
    }\n\
    \n\
    hunter:\n\
    {\n\
        number = " + str(num_hunters) + ";\n\
        hunters =    (\n\
                        {\n\
                            move_probability = 0.9; # Equivalent to speed\n\
                            role_reversal_probability = 0.3;\n\
                            type = 0; # 0 - normal hunter, 1 - hunter with MPD\n\
                        }\n\
                    );\n\
    }\n\
}\
    "
    fout.write(cfgContent)
    fout.close()
    
def generateRunFile(dir, executable_path, experimentType):
    if experimentType == "selection":
        flag = "-i"
    elif experimentType == "combiner":
        flag = "-s"
    elif experimentType == "monolithic":
        flag = "-e"
    else:
        print "Invalid experiment type. Exiting."
        sys.exit()
         
    fout = open(dir+"/run.sh",'w')
    runContent = "#!/bin/bash\n\
# Autogenerated by paramSearch.py at "+strftime("%a, %d %b %Y %H:%M:%S", localtime())+"\n\
export GLOG_minloglevel=1\n\
export GLOG_logtostderr=1\n\
export GLOG_v=0 # 4 is verbose debug log, 2 gives position updates every step, 5 is unnecessary debugging messages\n\
export LD_LIBRARY_PATH=\""+executable_path+"/glog/lib\":\""+executable_path+"/libconfig/lib\":\""+executable_path+"/gflags/lib\":$LD_LIBRARY_PATH\n\
./esp-predprey -c esp_predprey.cfg "+flag

    fout.write(runContent)
    fout.close()
    os.chmod(dir+"/run.sh", stat.S_IRWXU | stat.S_IRGRP | stat.S_IROTH)
    
def generateCondorFile(dir):
    fout = open(dir+"/condor_run",'w')
    condorContent = "# Autogenerated by paramSearch.py at "+strftime("%a, %d %b %Y %H:%M:%S", localtime())+"\n\
Getenv = True\n\
Requirements = Lucid \n\
Executable     = run.sh \n\
Log = condor."+str(int(time()))+".log\n\
Output  = output."+str(int(time()))+".log\n\
Error = error."+str(int(time()))+".log\n\
+Group = \"GRAD\" \n\
+Project = \"AI_Neural_Networks\" \n\
+ProjectDescription = \"Prey-Predator Simulation\" \n\
Queue "
    fout.write(condorContent)
    fout.close()
    
'''
Main method -- program entry point
'''    
if __name__ == '__main__':
  if len(sys.argv) > 1:
    executable_path = sys.argv[1]
  else:
    executable_path = "."
    
  executable_path = os.path.realpath(executable_path)

  a_num_hunters = [3,4]
  a_ex_types = ["selection", "combiner", "monolithic"]
  
  for num_hunters in a_num_hunters:
      for ex_type in a_ex_types:
        dirname = "exec_h_"+str(num_hunters)+"_"+ex_type[0]
        if not os.path.exists(dirname):
            os.makedirs(dirname)
        generateConfig(dirname, "small" ,num_hunters=num_hunters)
        generateRunFile(dirname, executable_path, ex_type)
        generateCondorFile(dirname)
        copy(executable_path+"/esp-predprey", dirname)
        '''
        os.chdir(dirname)
        print "Running " + dirname + "/run.sh"
        subprocess.call(["./run.sh"])
        os.chdir("..")
        '''