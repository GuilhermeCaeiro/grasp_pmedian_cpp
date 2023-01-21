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


class DatasetGenerator:
    
    def __init__(self, tasks_queue, max_processes = 1, verbose = False):
        self.tasks_queue = tasks_queue
        self.num_datasets_to_generate = len(tasks_queue)
        self.num_datasets_generated = 0
        self.num_datasets_being_generated = 0
        #self.set_name = set_name
        #self.num_iterations = str(int(num_iterations))
        #self.bounds = bounds
        #self.seeds = seeds
        self.max_processes = max_processes
        self.verbose = verbose
        
        self.available_processes = {}
        self.lock_flag = False

        self.log("Number of executions to be generated: %d." % self.num_datasets_generated)
        
        self.generate_available_processes_dict()
        
    def generate_available_processes_dict(self):
        for i in range(self.max_processes):
            self.available_processes[i] = [False, None, None] # [Used?, Process Manager, Process]
        
    # returns available thread id
    def get_available_process(self):
        for i in range(len(self.available_processes)):
            if self.available_processes[i][0] == False:
                return i
        return -1
    
    def check_avg_cpu_load(self):
        usage_episodes = []
        
        for i in range(0, 10):
            usage_episodes.append(psutil.cpu_percent())
            time.sleep(1)
            
        return np.mean(usage_episodes)
    
    def job_starter_loop(self):
        while self.num_datasets_generated < self.num_datasets_to_generate:
            available_process_id = self.get_available_process()
            
            if available_process_id != -1 and self.num_datasets_being_generated < self.num_datasets_to_generate:
                #cpu_load = self.check_avg_cpu_load()
                #self.log("Process available with ID %d." % (available_process_id))
                #self.log("CPU load: %d %%" % (cpu_load))
            
                #if cpu_load < 85.0:
                # starts a new process to generate a dateset
                self.log("Starting a new process.")
                self.num_datasets_being_generated += 1
                self.start_thread(self.start_dataset_generation, [available_process_id, self.tasks_queue.pop(0)])

                #else:
                #    self.log("CPU load too high (%.2f%%). Trying again later..." % (cpu_load))
                    
            else:
                #self.log("No process ID available or total limit of datasets being generated achieved.")
                pass
            
            time.sleep(5)
    
    def start_dataset_generation(self, process_id, task):
        self.log("Starting dataset generation on process with ID %d. Task order number is %d" % (process_id, task[0]))
        self.log("Starting dataset generation on process with ID %d. Task order number is %d" % (process_id, task[0]), str(process_id))
        results_dict = multiprocessing.Manager().dict()
        set_name = task[1]
        num_iterations = task[2]
        bounds = task[3]
        seed = task[4]
        rcl_size = task[5]
        ls_mode = task[6]
        process = multiprocessing.Process(target = self.generate_dataset, args = (set_name, num_iterations, bounds, seed, rcl_size, ls_mode, results_dict, process_id))
        process.daemon = True
        process.start()
        
        self.cautiously_activate_lock()                                                           # Locks access to class attributes
        self.available_processes[process_id][0] = True                                            # Reserves the ID
        self.available_processes[process_id][1] = results_dict                                    # Unused from the dict
        self.available_processes[process_id][2] = process                                         # Unused from the dict
        self.deactivate_lock()                                                                    # Unlocks access to class attributes
        
        self.log("Joining process with ID %d." % (process_id))
        self.log("Joining process with ID %d." % (process_id), str(process_id))
        process.join()
        
        self.log("Joined process with ID %d." % (process_id))
        self.log("Joined process with ID %d." % (process_id), str(process_id))
        
        seed = results_dict["seed"]
        
        self.log("Generated dataset for SET %s with SEED %s through process with ID %d." % (set_name, seed, process_id))
        self.log("Generated dataset for SET %s with SEED %s through process with ID %d." % (set_name, seed, process_id), str(process_id))
        
        self.log("Liberating process with ID %d." % (process_id))
        self.log("Liberating process with ID %d." % (process_id), str(process_id))
        self.cautiously_activate_lock()
        self.available_processes[process_id][0] = False
        self.available_processes[process_id][1] = None
        self.available_processes[process_id][2] = None
        self.num_datasets_generated += 1                                                          # Updates the number of datasets generated
        self.log("Dataset %d generated." % (self.num_datasets_generated))
        self.deactivate_lock()
        
    #def generator_process(self, set_name, results_dict, process_id):
    #    thread.sleep(40)
    #    results_dict["seed"] = 12345
    
    def generate_dataset(self, set_name, num_iterations, bounds, seed, rcl_size, ls_mode, results_dict, process_id):
        self.log("Starting dataset generation.", str(process_id))
        instance = set_name
        seed = str(random.randint(1000, 100000)) if seed is None else seed
        
        command = "python run_grasp.py %s %s %s %s %s" % (instance, seed, rcl_size, num_iterations, ls_mode)

        self.log("Executted command: %s" % (command), str(process_id))
        os.system(command)
        
        results_dict["seed"] = seed
        self.log("Finished dataset generation.", str(process_id))
    
    def start_thread(self, a_method, args = ()):
        thread = threading.Thread(target = a_method, args = args)
        thread.daemon = True
        thread.start()
        
    def cautiously_activate_lock(self):
        while True:
            if self.lock_flag == True:
                time.sleep(0.005)
                continue
            else:
                self.lock_flag = True
                break
            
    def deactivate_lock(self):
        self.lock_flag = False
        
    def start_dataset_generator(self):
        self.log("Starting main loop.")
        self.job_starter_loop()
        
    def log(self, message, process_id = "main"):
        message = "%s: %s" % (str(datetime.now()), message)
        if self.verbose:
            print(message)
        # FIXME may cause problems if multiple processes try to write at the same time.
        of = open("output_dataset_generator_%s.txt" % (process_id), "a+")
        of.write(message + "\n")
        of.close()
        
        
#dataset_generator = DatasetGenerator(20, "scpa2", 2000000, 5, True)
#dataset_generator.start_dataset_generator()
