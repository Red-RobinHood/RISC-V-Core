#include "sim.hpp"

int64_t regFile[32];
uint64_t PC = 0;
int8_t run = 1;
fedex pipe1(0, 0, 0, 0);
exmem pipe2(0, 0, 0);

void core(uint8_t *imem, int8_t *dmem)
{
    for (int i = 0; i < 32; i++)
        regFile[i] = 0;
#pragma HLS PIPELINE
#pragma HLS ARRAY_PARTITION dim = 1 factor = 32 type = cyclic variable = regFile
#pragma HLS ARRAY_PARTITION dim = 1 factor = 4 type = cyclic variable = imem
#pragma HLS ARRAY_PARTITION dim = 1 factor = 8 type = cyclic variable = dmem
    while (run)
    {
        pipe1 = fedec(imem);
        pipe2 = execute();
        memwb(dmem);
    }
}

fedex fedec(uint8_t *imem)
{
    uint32_t istn = imemectrl(PC, imem);
    int64_t rd = (istn >> 7) & 0x1F;
    int64_t rs1 = regFile[(istn >> 15) & 0x1F];
    int64_t rs2 = regFile[(istn >> 20) & 0x1F];
    int64_t imm = 0;
    PC += 4;
    switch (istn & 0x7F)
    {
    case 0b0110011:
        return fedex(istn, rs1, rs2, rd);
    case 0b0010011:
    case 0b0000011:
    case 0b1100111:
        imm = istn >> 20;
        imm |= (imm & 0x100) ? 0xFFFFFFFFFFFFF000 : 0;
        return fedex(istn, rs1, imm, rd);
    case 0b0100011:
        imm = (istn >> 25) << 5 | (istn >> 7) & 0x1F;
        imm |= (imm & 0x100) ? 0xFFFFFFFFFFFFF000 : 0;
        return fedex(istn, rs1, imm, rs2);
    case 0b1100011:
        imm = (((istn >> 31) << 12) | (((istn >> 7) & 0x1) << 11) | (((istn >> 25) & 0x3F) << 5) | ((istn >> 8) & 0xF) << 1);
        imm |= (imm & 0x1000) ? 0xFFFFFFFFFFFFE000 : 0;
        switch ((istn >> 12) & 0x7)
        {
        case 0b000:
            if (regFile[rs1] == regFile[rs2])
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        case 0b001:
            if (regFile[rs1] != regFile[rs2])
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        case 0b100:
            if (regFile[rs1] < regFile[rs2])
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        case 0b101:
            if (regFile[rs1] >= regFile[rs2])
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        case 0b110:
            if (static_cast<uint64_t>(regFile[rs1]) < static_cast<uint64_t>(regFile[rs2]))
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        case 0b111:
            if (static_cast<uint64_t>(regFile[rs1]) >= static_cast<uint64_t>(regFile[rs2]))
                return fedex(istn, (int64_t)PC - 4, imm, 0);
            break;
        }
        return fedex(istn, (int64_t)PC, 0, 0);
    case 0b1101111:
        rd = regFile[rd];
        imm = (((istn >> 31) << 20) | (((istn >> 12) & 0xFF) << 12) | (((istn >> 20) & 0x1) << 11) | ((istn >> 21) & 0x3FF) << 1);
        imm |= (imm & 0x100000) ? 0xFFFFFFFFFFE00000 : 0;
        return fedex(istn, (int64_t)PC - 4, imm, rd);
    case 0b0110111:
        rd = regFile[rd];
        imm = istn & 0xFFFFF000;
        return fedex(istn, imm, 12, rd);
    }
    return fedex(istn, 0, 0, 0);
}

exmem execute()
{
    int8_t ALUop;
    int8_t opcode = pipe1.istn & 0x7F;
    int64_t istn = pipe1.istn;
    if (opcode == 0)
    {
        run = 0;
        return exmem(0, 0, 0);
    }
    switch (opcode)
    {
    case 0b0110111:
        ALUop = 1;
        break;
    case 0b0010011:
        ALUop = (pipe1.istn >> 12) & 0x7;
        break;
    case 0b0110011:
        ALUop = ((pipe1.istn >> 12) & 0x7) | ((pipe1.istn >> 27) & 0x8);
        break;
    default:
        ALUop = 0;
        break;
    }
    return exmem(istn, ALU(ALUop, pipe1.src1, pipe1.src2), pipe1.src3);
}

