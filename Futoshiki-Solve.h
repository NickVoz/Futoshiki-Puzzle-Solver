//
// Created by nick on 5/26/22.
//

#ifndef BIOINFORMATICS2_FUTOSHIKI_SOLVE_H
#define BIOINFORMATICS2_FUTOSHIKI_SOLVE_H

#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <random>

using namespace std;
// Parameters and constants
#define POPULATION_SIZE 300
#define MAX_ITERATIONS 1000
#define X1 0
#define Y1 1
#define X2 2
#define Y2 3
#define X 0
#define Y 1
#define VAL 2
#define STARTING_SCORE 1
#define SCORE_PER_COL 15 // 20
#define SCORE_PER_LT 5 // 5
#define SOL_ARR vector<vector<vector<int>>>
#define SOL vector<vector<int>>



int getFitnessScore(SOL solution);

bool isValidSolution(SOL solution);
int getMaxSolScore();


#endif //BIOINFORMATICS2_FUTOSHIKI_SOLVE_H
