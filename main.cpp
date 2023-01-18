#include <iostream>
#include <fstream>
#include <cmath>
#include <bits/stdc++.h>
using namespace std;

const int INF = 1000000.0;

class Solution {
    public:
        string solution;
        double fitness;
        double tmpCandidateLocation; // added just to facilitate some things in the code
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
            test();
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

                cout << position_a << " " << position_b << " " << cost << endl;
            }

            for (int i = 0; i < n_candidate_locations; i++){
                costs[i][i] = 0.0;
            }

            Utils::printCostsVector(costs);

            //vector<vector<double>> finalCosts = 
            Utils::floydWarshall(costs, n_candidate_locations);

            //costs = &initialCosts;

            Utils::printCostsVector(costs);

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
        int max_iterations;
        string local_search_method;
        int seed;

        GRASP(Instance &aInstance, double aRCL_size, int aMax_iterations, string aLocal_search_method, int aSeed = 0){
            instance = &aInstance;
            rcl_size = aRCL_size;
            max_iterations = aMax_iterations;
            local_search_method = aLocal_search_method;
            seed = aSeed;

            vector<int> test = {2, 5, 7, 1};
            fitness(test);
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

            cout << objective_value << " " << penalty << " " << objective_value + penalty << endl;

            return objective_value + penalty;
        }
};

int main() {
    cout << "Hello World!\n";

    Instance instance("pmed1.txt");
    cout << instance.path << endl;

    GRASP grasp(instance, 0.5, 1000, "best_improvement");

    return 0;
} 
