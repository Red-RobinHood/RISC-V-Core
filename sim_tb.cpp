#include "sim.hpp"
#include "assembler.hpp"
#include <cassert>
#include <iostream>
#include <fstream>

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
    core(imem, dmem);
    cout << (int64_t)dmem[0] << endl;
    cout << (int64_t)dmem[1] << endl;
    cout << (int64_t)dmem[2] << endl;
    cout << (int64_t)dmem[3] << endl;
    return 0;
}