#include<cstdint>
#include<iostream>

using namespace std;

struct fedex
{
    uint32_t istn;
    int64_t src1;
    int64_t src2;
    int64_t src3;

    fedex(uint32_t istn, int64_t src1, int64_t src2, int64_t src3)
    {
        this->istn = istn;
        this->src1 = src1;
        this->src2 = src2;
        this->src3 = src3;
    }
};

struct exmem
{
    uint16_t istn;
    int64_t res;
    int64_t src3;

    exmem(uint16_t istn, int64_t res, int64_t src3)
    {
        this->istn = istn;
        this->res = res;
        this->src3 = src3;
    }
};

void core();
fedex fedec();
exmem execute();
int64_t ALU(uint8_t ALUop, int64_t arg1, int64_t arg2);
void memwb();
int32_t imemectrl(uint32_t pc, uint8_t *imem);
int64_t dmemctrl(uint32_t addr, int8_t *dmem, int64_t data, uint8_t we, uint8_t sz);