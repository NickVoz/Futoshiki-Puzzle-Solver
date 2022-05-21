#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#define POPULATION_SIZE 100


int boardSize;
std::vector<std::vector<int>> currentGenSolutions[POPULATION_SIZE];



/*
 * Initializes a vector with default values. Each member of the vector represents a row of numbers in the puzzle board.
 */
void initializeVector(std::vector<std::vector<int>>& vec) {
    int i = 0, j = 0;
    for (i; i < boardSize; ++i) {
        std::vector<int> arr;
        for (j = 0; j < boardSize; ++j)
            arr.push_back(-1);
        vec.push_back(arr);
    }
}


int main(int argc, char** argv) {
    // Input validity check
    if (argc < 2) {
        std::cout << "Not enough arguments";
        return -1;
    }
    // open file input stream
    std::ifstream infile(argv[1]);
    std::string line;
    // Read lines from config file
    int lineCounter = 1;
    while (std::getline(infile, line)) {
        std::istringstream iss(line);

        switch (lineCounter) {
            case 1:
                iss >>
        }
    }





}
