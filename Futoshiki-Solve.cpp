#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <tuple>
#include <algorithm>
#include <random>
using namespace std;

#define POPULATION_SIZE 100
#define MAX_ITERATIONS 500
#define X1 0
#define Y1 1
#define X2 2
#define Y2 3
#define X 0
#define Y 1
#define VAL 2
#define STARTING_SCORE 1
#define SCORE_PER_FEATURE 7
#define SOL_ARR vector<vector<vector<int>>>
#define SOL vector<vector<int>>

int boardSize;
int maxScore;

// Data structures
vector<vector<vector<int>>> currentGenSolutions;
vector<vector<vector<int>>> prevGenSolutions;
vector<tuple<int, int, int>> fixedPointsData; // Formatted as (x, y, VAL)
vector<tuple<int, int>> fixedPoints; // Coordinates only - formatted as (x, y)
vector<tuple<int, int, int, int>> cellRelations; // Formatted as (x1, y1, x2, y2)

std::random_device rd;


int getFitnessScore(vector<vector<int>> solution);

// Returns maximum solution fitness score from current solution vector
int getMaxSolScore() {
    int score = 0;
    for (auto sol : currentGenSolutions) {
        if (getFitnessScore(sol) > score)
            score = getFitnessScore(sol);
    }
    return score;
}

// Checks if a correct solution exists in the solution vector
bool isSolved() {
    int score = getMaxSolScore();
    return score == maxScore;
}

// Returns a random solution from the solutions array, biased towards higher fitness solutions
vector<int> getBiasedSolutionIndexArray() {
    vector<int> output;
    int index = 0;
    for (auto item : currentGenSolutions) {
        int score = getFitnessScore(item);
        for (int i = 0; i < score; ++i) {
            output.push_back(index);
        }
        index++;
    }
    return output;
}

// Returns a random solution index from vector of solutions
int getRandomSolutionIndex(vector<int> solArr) {
    uniform_int_distribution<int> distribution(0, solArr.size() - 1);
    mt19937 engine(rd());
    int i = distribution(engine);
    return solArr[i];
}


// Create mutation in received solution
void mutate(vector<vector<int>>& sol) {
    uniform_int_distribution<int> distribution(0, boardSize - 1);
    mt19937 engine(rd());
    int x, y;

    while (true) {
        x = distribution(engine);
        y = distribution(engine);
        bool flg = true;
        auto coors = make_tuple(x, y);
        for (auto item : fixedPoints) {
            if (item == coors) {
                flg = false;
                break;
            }
        }
        if (flg)
            break;
    }
    int prevVal = sol[x][y], newVal = prevVal;
    while (prevVal == newVal) {
        newVal = distribution(engine);
    }
    sol[x][y] = newVal;

}

vector<vector<vector<int>>> crossover(vector<vector<int>> vec1, vector<vector<int>> vec2) {
    uniform_int_distribution<int> distribution(0, boardSize - 1);
    mt19937 engine(rd());
    int x = distribution(engine), y = distribution(engine), i;
    //int x = 0, y = 0, i;
    vector<vector<vector<int>>> output;
    vector<vector<int>> outputVec1, outputVec2;
    vector<int> crossoverRow1, crossoverRow2;
    for (i = 0; i < x; ++i) {
        outputVec1.push_back(vec1[i]);
    }
    for (i = 0; i < y; ++i) {
        crossoverRow1.push_back(vec1[x][i]);
    }
    for (; i < boardSize; ++i) {
        crossoverRow1.push_back(vec2[x][i]);
    }
    outputVec1.push_back(crossoverRow1);
    for (i = x + 1; i < boardSize; ++i) {
        outputVec1.push_back(vec2[i]);
    }
    output.push_back(outputVec1);

    // Second vector
    for (i = 0; i < x; ++i) {
        outputVec2.push_back(vec2[i]);
    }
    for (i = 0; i < y; ++i) {
        crossoverRow2.push_back(vec2[x][i]);
    }
    for (; i < boardSize; ++i) {
        crossoverRow2.push_back(vec1[x][i]);
    }
    outputVec2.push_back(crossoverRow2);
    for (i = x + 1; i < boardSize; ++i) {
        outputVec2.push_back(vec1[i]);
    }
    output.push_back(outputVec2);

    return output;
}


// Calculates fintess score of received vector
int getFitnessScore(vector<vector<int>> solution) {
    int i, j, k, score = 1, x1, y1, x2, y2;
    bool flg;
    // Row check
    for (i = 0; i < boardSize; ++i) {
        flg = true;
        auto lineStart = solution[i].begin();
        auto lineEnd = solution[i].end();
        for (j = 0; j < boardSize; ++j) {
            int element = solution[i][j];
            if (!count(lineStart + j, lineEnd, element)) {
                flg = false;
                break;
            }
        }
        if (flg)
            score+=SCORE_PER_FEATURE;
    }
    // Column check
    for (j = 0; j < boardSize; ++j) {
        flg = true;
        for (i = 0; i < boardSize; ++i) {
            int element = solution[i][j];
            for (k = i + 1; k < boardSize; ++k) {
                int checkedElement = solution[k][j];
                if (element == checkedElement) {
                    flg = false;
                    break;
                }
            }
            if (!flg)
                break;
        }
        if (flg)
            score += SCORE_PER_FEATURE;
    }
    // Check 'greater than' values
    for (auto coor : cellRelations) {
        x1 = get<X1>(coor);
        y1 = get<Y1>(coor);
        x2 = get<X2>(coor);
        y2 = get<Y2>(coor);
        if (solution[x1][y1] > solution[x2][y2])
            score += SCORE_PER_FEATURE;
    }
    return score;
}

