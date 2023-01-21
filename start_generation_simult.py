#from dataset_generator import DatasetGenerator
import glob
import os
import subprocess
import random
import sys
import numpy as np
import time
from datetime import datetime
import json
import threading
import multiprocessing
import psutil
from dataset_generator_simult import DatasetGenerator
import pandas as pd

#boundaries = {
#    "scp42": [1205., 37132.],
#    "scp47": [1115., 36570.],
#    "scp55": [550., 38972.],
#    "scpa2": [560., 58550.],
#    "scpb5": [151.,59676.],
#    "tai30a": [1706855., 8596620.],
#    "tai35a": [2216627., 11803330.],
#    "tai40a": [2843274., 15469120.],
#    "tai50a": [4390920., 24275700.],
#    "tai100a": [15844731., 97714200.],
#    "tai100b": [1058131796., 130115042000.],
#}

gensetup_file_path = ""
max_simultaneos_processes = 1
num_executions_per_instance = 10
num_iterations = 10000

#print(sys.argv)


if len(sys.argv) > 1:
    max_simultaneos_processes = int(sys.argv[1])

if len(sys.argv) > 2:
    gensetup_file_path = sys.argv[2]

#scp_boundaries = pd.read_csv("scp_boundaries.csv", sep = ",", names = ["instance", "lower", "upper"])
#qap_boundaries = pd.read_csv("qap_boundaries.csv", sep = ",", names = ["instance", "lower", "upper"])
#boundaries = pd.concat([scp_boundaries, qap_boundaries], ignore_index=True)

instances = []

if gensetup_file_path == "":
    instances = [
        ["pmed1.txt", num_executions_per_instance, num_iterations, 0.4, "best_improvement"],
    ]
else:
    print("Retrieving definitions from", gensetup_file_path)
    gensetup = pd.read_csv(gensetup_file_path, ",", header=None)
    #print(gensetup)

    for index, row in gensetup.iterrows():
        gensetup = row.values.tolist()
        instances.append(gensetup)
print(max_simultaneos_processes, instances)
#exit(0)        

#for index, row in boundaries.iterrows():
#    instances.append([
#    	(row["instance"] + ".dat") if not row["instance"].startswith("scp") else row["instance"], 
#    	num_executions_per_instance, 
#    	num_iterations, 
#    	[row["lower"], row["upper"]], 
#    	[]
#	])

order = 0
task_queue = []

print(instances)

for instance in instances:
    #print("Running instance: ", instance)
    #a_dataset_generator = DatasetGenerator(instance[1], instance[0], instance[2], instance[4], instance[3], 4, True)
    #print(instance[0], instance[1], instance[2], instance[3], instance[4], time.time())
    #a_dataset_generator.start_dataset_generator()

    for i in range(instance[1]):
        seed = str(random.randint(1000, 100000))
        #print("SEED", seed);
        #if i < len(instance[4]):
        #    seed = str(instance[4][i])
        #print("SEED", seed);

        task_queue.append([order, instance[0], instance[2], [0, 0], seed, instance[3], instance[4]])

        order += 1


a_dataset_generator = DatasetGenerator(task_queue, max_simultaneos_processes, True)
a_dataset_generator.start_dataset_generator()