#include <iostream>
#include <random>
#include <fstream>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <bits/stdc++.h>
using namespace std;
using namespace std::chrono;

const int INF = 1000000.0;
static mt19937 rng;

class Solution {
    public:
        vector<int> solution;
        double fitness;
        int tmpCandidateLocation; // added just to facilitate some things in the code

        Solution(){

        }

        Solution(vector<int> aSolution, double aFitness){
            solution = aSolution;
            fitness = aFitness;
        }
        
        Solution(vector<int> aSolution, double aFitness, int aTmpCandidateLocation){
            solution = aSolution;
            fitness = aFitness;
            tmpCandidateLocation = aTmpCandidateLocation;
        }

        bool operator< (const Solution &otherSolution) const {
            return fitness < otherSolution.fitness;
        }

        //Solution& operator=(const Solution& otherSolution){
        //    solution = otherSolution.solution;
        //    fitness = otherSolution.fitness;
        //    tmpCandidateLocation = otherSolution.tmpCandidateLocation;
        //    return *this;
        //}
};

class CustomerAllocation {
    public:
        int location;
        double cost;

        CustomerAllocation(int aLocation, double aCost){
            location = aLocation;
            cost = aCost;
        }
};

class Utils {
    public:
        static void printCostsVector(vector<vector<double>> &aVector){
            for (int i = 0; i < aVector.size(); i++){
                for (int j = 0; j < aVector[i].size(); j++){
                    cout << aVector[i][j] << " ";
                }
                cout << endl;
            }
        }

