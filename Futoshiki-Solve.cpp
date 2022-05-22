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
#define MAX_ITERATIONS 100

int boardSize;

// Data structures
vector<vector<int>> currentGenSolutions[POPULATION_SIZE];
vector<tuple<int, int, int>> fixedPointsData; // Formatted as (x, y, VAL)
vector<tuple<int, int>> fixedPoints; // Coordinates only - formatted as (x, y)
vector<tuple<int, int, int, int>> cellRelations; // Formatted as (x1, y1, x2, y2

std::random_device rd;


void printFormatError() {
    cout << "Error: Incorrect file formatting" << endl;
    exit(-1);
}


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
        currentGenSolutions[i] = vec;
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
            int x = get<0>(tup);
            int y = get<1>(tup);
            int val = get<2>(tup);
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
    cout << "hello";
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
    int i = 0;
    while(i < MAX_ITERATIONS) {




        ++i;
    }



}
