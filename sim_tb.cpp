#include "sim.hpp"
#include "assembler.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

#define IMEM_SIZE 1024
#define DMEM_SIZE 1024

using namespace std;

int main()
{
    uint8_t imem[IMEM_SIZE];
    int8_t dmem[DMEM_SIZE];
    assert(convert() == 0);
    ifstream input("output.hex");
    string line;
    int i = 0;
    while (getline(input, line))
    {
        imem[i] = stoi(line, 0, 16);
        i++;
    }
    input.close();
    core(&imem[0], &dmem[0]);
    return 0;
}