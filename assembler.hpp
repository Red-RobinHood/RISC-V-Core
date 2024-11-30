#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <stack>
using namespace std;

void printVector(vector<string> v);
bool safeStoi(string s, int line, int base=10);
string binToHex(string bin);
int hexToInt(string hex, int line);
int binToInt(string bin, int line);
bool checkRegister(int reg, int line);
int getRegister(string reg, unordered_map<string, string> alias, int line);
pair<vector<string>, bool> getArguments(int line, int count, string args, bool flag);
pair<int, bool> getImmediate(string str, int pc, unordered_map<string, int> label, bool flag);
int convert();