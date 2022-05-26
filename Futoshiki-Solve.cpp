#include "Futoshiki-Solve.h"

int boardSize;
int maxScore;

// Data structures
SOL_ARR currentGenSolutions;
vector<tuple<int, int, int>> fixedPointsData; // Formatted as (x, y, VAL)
vector<tuple<int, int>> fixedPoints; // Coordinates only - formatted as (x, y)
vector<tuple<int, int, int, int>> cellRelations; // Formatted as (x1, y1, x2, y2)
SOL currentBestSol;
std::random_device rd;


int main(int argc, char **argv) {
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

    maxScore = boardSize * SCORE_PER_COL + cellRelations.size() * SCORE_PER_LT + STARTING_SCORE;
    // maxScore = cellRelations.size() * SCORE_PER_FEATURE + STARTING_SCORE;
    userPrompt();
    cout << endl << endl;

    if (isSolved()) {
        cout << "Solution found:" << endl;
        printSolution(currentGenSolutions[getMaxScoreIndex()]);

    } else {
        cout << "Closest found solution:" << endl;
        printSolution(currentBestSol);
    }

    cout << endl << "Results written to results.csv" << endl;
}

/******************** Input Parsing ********************/

// Read data from received file, update different variables and fields accordingly.
void readInputData(ifstream &fStream) {
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
    while (givenDigits > 0) {
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
        for (auto tup: fixedPointsData) {
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
            // remove one from all read digits to convert into cell coordinates
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

/******************** Initialization Functions *********************/

// Initializes a vector with a random permutation from range [1-boardSize]
// Each member of the vector represents a row of numbers in the puzzle board.
void initializeVector(SOL &vec) {
    mt19937 engine(rd());
    int i, j;
    // Loop over board size
    for (i = 0; i < boardSize; ++i) {
        vector<int> arr(boardSize);
        vector<int> numVector;
        // Creates a vector of numbers in range [1-boardSize]
        for (int k = 1; k <= boardSize; ++k) {
            numVector.push_back(k);
        }
        // Assigns fixed values to the board
        for (auto tup: fixedPointsData) {
            int val = get<VAL>(tup);
            int x = get<X>(tup);
            // if x value of a fixed point matches current row number, removes the fixed point number from the range
            if (x == i) {
                auto iter = std::find(numVector.begin(), numVector.end(), val);
                numVector.erase(iter, next(iter, 1));
            }
        }
        // Choose integer values to add to row
        for (j = 0; j < boardSize; ++j) {
            auto coor = make_tuple(i, j);
            if (find(fixedPoints.begin(), fixedPoints.end(), coor) != fixedPoints.end())
                continue;
            // Selects random number from range array, selects it as next item in current row vector
            uniform_int_distribution<int> distribution(0, numVector.size() - 1);
            int selection = distribution(engine);
            arr[j] = numVector[selection];
            // delete added value from range
            numVector.erase(next(numVector.begin(), selection), next(numVector.begin(), selection + 1));
        }
        vec.push_back(arr);
    }
}


// Generate an empty solutions vector, consisting of boardSize number of solution vectors
void initializeSolutions() {
    for (int i = 0; i < POPULATION_SIZE; ++i) {
        SOL vec;
        initializeVector(vec);
        currentGenSolutions.push_back(vec);
    }

}


/******************** User Prompt ********************/

// Prints interactive user prompt
void userPrompt() {
    int choice;
    // Creates results file
    std::ofstream outputFile("results.csv");
    system("clear");
    outputFile << "Generation,MaxPossibleScore,MaxScore,AvgFitness,PercentCorrect" << endl;
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
    int i = 0;
    // Loops for MAX_ITERATIONS
    while (i < MAX_ITERATIONS) {
        // Write current iteration stats
        double percentage = ((double) getMaxSolScore() / (double) maxScore) * 100;
        // Write data to file and print to stdout
        outputFile << i + 1 << "," << maxScore << "," << getMaxSolScore() <<
                   "," << getAvgScore() << "," << percentage << endl;
        cout << "Generation: " << i + 1 << " | Max possible Fitness: " << maxScore << " | Max Fitness Score: " <<
             getMaxSolScore() << " | Average Score: " << getAvgScore() << " | Percentage " << percentage << "%" <<
             endl;
        // Will stop if reaches a solution
        if (!isSolved()) {
            switch (choice) {
                case 1:
                    basicSolver();
                    break;
                case 2:
                    darwinianSolver();
                    break;
                case 3:
                    lamarckianSolver();
                    break;
            }

            // once in 20 generations add the overall best solution
            if (i % 100 == 0)
                currentGenSolutions.push_back(currentBestSol);

            ++i;
        } else
            break;
    }
    outputFile.close();
}

/******************** Algorithm Execution ********************/

// Basic evolution algorithm
void basicSolver() {
    // Array of solutions proportional to fitness values
    vector<int> biasedSolutionIndexArr = getBiasedSolutionIndexArray(currentGenSolutions);
    SOL_ARR newSolArr;
    // Inner loop, runs for POPULATION_SIZE/2, each time selecting 2 random solutions, performing crossover
    for (int j = 0; j < POPULATION_SIZE / 2; ++j) {
        SOL_ARR children(2);
        int parent1, parent2;
        // Create new children from crossover until child has score higher than 5/16 * maxScore
        do {
            children.clear();
            parent1 = getRandomSolutionIndex(biasedSolutionIndexArr);
            parent2 = getRandomSolutionIndex(biasedSolutionIndexArr);

            children = crossover(currentGenSolutions[parent1], currentGenSolutions[parent2]);
            mutate(children[0]);
            mutate(children[1]);
        } while (getFitnessScore(children[0]) < maxScore * 5 / 16 && getFitnessScore(children[1]) < maxScore * 5 / 16);
//        while (getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent2]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent2]));
        newSolArr.push_back(children[0]);
        newSolArr.push_back(children[1]);
    }
    currentGenSolutions = newSolArr;
    if (getFitnessScore(currentBestSol) < getMaxSolScore())
        currentBestSol = currentGenSolutions[getMaxScoreIndex()];
}

// Darwinian evolution algorithm
void darwinianSolver() {
    SOL_ARR optimizedSolutions(currentGenSolutions);
    optimize(optimizedSolutions);

    // Array of solutions proportional to fitness values
    vector<int> biasedSolutionIndexArr = getBiasedSolutionIndexArray(optimizedSolutions);
    SOL_ARR newSolArr;

    // Inner loop, runs for POPULATION_SIZE/2, each time selecting 2 random solutions, performing crossover
    for (int j = 0; j < POPULATION_SIZE / 2; ++j) {
        SOL_ARR children(2);
        int parent1, parent2;
        do {
            children.clear();
            parent1 = getRandomSolutionIndex(biasedSolutionIndexArr);
            parent2 = getRandomSolutionIndex(biasedSolutionIndexArr);
            children = crossover(currentGenSolutions[parent1], currentGenSolutions[parent2]);
            mutate(children[0]);
            mutate(children[1]);
        } while (getFitnessScore(children[0]) < maxScore * 5 / 16 && getFitnessScore(children[1]) < maxScore * 5 / 16);
//        while (getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent2]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent2]));
        newSolArr.push_back(children[0]);
        newSolArr.push_back(children[1]);
    }
    currentGenSolutions = newSolArr;
    if (getFitnessScore(currentBestSol) < getMaxSolScore())
        currentBestSol = currentGenSolutions[getMaxScoreIndex()];
}

// Lamarckian evolution algorithm
void lamarckianSolver() {
    optimize(currentGenSolutions);
    // Array of solutions proportional to fitness values
    vector<int> biasedSolutionIndexArr = getBiasedSolutionIndexArray(currentGenSolutions);
    SOL_ARR newSolArr;

    // Inner loop, runs for POPULATION_SIZE/2, each time selecting 2 random solutions, performing crossover
    for (int j = 0; j < POPULATION_SIZE / 2; ++j) {
        SOL_ARR children(2);
        int parent1, parent2;
        do {
            children.clear();
            parent1 = getRandomSolutionIndex(biasedSolutionIndexArr);
            parent2 = getRandomSolutionIndex(biasedSolutionIndexArr);

            children = crossover(currentGenSolutions[parent1], currentGenSolutions[parent2]);
            mutate(children[0]);
            mutate(children[1]);
        } while (getFitnessScore(children[0]) < maxScore * 5 / 16 && getFitnessScore(children[1]) < maxScore * 5 / 16);
//        while (getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[0]) < getFitnessScore(currentGenSolutions[parent2]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent1]) ||
//               getFitnessScore(children[1]) < getFitnessScore(currentGenSolutions[parent2]));
        newSolArr.push_back(children[0]);
        newSolArr.push_back(children[1]);
    }
    currentGenSolutions = newSolArr;
    if (getFitnessScore(currentBestSol) < getMaxSolScore())
        currentBestSol = currentGenSolutions[getMaxScoreIndex()];
}


/******************** Algorithm Implementation ********************/

// Performs genetic optimization on received solution array for a maximum
void optimize(SOL_ARR &solsArray) {
    int successful = 0, size = cellRelations.size();
    int x1, x2, y1, y2, i = 0;
    // loops over all solutions
    for (auto solution: solsArray) {
        // First series of optimizations - loop until
        for (i = 0; i < size && successful < boardSize; ++i) {
            x1 = get<X1>(cellRelations[i]);
            y1 = get<Y1>(cellRelations[i]);
            x2 = get<X2>(cellRelations[i]);
            y2 = get<Y2>(cellRelations[i]);
            // Checks wheter one of the coordinates is fixed, wont swap if so
            if (isFixedPoint(x1, y1) || isFixedPoint(x2, y2))
                continue;
            int val1 = solution[x1][y1], val2 = solution[x2][y2];
            // Checks whether less than statement is correct
            if (val1 >= val2)
                continue;
            // if located on same row, swap values
            if (x1 == x2) {
                solution[x1][y1] = val2;
                solution[x2][y2] = val1;
            }
                // if coordinates are not on same row, first locates the corresponding digit to swap
                // and swaps them as well, in order to maintain each row being a permutation of [1-boardSize]
            else {
                auto val2coor = find(solution[x1].begin(), solution[x1].end(), val2);
                auto val1coor = find(solution[x2].begin(), solution[x2].end(), val1);
                *val1coor = val2;
                *val2coor = val1;
                solution[x1][y1] = val2;
                solution[x2][y2] = val1;
            }
            ++successful;
        }
        // Second optimization, swaps a number in a column containing duplicate with another number from the same row,
        // making sure there is one less duplicate in the column
        // respects fixed numbers and '>' relations between checked cells
        for (int y = 0; y < boardSize && successful < boardSize; ++y) {
            vector<int> col;

            int val = 0, row;
            bool hasDupes = true;
            for (int x = 0; x < boardSize; ++x) {
                if (find(col.begin(), col.end(), solution[x][y]) != col.end() && hasDupes) {
                    if (isFixedPoint(x, y))
                        continue;
                    val = solution[x][y];
                    row = x;
                    hasDupes = false;
                }
                col.push_back(solution[x][y]);
            }
            if (val == 0)
                continue;
            int j = 1;

            // Find non-existing integer in the column, swap it with selected integer
            for (; j <= boardSize; ++j) {
                bool containsNumber = false;
                while (find(col.begin(), col.end(), j) != col.end()) {
                    auto a = find(col.begin(), col.end(), j);

                    col.erase(a);
                    containsNumber = true;
                }
                if (!containsNumber)
                    break;
            }
            auto numIndex = find(solution[row].begin(), solution[row].end(), j);
            if (isFixedPoint(row, numIndex - col.begin()))
                continue;
            bool flg = false;
            // Checkes whether "less than" relations are maintained
            for (auto coor : cellRelations) {
                x1 = get<X1>(coor);
                y1 = get<Y1>(coor);
                x2 = get<X2>(coor);
                y2 = get<Y2>(coor);
                if (x1 == row && x2 == row) {
                    if (y1 == y && y2 == numIndex - col.begin()) {
                        if (j > val) {
                            flg = true;
                            break;
                        }
                    }
                    if (y1 == numIndex - col.begin() && y2 == y) {
                        if (val > j) {
                            flg =true;
                            break;
                        }
                    }
                }
            }
            if (!flg)
                continue;
            *numIndex = val;
            solution[row][y] = j;
            ++successful;
        }
    }
}


// Create mutation in received solution
void mutate(SOL &sol) {
    uniform_int_distribution<int> distribution(1, boardSize);
    uniform_int_distribution<int> distributionIndices(0, boardSize - 1);
    mt19937 engine(rd());
    int x1, y1, y2;
    // Rolls new row and column indices while at least one of them is a fixed point
    while (true) {
        x1 = distributionIndices(engine);
        y1 = distributionIndices(engine);
        y2 = distributionIndices(engine);
        if (isFixedPoint(x1, y1) || isFixedPoint(x1, y2))
            continue;
        break;
    }
    // Mutate by swapping two members of a row vector
    int x1Val = sol[x1][y1], x2Val = sol[x1][y2];
    sol[x1][y1] = x2Val;
    sol[x1][y2] = x1Val;
}

// Perform crossover of two vectors
SOL_ARR crossover(SOL vec1, SOL vec2) {
    uniform_int_distribution<int> distribution(0, boardSize - 1);
    SOL_ARR output;
    SOL outputVec1, outputVec2;

    mt19937 engine(rd());
    int x = distribution(engine), i; // x contains a row index
    while (x == 0 || x == boardSize - 1) {
        x = distribution(engine); // choose again if x is top or bottom row
    }
    // outputVec1 contains first x rows of received vec1 and last boardSize -x rows are from vec2
    for (i = 0; i < x; ++i) {
        outputVec1.push_back(vec1[i]);
    }
    for (; i < boardSize; ++i) {
        outputVec1.push_back(vec2[i]);
    }

    // Second vector
    for (i = 0; i < x; ++i) {
        outputVec2.push_back(vec2[i]);
    }

    for (; i < boardSize; ++i) {
        outputVec2.push_back(vec1[i]);
    }
    output.push_back(outputVec1);
    output.push_back(outputVec2);
    return output;
}

/*
 * Calculates fitness score of received vector
 * Assigns different score based on amount of correct columns(only containing different values)
 * and number of correct '>' sign number assignments
 */
int getFitnessScore(SOL solution) {
    int x1, y1, x2, y2, i, j, k, score = 1;
    bool flg;
    if (solution.size() < boardSize)
        return 0;

    // Column check
    for (j = 0; j < boardSize; ++j) {
        flg = true;
        for (i = 0; i < boardSize; ++i) {
            int element = solution[i][j];
            for (k = i + 1; k < boardSize; ++k) {
                int checkedElement = solution[k][j];
                if (element == checkedElement) {
                    // if column contains checked element does not add additional score
                    flg = false;
                    break;
                }
            }
            if (!flg)
                break;
        }
        if (flg)
            score += SCORE_PER_COL;
    }
    // Check 'greater than' values, add score accordingly
    for (auto coor: cellRelations) {
        x1 = get<X1>(coor);
        y1 = get<Y1>(coor);
        x2 = get<X2>(coor);
        y2 = get<Y2>(coor);
        if (solution[x1][y1] > solution[x2][y2])
            score += SCORE_PER_LT;
    }
    return score;
}

/******************** Auxiliary Functions ********************/

// Returns maximum solution fitness score from current solution vector
int getMaxSolScore() {
    int score = 0;
    for (auto sol: currentGenSolutions) {
        if (getFitnessScore(sol) > score)
            score = getFitnessScore(sol);
    }
    return score;
}

// Returns position of maximal score
int getMaxScoreIndex() {
    int score = 0, i = 0, index;
    for (auto sol: currentGenSolutions) {
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

// Calculates average score of current iteration
double getAvgScore() {
    double counter;
    for (auto item: currentGenSolutions) {
        counter += getFitnessScore(item);
    }
    return counter / (double) currentGenSolutions.size();
}

// Returns a random solution from the solutions array, biased towards higher fitness solutions
vector<int> getBiasedSolutionIndexArray(SOL_ARR solVector) {
    vector<int> output;
    int index = 0;
    for (auto item: solVector) {
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

// Checks whether a point is part of the fixed points provided in the config file
bool isFixedPoint(int x, int y) {
    auto coors = make_tuple(x, y);
    for (auto item: fixedPoints) {
        if (item == coors) {
            return true;
        }
    }
    return false;
}

// Prints error message and exits the program in case input was incorrect
void printFormatError() {
    cout << "Error: Incorrect file formatting" << endl;
    exit(-1);
}

// Prints received solution
void printSolution(SOL solutions) {
    for (auto row: solutions) {
        for (auto number: row) {
            cout << number << " ";
        }
        cout << endl;
    }
}