void printFormatError() {
    cout << "Error: Incorrect file formatting" << endl;
    exit(-1);
}


/******************Initialization Functions******************/

// Initializes a vector with random values. Each member of the vector represents a row of numbers in the puzzle board.
void initializeVector(vector<vector<int>>& vec) {
    uniform_int_distribution<int> distribution(1, boardSize);
    mt19937 engine(rd());

    int i, j;
    for (i = 0; i < boardSize; ++i) {
        vector<int> arr;
        for (j = 0; j < boardSize; ++j)
            arr.push_back(distribution(engine));
        vec.push_back(arr);
    }
}



// Generate an empty solutions vector, consisting of boardSize number of solution vectors
void initializeSolutions() {
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        vector<vector<int>> vec;
        initializeVector(vec);
        currentGenSolutions.push_back(vec);
    }

}


// Read data from received file, update different variables and fields accordingly.
void readInputData(ifstream& fStream) {
    string line;
    int givenDigits;
    // Read board size line
    getline(fStream, line);
    boardSize = stoi(line);
    // Initialize solution matrix
    initializeSolutions();
    getline(fStream, line);
    givenDigits = stoi(line);
    // Error check
    if (givenDigits == 0 || boardSize == 0)
        printFormatError();
    // Read fixed matrix points
    while(givenDigits > 0) {
        int x, y, digit; // Coordinates of requested digit.
        if (getline(fStream, line)) {
            istringstream iss(line);
            // Check for errors in input file
            if (!(iss >> x >> y >> digit))
                printFormatError();
            x--;
            y--;
            fixedPoints.push_back(make_tuple(x, y));
            fixedPointsData.push_back(make_tuple(x, y, digit));
            givenDigits--;
        } else
            printFormatError();
    }
    // Update results matrix with fixed points
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        for (auto tup : fixedPointsData) {
            int x = get<X>(tup);
            int y = get<Y>(tup);
            int val = get<VAL>(tup);
            currentGenSolutions[i][x][y] = val;
        }
    }
    getline(fStream, line);
    int greaterThanNum;
    if ((greaterThanNum = stoi(line)) == 0)
        printFormatError();
    // Read 'greater than' cell coordinates
    while (greaterThanNum > 0) {
        int x1, y1, x2, y2; // Coordinates of two cells.
        if (getline(fStream, line)) {
            istringstream iss(line);
            // Check for errors in input file
            if (!(iss >> x1 >> y1 >> x2 >> y2)) {
                cout << "Error: Incorrect file formatting" << endl;
                exit(-1);
            }
            x1--;
            y1--;
            x2--;
            y2--;
            auto pointTup = make_tuple(x1, y1, x2, y2);
            cellRelations.push_back(pointTup);
            greaterThanNum--;
        } else
            printFormatError();
    }
}



int main(int argc, char** argv) {
    // Input validity check
    if (argc < 2) {
        cout << "Error: Not enough arguments" << endl;
        return -1;
    }
    // open file input stream
    ifstream input(argv[1]);
    if (input.fail()) {
        cout << "Error: unable to open input file" << endl;
        return -1;
    }
    readInputData(input);
    input.close();

    int i = 0;
    maxScore = boardSize * SCORE_PER_FEATURE * 2 + cellRelations.size() * SCORE_PER_FEATURE + STARTING_SCORE;

    while(i < MAX_ITERATIONS && !isSolved()) {
        vector<vector<vector<int>>> newSolArr;
        // Array of solutions proportional to fitness values
        vector<int> biasedSolutionIndexArr = getBiasedSolutionIndexArray();
        int counter = 0;

        // Inner loop, runs for POPULATION_SIZE/2, each time selecting 2 random solutions, performing crossover
        for (int j = 0; j < POPULATION_SIZE / 2 - 2; ++j) {
            int parent1 = getRandomSolutionIndex(biasedSolutionIndexArr);
            int parent2 = getRandomSolutionIndex(biasedSolutionIndexArr);
            auto children = crossover(currentGenSolutions[parent1], currentGenSolutions[parent2]);
            mutate(children[0]);
            mutate(children[1]);
            newSolArr.push_back(children[0]);
            newSolArr.push_back(children[1]);
        }
        int a= 0,b =0, score = 0, index = 0;
        for (auto  w :currentGenSolutions) {
            if (getFitnessScore(w) > score) {
                b = a;
                a = index;
                score = getFitnessScore(w);
            }
            index++;
        }
        newSolArr.push_back(currentGenSolutions[a]);
        newSolArr.push_back(currentGenSolutions[b]);

        prevGenSolutions = vector<vector<vector<int>>>(currentGenSolutions);
        currentGenSolutions = newSolArr;
        /*
        cout << "generation: " << i << endl << "Max is: " << getMaxSolScore() << endl;
        for (auto k : currentGenSolutions) {
            cout << "score is: " << getFitnessScore(k) << endl;
        } */
        cout << "max: " << maxScore <<endl;
        cout << "Generation: " << i << endl << "Current highest score: " << getMaxSolScore() << endl;
       ++i;
    }

    cout << "";
}