int64_t ALU(uint8_t ALUop, int64_t arg1, int64_t arg2)
{
    switch (ALUop)
    {
    case 0:
        return arg1 + arg2;
    case 1:
        return arg1 << arg2;
    case 4:
        return arg1 ^ arg2;
    case 5:
        return static_cast<uint64_t>(arg1) >> arg2;
    case 6:
        return arg1 | arg2;
    case 7:
        return arg1 & arg2;
    case 8:
        return arg1 - arg2;
    case 13:
        return arg1 >> arg2;
    }
    return 0;
}

void memwb(int8_t *dmem)
{
    switch (pipe2.istn & 0x7F)
    {
    case 0b0110011:
    case 0b0010011:
        regFile[pipe2.src3] = pipe2.res;
        break;
    case 0b0000011:
        switch ((pipe2.istn >> 12) & 0x7)
        {
        case 0b000:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 1);
            (regFile[pipe2.src3] & 0x80) ? regFile[pipe2.src3] |= 0xFFFFFFFFFFFFFF00 : regFile[pipe2.src3] &= 0xFF;
            break;
        case 0b001:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 2);
            (regFile[pipe2.src3] & 0x8000) ? regFile[pipe2.src3] |= 0xFFFFFFFFFFFF0000 : regFile[pipe2.src3] &= 0xFFFF;
            break;
        case 0b010:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 4);
            (regFile[pipe2.src3] & 0x80000000) ? regFile[pipe2.src3] |= 0xFFFFFFFF00000000 : regFile[pipe2.src3] &= 0xFFFFFFFF;
            break;
        case 0b011:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 8);
            break;
        case 0b100:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 1);
            regFile[pipe2.src3] &= 0xFF;
            break;
        case 0b101:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 2);
            regFile[pipe2.src3] &= 0xFFFF;
            break;
        case 0b110:
            regFile[pipe2.src3] = dmemctrl(pipe2.res, dmem, 0, 0, 4);
            regFile[pipe2.src3] &= 0xFFFFFFFF;
            break;
        }
        break;
    case 0b1100011:
        PC = pipe2.res;
    case 0b0100011:
        dmemctrl(pipe2.res, dmem, pipe2.src3, 1, (pipe2.istn >> 12) & 0x7);
        break;
    case 0b1101111:
    case 0b1100111:
        regFile[pipe2.src3] = PC;
        PC = pipe2.res;
        break;
    }
    regFile[0] = 0;
}

int32_t imemectrl(uint32_t pc, uint8_t *imem)
{
    return imem[pc] | (imem[pc + 1] << 8) | (imem[pc + 2] << 16) | (imem[pc + 3] << 24);
}

int64_t dmemctrl(uint32_t addr, int8_t *dmem, int64_t data, uint8_t we, uint8_t sz)
{
    if (we)
    {
        switch (sz)
        {
        case 0b00000001:
            dmem[addr] = data & 0xFF;
            break;
        case 0b00000011:
            dmem[addr] = data & 0xFF;
            dmem[addr + 1] = data >> 8 & 0xFF;
            break;
        case 0b00001111:
            dmem[addr] = data & 0xFF;
            dmem[addr + 1] = data >> 8 & 0xFF;
            dmem[addr + 2] = data >> 16 & 0xFF;
            dmem[addr + 3] = data >> 24 & 0xFF;
            break;
        case 0b11111111:
            dmem[addr] = data & 0xFF;
            dmem[addr + 1] = data >> 8 & 0xFF;
            dmem[addr + 2] = data >> 16 & 0xFF;
            dmem[addr + 3] = data >> 24 & 0xFF;
            dmem[addr + 4] = data >> 32 & 0xFF;
            dmem[addr + 5] = data >> 40 & 0xFF;
            dmem[addr + 6] = data >> 48 & 0xFF;
            dmem[addr + 7] = data >> 56 & 0xFF;
            break;
        }
        return 0;
    }
    else
    {
        switch (sz)
        {
        case 0b00000001:
            return dmem[addr];
        case 0b00000010:
            return (int64_t)dmem[addr] | (int64_t)dmem[addr + 1] << 8;
        case 0b00001000:
            return (int64_t)dmem[addr] | (int64_t)dmem[addr + 1] << 8 | (int64_t)dmem[addr + 2] << 16 | (int64_t)dmem[addr + 3] << 24;
        case 0b10000000:
            return (int64_t)dmem[addr] | (int64_t)dmem[addr + 1] << 8 | (int64_t)dmem[addr + 2] << 16 | (int64_t)dmem[addr + 3] << 24 | (int64_t)dmem[addr + 4] << 32 | (int64_t)dmem[addr + 5] << 40 | (int64_t)dmem[addr + 6] << 48 | (int64_t)dmem[addr + 7] << 56;
        }
        return 0;
    }
}