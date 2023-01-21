import random
import os
import sys

#random.seed(3356)

#seeds_list = []
#number_of_samplings = 1
instance = sys.argv[1]
seed = sys.argv[2] #random.randint(1000, 100000)
rcl_size = sys.argv[3]
num_iterations = sys.argv[4]
ls_mode = sys.argv[5]

command = ""

#command = "./qsolver %s 1 %s %s" % (instance, num_iterations, seed)
if instance.lower().startswith("pmed"):
	command = "./pmedian %s %s %s %s %s" % (instance, seed, rcl_size, num_iterations, ls_mode)


print(command)
print(instance, seed, rcl_size, num_iterations, ls_mode)

# comppiling grasp
#os.system("gcc -o grasp parte2-pkcc-arq_EPS1_EPS2.c tool.c -lm")

# running samplings

#
file_name = instance + "_" + str(seed) + "_datasetfull"
extension = ".csv"

## writing output file header
file = open(file_name + extension, "w")
file.write("seed,instance,rcl_size,iteration,cost,best_cost,best,iterations_since_last_min, iterations_since_best_cost, total_single_mins_found,time_iteration,time\n")
file.close()

os.system(command)
os.system("zip %s.zip %s%s" % (file_name, file_name, extension))
os.system("rm %s%s" % (file_name, extension))