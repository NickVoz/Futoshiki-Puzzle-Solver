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
#define SCORE_PER_COL 3 // 20
#define SCORE_PER_LT 20 // 5
#define SOL_ARR vector<vector<vector<int>>>
#define SOL vector<vector<int>>



int getFitnessScore(SOL solution);
int getMaxSolScore();
int getMaxScoreIndex();
bool isSolved();
double getAvgScore();
vector<int> getBiasedSolutionIndexArray(SOL_ARR solVector);
int getRandomSolutionIndex(vector<int> solArr);
bool isFixedPoint(int x, int y);
void mutate(SOL &sol);
SOL_ARR crossover(SOL vec1, SOL vec2);
int getFitnessScore(SOL solution);
void printFormatError();
void initializeVector(SOL &vec);
void initializeSolutions();
void readInputData(ifstream &fStream);
void printSolution(SOL solutions);
void optimize(SOL_ARR &solsArray);
void basicSolver();
void lamarckianSolver();
void userPrompt();
void darwinianSolver();



#endif //BIOINFORMATICS2_FUTOSHIKI_SOLVE_H
