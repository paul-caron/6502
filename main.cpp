#include "cpu6502.hpp"
#include <cstdint>
#include <iostream>
#include <ncurses.h>
#include <sstream>

void drawRam(cpu6502 & cpu){
    move(0,0);
    printw("RAM page 0x00");
    int i=0;
    for(int y=0;y<16;++y){
    for(int x=0;x<16;++x){
        move(2+y,4*x);
        printw("%2x",cpu.read(i++));
    }
    }
}

void drawPage80(cpu6502 & cpu){
    move(19,0);
    printw("RAM page 0x80");
    int i=0x8000;
    for(int y=0;y<16;++y){
    for(int x=0;x<16;++x){
        move(21+y,4*x);
        printw("%2x",cpu.read(i++));
    }
    }
}

void drawCPU(cpu6502 & cpu){
    move(0,66);
    printw("CPU status");
    move(2,66);
    printw("%d%d%d%d%d%d%d%d", (cpu.P>>7)&1, (cpu.P>>6)&1, (cpu.P>>5)&1, (cpu.P>>4)&1, (cpu.P>>3)&1,
        (cpu.P>>2)&1, (cpu.P>>1)&1, cpu.P&1);
    move(4,66);
    printw("A    %2x",cpu.A);
    move(6,66);
    printw("X    %2x",cpu.X);
    move(8,66);
    printw("Y    %2x",cpu.Y);
    move(10,66);
    printw("SP   %2x",cpu.SP);
    move(12,66);
    printw("PC %4x",cpu.PC);
    move(14,66);
    printw("OP   %2x",cpu.opcode);
    move(16,66);
    printw("MNEMONIC  = %s", cpu.mnemonic.c_str());
    move(18,66);
    printw("ADDR MODE = %s", cpu.addressing_mode.c_str());
}


int main(){
    cpu6502 cpu = cpu6502();

    std::stringstream ss;
    ss << "A2 0A 8E 00 00 A2 03 8E 01 00 AC 00 00 A9 00 18 6D 01 00 88 D0 FA 8D 02 00 EA EA EA 4C 00 80";
    uint16_t nOffset = 0x8000;
    while (!ss.eof())
    {
	 std::string b;
	 ss >> b;
         cpu.write(nOffset++, (uint8_t)std::stoul(b, nullptr, 16));
    }

    cpu.write(0xFFFC, 0x00);
    cpu.write(0xFFFD, 0x80);
    cpu.reset();

    initscr();
    cbreak();
//    set_cursor(0);
    char c=0;
    while(c!='q'){
        clear();
        drawRam(cpu);
        drawPage80(cpu);
        drawCPU(cpu);
        move(100,80);
        refresh();
        c=getch();
        cpu.emulate_cycle();
        cpu.cycles=0;
    }
    endwin();
    return 0;
}