        // function obtained from https://www.geeksforgeeks.org/floyd-warshall-algorithm-dp-16/
        static void floydWarshall(vector<vector<double>> &dist, int V){
            int i, j, k;

            for (k = 0; k < V; k++) {
                for (i = 0; i < V; i++) {
                    for (j = 0; j < V; j++) {
                        if (dist[i][j] > (dist[i][k] + dist[k][j]) && (dist[k][j] != 1000000.0 && dist[i][k] != 1000000.0))
                            dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }

        static int getRandomWithinRange(int start, int end){
            uniform_int_distribution<uint32_t> uint_dist(start, end);
            return uint_dist(rng);
        }

        static void printVectorOfInt(vector<int> &vectorOfInt){
            for (int intValue: vectorOfInt)
                cout << intValue << " ";
        }

        static milliseconds getCurrentTimeInMS(){
            return duration_cast<milliseconds>(system_clock::now().time_since_epoch());
        }

        static int msToNum(milliseconds ms){
            return std::chrono::duration_cast<std::chrono::milliseconds>(ms).count();
        }
};

class Instance {
    public:
        string path;
        int p;
        int n_candidate_locations;
        int n_customer_locations;
        vector<vector<double>> costs;

        Instance(string aPath){
            path = aPath;

            readInstanceFile();
            //test();
        }

        vector<string> splitLine(string line){
            stringstream stream(line);
            vector<string> split_line;
            string part;
            int count = 0;
            
            while (stream >> part) {
                split_line.push_back(part);
                count++;
            }

            //cout << split_line.at(0) << split_line.at(1) << split_line.at(2) << endl;
            return split_line;
        }

        void initializeCosts(int num_lines, int num_columns){
            for (int i = 0; i < num_lines; i++){
                costs.push_back(vector<double> (num_columns, INF));
            }
        }

        void readInstanceFile(){
            ifstream fileReader(path);
            
            string tmp;

            getline(fileReader, tmp);
            
            vector<string> splittedHeader = splitLine(tmp);
            p = stoi(splittedHeader.at(2));
            n_candidate_locations = stoi(splittedHeader.at(0));
            n_customer_locations = stoi(splittedHeader.at(0));
            //costs = new vector<vector<double>>(n_candidate_locations, vector<double> (n_customer_locations, 10000000.0));
            initializeCosts(n_candidate_locations, n_customer_locations);

            while (getline(fileReader, tmp)){
                //cout << tmp << endl;
                //splitLine(tmp);
                vector<string> splittedLine = splitLine(tmp);

                

                int position_a = stoi(splittedLine.at(0));
                int position_b = stoi(splittedLine.at(1));
                int cost = stoi(splittedLine.at(2));

                costs[position_a - 1][position_b - 1] = cost;
                costs[position_b - 1][position_a - 1] = cost;

                //cout << position_a << " " << position_b << " " << cost << endl;
            }

            for (int i = 0; i < n_candidate_locations; i++){
                costs[i][i] = 0.0;
            }

            //Utils::printCostsVector(costs);

            //vector<vector<double>> finalCosts = 
            Utils::floydWarshall(costs, n_candidate_locations);

            //costs = &initialCosts;

            //Utils::printCostsVector(costs);

        }

        void test(){
            cout << "Testing if the contents of costs still exist." << endl;
            Utils::printCostsVector(costs);
        }
};

class GRASP {
    public:
        Instance * instance;
        double rcl_size;
        int maxIterations;
        string localSearchMethod;
        int seed;
        Solution bestSolution;
        vector<CustomerAllocation> customerAllocations;

        GRASP(Instance &aInstance, double aRCL_size, int aMaxIterations, string aLocalSearchMethod, int aSeed = 0){
            instance = &aInstance;
            rcl_size = aRCL_size;
            maxIterations = aMaxIterations;
            localSearchMethod = aLocalSearchMethod;
            seed = aSeed;

            bestSolution.fitness = INF;
            //vector<int> test = {2, 5, 7, 1};
            //fitness(test);
        }

        double fitness(vector<int> &solution){
            int num_columns = instance->n_customer_locations;
            int num_rows = solution.size();
            vector<double> mins(num_columns, 0);

            for (int j = 0; j < num_columns; j++){
                double column_min = 1000000.0;
                for (int i = 0; i < num_rows; i++){
                    int row_number = solution[i];

                    //vector<vector<double>> &costs = *(instance->costs);

                    //Utils::printCostsVector(*costs);

                    //cout << instance->costs << " " << costs[row_number][j] << endl;
                    double value = instance->costs[row_number][j];
                    //double value = 0;
                    if (value < column_min){
                        column_min = value;
                    }
                }

                mins[j] = column_min;
            }

            double objective_value = 0.0;

            for (auto& n : mins)
                objective_value += n;

            double penalty = 100 * fabs(instance->p - solution.size());

            //cout << objective_value << " " << penalty << " " << objective_value + penalty << endl;

            return objective_value + penalty;
        }

        // partcalc -> partial calculation
        double fitness_partcalc(vector<int> &solution, int addedLocation, int removedLocation = -1){
            int num_columns = instance->n_customer_locations;
            int num_rows = solution.size();
            double totalCost = 0.0;

            //cout << "Solution being evaluated: ";
            //Utils::printVectorOfInt(solution);
            //cout << endl;
            //cout << "addedLocation: " << addedLocation << "\nremovedLocation: " << removedLocation << endl; 

            // initializing allocations if this function wasn't run yet.
            if (customerAllocations.size() == 0){
                //cout << "customerAllocations is empty" << endl;
                for (int j = 0; j < num_columns; j++){
                    double column_min = INF;
                    int locationIndex = -1;

                    for (int i = 0; i < num_rows; i++){
                        int row_number = solution[i];

                        double value = instance->costs[row_number][j];
                        if (value < column_min){
                            locationIndex = row_number;
                            column_min = value;
                        }
                    }

                    customerAllocations.push_back(CustomerAllocation(locationIndex, column_min));
                    totalCost += column_min;
                }
            }else{
                for (int j = 0; j < num_columns; j++){
                    if (customerAllocations[j].location == removedLocation){ // customer must be allocated to another location present in the solution
                        double column_min = INF;
                        int locationIndex = -1;

                        for (int i = 0; i < num_rows; i++){
                            int row_number = solution[i];

                            double value = instance->costs[row_number][j];
                            //double value = 0;
                            if (value < column_min){
                                locationIndex = row_number;
                                column_min = value;
                            }
                        }

                        customerAllocations[j].location = locationIndex;
                        customerAllocations[j].cost = column_min;
                        totalCost += column_min;

                        //cout << "A1 " << customerAllocations[j].location << " " << customerAllocations[j].cost << " " << addedLocation << " " << removedLocation << " " << totalCost << endl;
                        //exit(0);
                    }else{ 
                        double costForTheNewLocation = instance->costs[addedLocation][j];
                        if (customerAllocations[j].cost > costForTheNewLocation){ // checks if changing to the new location improves the solution
                            customerAllocations[j].location = addedLocation;
                            customerAllocations[j].cost = costForTheNewLocation;
                        }
                        totalCost += customerAllocations[j].cost;

                        //cout << "B1 " << customerAllocations[j].location << " " << customerAllocations[j].cost << " " << addedLocation << " " << removedLocation << " " << totalCost << endl;
                    }
                }
            }

            //for (int i = 0; i < customerAllocations.size(); i++){
            //    cout << "(" << customerAllocations[i].location << "," << customerAllocations[i].cost << ")-";
            //}

            double penalty = 100 * fabs(instance->p - solution.size());

            //cout << totalCost << " " << penalty << " " << totalCost + penalty << endl;
            //exit(0);

            return totalCost + penalty;
        }

        Solution greedyRandomizedSearch(){
            vector<int> candidateLocations;
            vector<int> partialSolution;
            Solution chosen;

            // initializing candidate locations
            for (int i = 0; i < instance->n_candidate_locations; i++)
                candidateLocations.push_back(i);

            for (int i = 0; i < instance->p; i++){
                vector<Solution> candidateSolutions;
                int previousCandidateLocation;

                //cout << "p " << i << endl;
                
                for  (int j = 0; j < candidateLocations.size(); j++){
                    //cout << j << endl;
                    int candidateLocation = candidateLocations[j];
                    vector<int> candidateSolutionLocations;

                    for (int k = 0; k < partialSolution.size(); k++)
                        candidateSolutionLocations.push_back(partialSolution[k]);
                    candidateSolutionLocations.push_back(candidateLocation);

                    //cout << "GRS " << endl;

                    int removedLocation = (j == 0) ? -1 : previousCandidateLocation; // from previous iteration
                    double solutionFitness = fitness_partcalc(candidateSolutionLocations, candidateLocation, removedLocation);
                    //cout << solutionFitness << endl;
                    Solution candidateSolution(candidateSolutionLocations, solutionFitness, candidateLocation);
                    candidateSolutions.push_back(candidateSolution);
                    
                    previousCandidateLocation = candidateLocation;

                    //if (j == 4){
                    //    exit(0);
                    //}
                }

                sort(candidateSolutions.begin(), candidateSolutions.end());

                int currentRCLSize = ceil(candidateSolutions.size() * rcl_size);
                int rclSolutionIndex = Utils::getRandomWithinRange(0, currentRCLSize - 1);
                chosen = candidateSolutions[rclSolutionIndex];
                
                partialSolution.push_back(chosen.tmpCandidateLocation);
                candidateLocations.erase(
                    std::remove(
                        candidateLocations.begin(), 
                        candidateLocations.end(), 
                        chosen.tmpCandidateLocation
                    ), 
                    candidateLocations.end()
                );

                //cout << "Chosen solution: ";
                //Utils::printVectorOfInt(chosen.solution);
                //cout << endl;

                customerAllocations.clear();

                //exit(0);

                //if (i == 1){
                //    exit(0);
                //}
            }

            //exit(0);

            //Utils::printVectorOfInt(chosen.solution);
            //cout << chosen.fitness << endl;

            return chosen;
        }

        Solution localSearch(Solution &solution){
            vector<int> chosenLocations = solution.solution;
            vector<int> notChosenLocations;

            for (int i = 0; i < instance->n_candidate_locations; i++){
                if (!(std::find(chosenLocations.begin(), chosenLocations.end(), i) != chosenLocations.end())){
                    notChosenLocations.push_back(i);
                }
            }

            //cout << "Chosen locations: ";
            //Utils::printVectorOfInt(chosenLocations);
            //cout << endl;

            //cout << "Not chosen locations: ";
            //Utils::printVectorOfInt(notChosenLocations);
            //cout << endl;

            Solution improvedSolution = solution;

            for (int chosen = 0; chosen < chosenLocations.size(); chosen++){
                int previousCandidateLocation;

                for (int notChosen = 0; notChosen < notChosenLocations.size(); notChosen++){
                    vector<int> candidateSolution = chosenLocations; // chosenLocations is copied to candidateSolution

                    // Swaps a chosen location for a not chosen one.
                    candidateSolution.erase(
                        std::remove(
                            candidateSolution.begin(), 
                            candidateSolution.end(), 
                            chosenLocations[chosen]
                        ), 
                        candidateSolution.end()
                    );

                    candidateSolution.push_back(notChosenLocations[notChosen]);

                    //cout << "LS " << endl;

                    int removedLocation = (notChosen == 0) ? -1 : previousCandidateLocation; // from previous iteration
                    double candidateFitness = fitness_partcalc(candidateSolution, notChosenLocations[notChosen], removedLocation); // (sol., added, removed)

                    if (candidateFitness < improvedSolution.fitness){
                        Solution newImprovedSolution(candidateSolution, candidateFitness);
                        improvedSolution = newImprovedSolution;

                        if (localSearchMethod == "first_improvement"){
                            return improvedSolution;
                        }
                    }

                    previousCandidateLocation = notChosenLocations[notChosen];

                }

                customerAllocations.clear();
            }

            return improvedSolution;
        }

        Solution loop(){
            milliseconds startTime = Utils::getCurrentTimeInMS();
            vector<double> iterationSolutions;
            int iterationsSinceLastMin = 0;
            int iterationsSinceBestCost = 0;
            int totalSingleMinsFound = 0;
            bool bestUpdated = false;

            std::ofstream outfile(instance->path + "_" + to_string(seed) + "_datasetfull.csv", ios::app);

            for (int iteration = 0; iteration < maxIterations; iteration++){
                milliseconds iterationStartTime = Utils::getCurrentTimeInMS();

                if ((iteration % 1000) == 0){
                    milliseconds currentTime = Utils::getCurrentTimeInMS();
                    cout << "Iteration " << iteration << " Time " << Utils::msToNum(currentTime - startTime) / 1000.0 << endl;
                }

                // constructive phase
                Solution solution = greedyRandomizedSearch();
                // local search phase
                solution = localSearch(solution);

                if (solution.fitness < bestSolution.fitness){
                    bestSolution = solution;

                    iterationsSinceLastMin = 0;
                    iterationsSinceBestCost = 0;
                    totalSingleMinsFound++;
                    bestUpdated = true;
                }else if (solution.fitness == bestSolution.fitness){
                    iterationsSinceLastMin = 0;
                }
                //break;

                milliseconds iterationFinishTime = Utils::getCurrentTimeInMS();



                iterationSolutions.push_back(solution.fitness);

                // string outputstring = to_string(seed) + "," +
                //     instance->path + "," + 
                //     to_string(rcl_size) + "," +
                //     localSearchMethod + "," +
                //     to_string(iteration + 1) + "," + 
                //     to_string(solution.fitness) + "," +
                //     to_string(bestSolution.fitness) + "," +
                //     ((bestUpdated) ? "true" : "false") + "," +
                //     to_string(iterationsSinceLastMin) + "," +
                //     to_string(iterationsSinceBestCost) + "," +
                //     to_string(totalSingleMinsFound) + "," +
                //     to_string(Utils::msToNum(iterationFinishTime - iterationStartTime)) + "," +
                //     to_string(Utils::msToNum(iterationFinishTime - startTime)) + "\n";

                string outputstring = to_string(solution.fitness) + "\n";

                outfile << outputstring; 

                iterationsSinceLastMin++;
                iterationsSinceBestCost++;
                bestUpdated = false;

            }
            milliseconds finishTime = Utils::getCurrentTimeInMS();
            //milliseconds totalTime = finishTime = startTime;
            Utils::printVectorOfInt(bestSolution.solution);
            cout << endl;
            cout << "Instance " << instance->path << " | Solution fitness " << bestSolution.fitness << " | Total time in s: " <<  Utils::msToNum(finishTime - startTime) / 1000.0 << endl;

            return bestSolution;
        }
};

int main(int argc, char** argv) {
    if (argc < 5){
        cout << "You MUST provide at least 4 parameters: \"instance file\", \"seed\", \"rcl size\" (value in the range (0,1]) and \"number of iterations\". A fifth one, \"LS mode\", can also be provided and should be \"best_improvement\" (default) or \"first_improvement\".\nExiting...\n";
        exit(0);
    }
    
    string instanceFile = argv[1];
    int seed = stoi(argv[2]);
    double rclSize = stod(argv[3]);
    int numIterations = stoi(argv[4]);
    string lsMode = "best_improvement";

    if (argc == 6){
        lsMode = argv[5];
    }
    
    cout << "Starting GRASP with parameters: " << instanceFile << " " << seed << " " << rclSize << " " << numIterations << " " << lsMode << endl;

    // seeding the random number generator
    rng.seed(seed);

    Instance instance(instanceFile);
    //cout << instance.path << endl;

    GRASP grasp(instance, rclSize, numIterations, lsMode, seed);
    Solution result = grasp.loop();

    return 0;
} 
