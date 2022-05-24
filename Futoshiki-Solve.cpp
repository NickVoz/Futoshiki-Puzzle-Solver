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
#define MAX_ITERATIONS 400
#define X1 0
#define Y1 1
#define X2 2
#define Y2 3
#define X 0
#define Y 1
#define VAL 2
#define STARTING_SCORE 1
#define SCORE_PER_FEATURE 2
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
bool isValidSolution(vector<vector<int>> solution);



// Returns maximum solution fitness score from current solution vector
int getMaxSolScore() {
    int score = 0;
    for (auto sol : currentGenSolutions) {
        if (getFitnessScore(sol) > score)
            score = getFitnessScore(sol);
    }
    return score;
}

// Returns position of maximal score
int getMaxScoreIndex() {
    int score = 0, i = 0, index;
    for (auto sol : currentGenSolutions) {
        if (getFitnessScore(sol) > score) {
            score = getFitnessScore(sol);
            index = i;
        }
        ++i;
    }
    return index;
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
    uniform_int_distribution<int> distribution(1, boardSize);
    uniform_int_distribution<int> distributionIndices(0, boardSize - 1);
    mt19937 engine(rd());
    int x, y;
    //for (int k = 0; k < boardSize; ++k) {
        while (true) {
            x = distributionIndices(engine);
            y = distributionIndices(engine);
            bool flg = true;
            auto coors = make_tuple(x, y);
            for (auto item: fixedPoints) {
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
    //}
}

vector<vector<vector<int>>> crossover(vector<vector<int>> vec1, vector<vector<int>> vec2) {
    uniform_int_distribution<int> distribution(0, boardSize - 1);
    vector<vector<vector<int>>> output;
    vector<vector<int>> outputVec1, outputVec2;
    vector<int> crossoverRow1, crossoverRow2;

    mt19937 engine(rd());
    int x = distribution(engine), y = distribution(engine), i, j;
    //while (!isValidSolution(outputVec2) && !isValidSolution(outputVec1)) {
        while (x == 0 && y == 0 || x == boardSize - 1 && y == boardSize - 1) {
            x = distribution(engine);
            y = distribution(engine);
        }

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
    //}
    return output;
}

bool isValidSolution(vector<vector<int>> solution) {
    if (solution.size() == 0)
        return false;
    for (int num = 1; num <= boardSize; ++num) {
        int count = 0;
        for (auto row : solution) {
            for (auto val : row) {
                if (val == num)
                    ++count;
            }
        }
        if (count != boardSize)
            return false;
    }
    return true;
}



// Calculates fintess score of received vector
int getFitnessScore(vector<vector<int>> solution) {
    int x1, y1, x2, y2, i, j, k, score = 1;
    bool flg;
    // Row check
    for (i = 0; i < boardSize; ++i) {
        flg = true;
        auto lineStart = solution[i].begin();
        auto lineEnd = solution[i].end();
        for (j = 0; j < boardSize; ++j) {
            int element = solution[i][j];
            if (count(lineStart, lineEnd, element) > 1) {
                flg = false;
                break;
            }
        }
        if (flg)
            score+= SCORE_PER_FEATURE;
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
    vector<int> numVector;
    for (int j = 1; j <= boardSize; ++j) {
        for (int k = 1; k <= boardSize; ++k) {
            numVector.push_back(j);
        }
    }
    mt19937 engine(rd());
    int i, j;
    for (auto tup : fixedPointsData) {
        int val = get<VAL>(tup);
        auto iter = std::find(numVector.begin(), numVector.end(), val);
        numVector.erase(iter, next(iter, 1));
    }

    for (i = 0; i < boardSize; ++i) {
        vector<int> arr(boardSize);
        for (j = 0; j < boardSize; ++j) {
            auto coor = make_tuple(i, j);

            if (find(fixedPoints.begin(), fixedPoints.end(), coor) != fixedPoints.end())
                continue;
            uniform_int_distribution<int> distribution(0, numVector.size() - 1);
            int selection = distribution(engine);
            arr[j] = numVector[selection];
            numVector.erase(next(numVector.begin(), selection), next(numVector.begin(), selection + 1));

        }
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
    initializeSolutions();
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

//
void printSolution() {
    int solIndex = getMaxScoreIndex();
    auto maxSol = currentGenSolutions[solIndex];
    cout << endl << endl << "Solution is: " << endl;
    for (auto row : maxSol) {
        for (auto number : row) {
            cout << number  << " ";
        }
        cout << endl;
    }
}


// Normal genetic algorithm, without optimisations
void basicSolver() {
    int i = 0;
    while(i < MAX_ITERATIONS && !isSolved()) {
        vector<vector<vector<int>>> newSolArr;
        // Array of solutions proportional to fitness values
        vector<int> biasedSolutionIndexArr = getBiasedSolutionIndexArray();
        int counter = 0;

        // Inner loop, runs for POPULATION_SIZE/2, each time selecting 2 random solutions, performing crossover
        for (int j = 0; j < POPULATION_SIZE / 2 ; ++j) {
            int parent1 = getRandomSolutionIndex(biasedSolutionIndexArr);
            int parent2 = getRandomSolutionIndex(biasedSolutionIndexArr);
            auto children = crossover(currentGenSolutions[parent1], currentGenSolutions[parent2]);
            mutate(children[0]);
            mutate(children[1]);
            newSolArr.push_back(children[0]);
            newSolArr.push_back(children[1]);
        }
        int a= 0,b =0, score = 0, index = 0;
        /*
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
*/
        prevGenSolutions = vector<vector<vector<int>>>(currentGenSolutions);
        currentGenSolutions = newSolArr;

        /*cout << "generation: " << i << endl << "Max is: " << getMaxSolScore() << endl;
        for (auto k : currentGenSolutions) {
            cout << "score is: " << getFitnessScore(k) << endl;
        } */
        double percentage = ((double) getMaxSolScore() / (double) maxScore) * 100;
        cout << "Generation: " << i << " | Max Fitness Score: "<< getMaxSolScore()
             << " | Percentage " << percentage  << "%" << endl;
        ++i;
        printSolution();
    }
}


void userPrompt() {
    int choice;
    cout << "\033[1;41;37m===========================================\033[0m" << endl
         << "\033[1;41;37m==========Futoshiki Puzzle Solver==========\033[0m" << endl
         << "\033[1;41;37m===========================================\033[0m" << endl << endl;
    while (true) {
        cout << "Please choose desired algorithm:" << endl << "1. Basic genetic algorithm" << endl
             << "2. Darwinian genetic algorithm" << endl << "3. Lamarckian genetic algorithm" << endl;
        if (cin.peek() == '\n') {
            cin.clear();
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
        } else if (!(std::cin >> choice) || choice < 1 || choice > 3) {
            cin.clear();
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            cout << "Invalid input, please try again" << endl;
        } else {
            cin.clear();
            cin.ignore(numeric_limits<std::streamsize>::max(), '\n');
            break;
        }
    }
    switch(choice) {
        case 1:
            basicSolver();
            break;
        case 2:
            //darwinianSolver();
            break;
        case 3:
            //lamarckianSolver();
            break;
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

    maxScore = boardSize * SCORE_PER_FEATURE * 2 + cellRelations.size() * SCORE_PER_FEATURE + STARTING_SCORE;
    // maxScore = cellRelations.size() * SCORE_PER_FEATURE + STARTING_SCORE;
    userPrompt();
    if (isSolved()) {
        printSolution();
    }

}
