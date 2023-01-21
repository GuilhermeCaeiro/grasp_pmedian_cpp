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

max_simultaneos_processes = 2
num_executions_per_instance = 10
num_iterations = 10000

#scp_boundaries = pd.read_csv("scp_boundaries.csv", sep = ",", names = ["instance", "lower", "upper"])
#qap_boundaries = pd.read_csv("qap_boundaries.csv", sep = ",", names = ["instance", "lower", "upper"])
#boundaries = pd.concat([scp_boundaries, qap_boundaries], ignore_index=True)

instances = [
    #["tai30a.dat", 1, 1000000, [], [1706855, 8596620]], # inst, num exec, num it/exec, seeds, bounds
    #["tai35a.dat", 1, 1000000, [], [2216627, 11803330]],
    #["tai40a.dat", 1, 1000000, [], [2843274, 15469120]],
    #["tai50a.dat", 1, 1000000, [], [4390920, 24275700]],
    #["tai50b.dat", 1, 1000000, [], [395543467, 30319229200]],
    #["tai100a.dat", 1, 1000000, [], [15844731, 97714200]],
    #["tai100b.dat", 1, 1000000, [], [1058131796, 130115042000]],
    #["tai150b.dat", 1, 1000000, [], [441786736, 26078235600]],
    #["sko42.dat", 1, 1000000, [], [14934, 168840]],
    #["sko56.dat", 1, 1000000, [], [32610, 358400]],
    #["sko64.dat", 1, 1000000, [], [45736, 504320]],
    #["sko100a.dat", 1, 1000000, [], [143846, 1584000]],
    #["sko100b.dat", 1, 1000000, [], [145522, 1584000]],
    #["tho40.dat", 1, 1000000, [], [224414, 3730400]],
    #["wil50a.dat", 1, 1000000, [], [47098, 258300]],

    #["tai30a.dat", 20, 1000000, [3020, 11080, 20195, 21055, 23706, 32898, 33342, 38358, 40743, 59290], [1706855, 8596620]]#, 71248, 78526, 79514, 80705, 82977, 89929, 93232, 93982, 97344, 98457], [1706855, 8596620]]
    
    #["tai35a.dat", 6, 2000, [boundaries["tai35a"]["lower"], boundaries["tai35a"]["upper"]], []]
    #["tai40a.dat", 6, 2000, [2843274, 15469120], []],

    #["scp41", 10],
    #["scp44", 10],
    #["scp45", 10],
    #["scp510", 10],
    #["scp64", 10],
    #["scpa1", 10],
    #["scpa4", 10],
    #["scpb2", 10],
    #["scpb3", 10],
    #["scpb4", 10],
    #["scpc1", 10],
    #["scpc3", 10],
    #["scpc5", 10],
    #["scpd1", 10],
    #["scpd5", 10],

    #["tai30a.dat", 1, 10000, [1706855, 8596620], []],
    #["scp47", 1, 10000, [1115, 36570], []],

    ["pmed1.txt", num_executions_per_instance, num_iterations, 0.4, "best_improvement"],

]

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