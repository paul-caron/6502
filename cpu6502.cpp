#include "cpu6502.hpp"

cpu6502::cpu6502(){
    populate_table();
}

uint8_t cpu6502::read(uint16_t address){
    return memory.at(address);
}

void cpu6502::write(uint16_t address, uint8_t data){
    memory.at(address)=data;
}

void cpu6502::emulate_cycle(){
static int count = 0;
    if(cycles<=0){
        opcode = read(PC++);
        P |= U;
        cycles = table[opcode].cycles;
        (this->*table[opcode].addressing_mode)();
        mnemonic = table[opcode].Mnemonic;
        (this->*table[opcode].operation)();
        P |= U;
    }
    --cycles;
}

void cpu6502::reset(){
    A=0x00;
    X=0x00;
    Y=0x00;
    P=0x00 | U;
    SP=0xFD;
    address=0x0000;
    offset=0x00;
    cycles=8;
    uint16_t low_byte=read(0xFFFC);
    uint16_t high_byte=read(0xFFFD);
    PC = low_byte + high_byte*256;
    addressing_mode="";
    mnemonic="";
    opcode=0x00;
}

void cpu6502::irq(){
    if(!(P&I)){
        uint8_t low = PC & 0x00FF;
        uint8_t high = (PC & 0xFF00)/256;
        write(0x0100 + SP--, high);
        write(0x0100 + SP--, low);
        P |= I;
        P |= U;
        P &= ~B;
        write(0x0100 + SP--, P);
        PC = read(0xFFFE);
        PC += read(0xFFFF)*256;
        cycles = 7;
    }
}

void cpu6502::nmi(){
    uint8_t low = PC & 0x00FF;
    uint8_t high = (PC & 0xFF00)/256;
    write(0x0100 + SP--, high);
    write(0x0100 + SP--, low);
    P |= I;
    P |= U;
    P &= ~B;
    write(0x0100 + SP--, P);
    PC = read(0xFFFA);
    PC += read(0xFFFB)*256;
    cycles = 8;
}

void cpu6502::populate_table(){
    //0x00
    table.push_back({"BRK",&cpu6502::brk,&cpu6502::imp,7});
    //0x01
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::xInd,6});
    //0x02
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x03
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x04
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x05
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::zpg,3});
    //0x06
    table.push_back({"ASL",&cpu6502::asl,&cpu6502::zpg,5});
    //0x07
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x08
    table.push_back({"PHP",&cpu6502::php,&cpu6502::imp,3});
    //0x09
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::imm,2});
    //0x0A
    table.push_back({"ASL",&cpu6502::asl,&cpu6502::a,2});
    //0x0B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x0C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x0D
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::abs,4});
    //0x0E
    table.push_back({"ASL",&cpu6502::asl,&cpu6502::abs,6});
    //0x0F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x10
    table.push_back({"BPL",&cpu6502::bpl,&cpu6502::rel,2});
    //0x11
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::indY,5});
    //0x12
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x13
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x14
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x15
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::zpgX,4});
    //0x16
    table.push_back({"ASL",&cpu6502::asl,&cpu6502::zpgX,6});
    //0x17
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x18
    table.push_back({"CLC",&cpu6502::clc,&cpu6502::imp,2});
    //0x19
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::absY,4});
    //0x1A
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x1B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x1C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x1D
    table.push_back({"ORA",&cpu6502::ora,&cpu6502::absX,4});
    //0x1E
    table.push_back({"ASL",&cpu6502::asl,&cpu6502::absX,7});
    //0x1F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x20
    table.push_back({"JSR",&cpu6502::jsr,&cpu6502::abs,6});
    //0x21
    table.push_back({"AND",&cpu6502::and_,&cpu6502::xInd,6});
    //0x22
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x23
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x24
    table.push_back({"BIT",&cpu6502::bit,&cpu6502::zpg,3});
    //0x25
    table.push_back({"AND",&cpu6502::and_,&cpu6502::zpg,3});
    //0x26
    table.push_back({"ROL",&cpu6502::rol,&cpu6502::zpg,5});
    //0x27
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x28
    table.push_back({"PLP",&cpu6502::plp,&cpu6502::imp,4});
    //0x29
    table.push_back({"AND",&cpu6502::and_,&cpu6502::imm,2});
    //0x2A
    table.push_back({"ROL",&cpu6502::rol,&cpu6502::a,2});
    //0x2B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x2C
    table.push_back({"BIT",&cpu6502::bit,&cpu6502::abs,4});
    //0x2D
    table.push_back({"AND",&cpu6502::and_,&cpu6502::abs,4});
    //0x2E
    table.push_back({"ROL",&cpu6502::rol,&cpu6502::abs,6});
    //0x2F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x30
    table.push_back({"BMI",&cpu6502::bmi,&cpu6502::rel,2});
    //0x31
    table.push_back({"AND",&cpu6502::and_,&cpu6502::indY,5});
    //0x32
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x33
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x34
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x35
    table.push_back({"AND",&cpu6502::and_,&cpu6502::zpgX,4});
    //0x36
    table.push_back({"ROL",&cpu6502::rol,&cpu6502::zpgX,6});
    //0x37
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x38
    table.push_back({"SEC",&cpu6502::sec,&cpu6502::imp,2});
    //0x39
    table.push_back({"AND",&cpu6502::and_,&cpu6502::absY,4});
    //0x3A
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x3B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x3C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x3D
    table.push_back({"AND",&cpu6502::and_,&cpu6502::absX,4});
    //0x3E
    table.push_back({"ROL",&cpu6502::rol,&cpu6502::absX,7});
    //0x3F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x40
    table.push_back({"RTI",&cpu6502::rti,&cpu6502::imp,6});
    //0x41
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::xInd,6});
    //0x42
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x43
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x44
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x45
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::zpg,3});
    //0x46
    table.push_back({"LSR",&cpu6502::lsr,&cpu6502::zpg,5});
    //0x47
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x48
    table.push_back({"PHA",&cpu6502::pha,&cpu6502::imp,3});
    //0x49
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::imm,2});
    //0x4A
    table.push_back({"LSR",&cpu6502::lsr,&cpu6502::a,2});
    //4B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //4C
    table.push_back({"JMP",&cpu6502::jmp,&cpu6502::abs,3});
    //4D
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::abs,4});
    //4E
    table.push_back({"LSR",&cpu6502::lsr,&cpu6502::abs,6});
    //4F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //50
    table.push_back({"BVC",&cpu6502::bvc,&cpu6502::rel,2});
    //51
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::indY,5});
    //52
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //53
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //54
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //55
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::zpgX,4});
    //56
    table.push_back({"LSR",&cpu6502::lsr,&cpu6502::zpgX,6});
    //0x57
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x58
    table.push_back({"CLI",&cpu6502::cli,&cpu6502::imp,2});
    //0x59
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::absY,4});
    //5A
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //5B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x5C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x5D
    table.push_back({"EOR",&cpu6502::eor,&cpu6502::absX,4});
    //0x5E
    table.push_back({"LSR",&cpu6502::lsr,&cpu6502::absX,7});
    //0x5F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x60
    table.push_back({"RTS",&cpu6502::rts,&cpu6502::imp,6});
    //0x61
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::xInd,6});
    //0x62
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x63
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x64
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x65
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::zpg,3});
    //0x66
    table.push_back({"ROR",&cpu6502::ror,&cpu6502::zpg,5});
    //0x67
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x68
    table.push_back({"PLA",&cpu6502::pla,&cpu6502::imp,4});
    //0x69
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::imm,2});
    //0x6A
    table.push_back({"ROR",&cpu6502::ror,&cpu6502::a,2});
    //0x6B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x6C
    table.push_back({"JMP",&cpu6502::jmp,&cpu6502::ind,5});
    //0x6D
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::abs,4});
    //0x6E
    table.push_back({"ROR",&cpu6502::ror,&cpu6502::abs,6});
    //0x6F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x70
    table.push_back({"BVS",&cpu6502::bvs,&cpu6502::rel,2});
    //0x71
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::indY,5});
    //0x72
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x73
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x74
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x75
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::zpgX,4});
    //0x76
    table.push_back({"ROR",&cpu6502::ror,&cpu6502::zpgX,6});
    //0x77
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x78
    table.push_back({"SEI",&cpu6502::sei,&cpu6502::imp,2});
    //0x79
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::absY,4});
    //0x7A
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x7B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x7C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //7D
    table.push_back({"ADC",&cpu6502::adc,&cpu6502::absX,4});
    //7E
    table.push_back({"ROR",&cpu6502::ror,&cpu6502::absX,7});
    //7F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x80
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x81
    table.push_back({"STA",&cpu6502::sta,&cpu6502::xInd,6});
    //0x82
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x83
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x84
    table.push_back({"STY",&cpu6502::sty,&cpu6502::zpg,3});
    //0x85
    table.push_back({"STA",&cpu6502::sta,&cpu6502::zpg,3});
    //0x86
    table.push_back({"STX",&cpu6502::stx,&cpu6502::zpg,3});
    //0x87
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x88
    table.push_back({"DEY",&cpu6502::dey,&cpu6502::imp,2});
    //0x89
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x8A
    table.push_back({"TXA",&cpu6502::txa,&cpu6502::imp,2});
    //0x8B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x8C
    table.push_back({"STY",&cpu6502::sty,&cpu6502::abs,4});
    //0x8D
    table.push_back({"STA",&cpu6502::sta,&cpu6502::abs,4});
    //0x8E
    table.push_back({"STX",&cpu6502::stx,&cpu6502::abs,4});
    //0x8F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x90
    table.push_back({"BCC",&cpu6502::bcc,&cpu6502::rel,2});
    //0x91
    table.push_back({"STA",&cpu6502::sta,&cpu6502::indY,6});
    //0x92
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x93
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x94
    table.push_back({"STY",&cpu6502::sty,&cpu6502::zpgX,4});
    //0x95
    table.push_back({"STA",&cpu6502::sta,&cpu6502::zpgX,4});
    //0x96
    table.push_back({"STX",&cpu6502::stx,&cpu6502::zpgY,4});
    //0x97
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x98
    table.push_back({"TYA",&cpu6502::tya,&cpu6502::imp,2});
    //0x99
    table.push_back({"STA",&cpu6502::sta,&cpu6502::absY,5});
    //0x9A
    table.push_back({"TXS",&cpu6502::txs,&cpu6502::imp,2});
    //0x9B
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x9C
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x9D
    table.push_back({"STA",&cpu6502::sta,&cpu6502::absX,5});
    //0x9E
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0x9F
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //A0
    table.push_back({"LDY",&cpu6502::ldy,&cpu6502::imm,2});
    //A1
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::xInd,6});
    //A2
    table.push_back({"LDX",&cpu6502::ldx,&cpu6502::imm,2});
    //A3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //A4
    table.push_back({"LDY",&cpu6502::ldy,&cpu6502::zpg,3});
    //A5
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::zpg,3});
    //A6
    table.push_back({"LDX",&cpu6502::ldx,&cpu6502::zpg,3});
    //A7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //A8
    table.push_back({"TAY",&cpu6502::tay,&cpu6502::imp,2});
   //0xA9
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::imm,2});
    //0xAA
    table.push_back({"TAX",&cpu6502::tax,&cpu6502::imp,2});
    //0xAB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xAC
    table.push_back({"LDY",&cpu6502::ldy,&cpu6502::abs,4});
    //0xAD
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::abs,4});
    //0xAE
    table.push_back({"LDX",&cpu6502::ldx,&cpu6502::abs,4});
    //0xAF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xB0
    table.push_back({"BCS",&cpu6502::bcs,&cpu6502::rel,2});
    //0xB1
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::indY,5});
    //0xB2
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xB3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xB4
    table.push_back({"LDY",&cpu6502::ldy,&cpu6502::zpgX,4});
    //0xB5
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::zpgX,4});
    //0xB6
    table.push_back({"LDX",&cpu6502::ldx,&cpu6502::zpgY,4});
    //0xB7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xB8
    table.push_back({"CLV",&cpu6502::clv,&cpu6502::imp,2});
    //0xB9
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::absY,4});
    //0xBA
    table.push_back({"TSX",&cpu6502::tsx,&cpu6502::imp,2});
    //0xBB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xBC
    table.push_back({"LDY",&cpu6502::ldy,&cpu6502::absX,4});
    //0xBD
    table.push_back({"LDA",&cpu6502::lda,&cpu6502::absX,4});
    //0xBE
    table.push_back({"LDX",&cpu6502::ldx,&cpu6502::absY,4});
    //0xBF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xC0
    table.push_back({"CPY",&cpu6502::cpy,&cpu6502::imm,2});
    //0xC1
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::xInd,6});
    //0xC2
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xC3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xC4
    table.push_back({"CPY",&cpu6502::cpy,&cpu6502::zpg,3});
    //0xC5
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::zpg,3});
    //0xC6
    table.push_back({"DEC",&cpu6502::dec,&cpu6502::zpg,5});
    //0xC7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xC8
    table.push_back({"INY",&cpu6502::iny,&cpu6502::imp,2});
    //0xC9
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::imm,2});
    //0xCA
    table.push_back({"DEX",&cpu6502::dex,&cpu6502::imp,2});
    //0xCB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xCC
    table.push_back({"CPY",&cpu6502::cpy,&cpu6502::abs,4});
    //0xCD
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::abs,4});
    //0xCE
    table.push_back({"DEC",&cpu6502::dec,&cpu6502::abs,6});
    //0xCF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xD0
    table.push_back({"BNE",&cpu6502::bne,&cpu6502::rel,2});
    //0xD1
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::indY,5});
    //0xD2
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xD3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xD4
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xD5
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::zpgX,4});
    //0xD6
    table.push_back({"DEC",&cpu6502::dec,&cpu6502::zpgX,6});
    //0xD7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xD8
    table.push_back({"CLD",&cpu6502::cld,&cpu6502::imp,2});
    //0xD9
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::absY,4});
    //0xDA
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xDB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xDC
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xDD
    table.push_back({"CMP",&cpu6502::cmp,&cpu6502::absX,4});
    //0xDE
    table.push_back({"DEC",&cpu6502::dec,&cpu6502::absX,7});
    //0xDF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xE0
    table.push_back({"CPX",&cpu6502::cpx,&cpu6502::imm,2});
    //0xE1
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::xInd,6});
    //0xE2
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xE3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xE4
    table.push_back({"CPX",&cpu6502::cpx,&cpu6502::zpg,3});
    //0xE5
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::zpg,3});
    //0xE6
    table.push_back({"INC",&cpu6502::inc,&cpu6502::zpg,5});
    //0xE7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xE8
    table.push_back({"INX",&cpu6502::inx,&cpu6502::imp,2});
    //0xE9
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::imm,2});
    //0xEA
    table.push_back({"NOP",&cpu6502::nop,&cpu6502::imp,2});
    //0xEB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xEC
    table.push_back({"CPX",&cpu6502::cpx,&cpu6502::abs,4});
    //0xED
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::abs,4});
    //0xEE
    table.push_back({"INC",&cpu6502::inc,&cpu6502::abs,6});
    //0xEF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xF0
    table.push_back({"BEQ",&cpu6502::beq,&cpu6502::rel,2});
    //0xF1
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::indY,5});
    //0xF2
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xF3
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xF4
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xF5
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::zpgX,4});
    //0xF6
    table.push_back({"INC",&cpu6502::inc,&cpu6502::zpgX,6});
    //0xF7
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xF8
    table.push_back({"SED",&cpu6502::sed,&cpu6502::imp,2});
    //0xF9
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::absY,4});
    //0xFA
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xFB
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xFC
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});
    //0xFD
    table.push_back({"SBC",&cpu6502::sbc,&cpu6502::absX,4});
    //0xFE
    table.push_back({"INC",&cpu6502::inc,&cpu6502::absX,7});
    //0xFF
    table.push_back({"ILL",&cpu6502::illegal,&cpu6502::imp,0});




}


//addressing modes

void cpu6502::a(){
    addressing_mode = "A";
    return;
}

void cpu6502::abs(){
    addressing_mode = "ABS";
    address = read(PC++);
    address += read(PC++) << 8;
}

void cpu6502::absX(){
    addressing_mode = "ABSX";
    address = read(PC++);
    address += read(PC++) << 8;
    address += X;
}

void cpu6502::absY(){
    addressing_mode = "ABSY";
    address = read(PC++);
    address += read(PC++) << 8;
    address += Y;
}

void cpu6502::imm(){
    addressing_mode = "IMM";
    address = PC++;
}

void cpu6502::indY(){
    addressing_mode = "INDY";
    uint16_t ptr = read(PC++);
    uint16_t ptr_lo = read(ptr&0x00FF);
    uint16_t ptr_hi = read((ptr+1)&0x00FF);
    address = read(ptr_lo);
    address += read(ptr_hi) << 8;
    address += Y;
}

void cpu6502::imp(){
    addressing_mode = "IMP";
    return;
}

void cpu6502::ind(){
    addressing_mode = "IND";
    uint16_t ptr = read(PC++);
    ptr += read(PC++) << 8;
    if((ptr&0x00FF)==0x00FF){
        address = read(ptr);
        address += read(ptr & 0xFF00) << 8;
    }else{
        address = read(ptr);
        address += read(ptr+1) << 8;
    }
}

void cpu6502::rel(){
    addressing_mode = "REL";
    offset = read(PC++);
}

void cpu6502::xInd(){
    addressing_mode = "XIND";
    uint16_t ptr = read(PC);
    uint16_t ptr_lo = read((ptr+uint16_t(X))&0x00FF);
    uint16_t ptr_hi = read((ptr+uint16_t(X+1))&0x00FF);
    address = read(ptr_lo);
    address += read(ptr_hi) << 8;
}

void cpu6502::zpg(){
    addressing_mode = "ZPG";
    address = read(PC++) & 0x00FF;
}

void cpu6502::zpgX(){
    addressing_mode = "ZPGX";
    address = (read(PC++)+X) & 0x00FF;
}

void cpu6502::zpgY(){
    addressing_mode = "ZPGY";
    address = (read(PC++)+Y) & 0x00FF;
}

//operations
void cpu6502::adc(){
    uint16_t result = uint16_t(A) + uint16_t(read(address)) + uint16_t((P&C)!=0);
    if(result > 0x00FF) P |= C;
    else P &= ~C;
    if(result == 0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;

    if((A&0x80)&&(read(address)&0x80)&&(!(result&0x0080))) P |= V;
    else if((!(A&0x80))&&(!(read(address)&0x80))&&(result&0x0080)) P |= V;
    else P &= ~V;
    A = result & 0x00FF;
}

void cpu6502::and_(){
    A &= read(address);
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::asl(){
    uint16_t result = uint16_t(read(address)) << 1;
    if(result > 255) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result&0x80) P |= N;
    else P &= ~N;
    if(addressing_mode == "A" || addressing_mode == "IMP") A = result & 0x00FF;
    else write(address, result & 0x00FF);
}

void cpu6502::bcc(){
    if(!(P&C)){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::bcs(){
    if(P&C){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::beq(){
    if(P&Z){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::bit(){
    uint8_t fetched = read(address);
    uint8_t result = A & fetched;
    if(result==0) P |= Z;
    else P &= ~Z;
    if(fetched&N) P |= N;
    else P &= ~N;
    if(fetched&V) P |= V;
    else P &= ~V;
}

void cpu6502::bmi(){
    if(P&N){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::bne(){
    if(!(P&Z)){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::bpl(){
    if(!(P&N)){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::brk(){
    ++ PC;
    P |= I;
    write(0x0100 + SP--, (PC >> 8) & 0x00FF);
    write(0x0100 + SP--, PC & 0x00FF);
    P |= B;
    write(0x0100 + SP--, P);
    P &= ~B;
    PC = read(0xFFFE);
    PC += uint16_t(read(0xFFFF)) << 8;
}

void cpu6502::bvc(){
    if(!(P&V)){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::bvs(){
    if(P&V){
        ++cycles;
        address = PC + offset;
        if((address & 0xFF00) != (PC & 0xFF00))
            ++cycles;
        PC = address;
    }
}

void cpu6502::clc(){
    P &= ~C;
}

void cpu6502::cld(){
    P &= ~D;
}

void cpu6502::cli(){
    P &= ~I;
}

void cpu6502::clv(){
    P &= ~V;
}

void cpu6502::cmp(){
    uint16_t result = uint16_t(A) - uint16_t(read(address));
    if(A >= read(address)) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
}

void cpu6502::cpx(){
    uint16_t result = uint16_t(X) - uint16_t(read(address));
    if(A >= read(address)) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
}

void cpu6502::cpy(){
    uint16_t result = uint16_t(Y) - uint16_t(read(address));
    if(A >= read(address)) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
}

void cpu6502::dec(){
    uint16_t result = read(address) - 1;
    write(address, result & 0x00FF);
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
}

void cpu6502::dex(){
    --X;
    if(!X) P |= Z;
    else P &= ~Z;
    if(X & 0x80) P |= N;
    else P &= ~N;
}

void cpu6502::dey(){
    --Y;
    if(!Y) P |= Z;
    else P &= ~Z;
    if(Y & 0x80) P |= N;
    else P &= ~N;
}

void cpu6502::eor(){
    A ^= read(address);
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::illegal(){
    return;
}

void cpu6502::inc(){
    uint16_t result = read(address) + 1;
    write(address, result & 0x00FF);
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
}

void cpu6502::inx(){
    ++X;
    if(!X) P |= Z;
    else P &= ~Z;
    if(X & 0x80) P |= N;
    else P &= ~N;
}

void cpu6502::iny(){
    ++Y;
    if(!Y) P |= Z;
    else P &= ~Z;
    if(Y & 0x80) P |= N;
    else P &= ~N;
}

void cpu6502::jmp(){
    PC = address;
}

void cpu6502::jsr(){
    --PC;
    write(0x0100 + SP--, (PC >> 8) & 0x00FF);
    write(0x0100 + SP--, PC & 0x00FF);
    PC = address;
}

void cpu6502::lda(){
    A = read(address);
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::ldx(){
    X = read(address);
    if(X==0) P |= Z;
    else P &= ~Z;
    if(X&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::ldy(){
    Y = read(address);
    if(Y==0) P |= Z;
    else P &= ~Z;
    if(Y&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::lsr(){
    uint16_t result = uint16_t(read(address));
    if(result &0x01) P |= C;
    else P &= ~C;

    result >>= 1;

    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result&0x80) P |= N;
    else P &= ~N;
    if(addressing_mode == "A" || addressing_mode == "IMP") A = result & 0x00FF;
    else write(address, result & 0x00FF);
}

void cpu6502::nop(){
    return;
}

void cpu6502::ora(){
    A |= read(address);
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::pha(){
    write(0x0100+SP--,A);
}

void cpu6502::php(){
    write(0x0100+SP--,P|B|U);
    P &= ~U;
    P &= ~B;
}

void cpu6502::pla(){
    ++ SP;
    A = read(0x0100 + SP);
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::plp(){
    ++ SP;
    P = read(0x0100+SP);
    P |= U;
}

void cpu6502::rol(){
    uint16_t result = (uint16_t(read(address))<< 1) | uint16_t((P&C)==1);
    if(result & 0xFF00) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
    if(addressing_mode == "A" || addressing_mode=="IMP")
        A= result & 0x00FF;
    else write(address, result & 0x00FF);
}

void cpu6502::ror(){
    uint16_t result = uint16_t(read(address)>>1) | uint16_t(((P&C)==1)<<7);
    if(read(address) & 0x01) P |= C;
    else P &= ~C;
    if((result&0x00FF)==0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;
    if(addressing_mode == "A" || addressing_mode=="IMP")
        A= result & 0x00FF;
    else write(address, result & 0x00FF);
}

void cpu6502::rti(){
    ++ SP;
    P = read(0x0100 + SP);
    P &= ~B;
    P &= ~U;
    ++ SP;
    PC = uint16_t(read(0x0100+SP++));
    PC |= uint16_t(read(0x0100+SP)) << 8;
}

void cpu6502::rts(){
    ++ SP;
    PC = uint16_t(read(0x0100+SP++));
    PC |= uint16_t(read(0x0100+SP)) << 8;
    ++PC;
}

void cpu6502::sbc(){
    uint16_t value = uint16_t(read(address)) ^ 0x00FF;
    uint16_t result = uint16_t(A) + value + uint16_t((P&C)!=0);
    if(result > 0x00FF) P |= C;
    else P &= ~C;
    if(result == 0) P |= Z;
    else P &= ~Z;
    if(result & 0x0080) P |= N;
    else P &= ~N;

    if((A&0x80)&&(value&0x80)&&(!(result&0x0080))) P |= V;
    else if((!(A&0x80))&&(!(value&0x80))&&(result&0x0080)) P |= V;
    else P &= ~V;
    A = result & 0x00FF;
}

void cpu6502::sec(){
    P |= C;
}

void cpu6502::sed(){
    P |= D;
}

void cpu6502::sei(){
    P |= I;
}

void cpu6502::sta(){
    write(address,A);
}

void cpu6502::stx(){
    write(address,X);
}

void cpu6502::sty(){
    write(address,Y);
}

void cpu6502::tax(){
    X = A;
    if(X==0) P |= Z;
    else P &= ~Z;
    if(X&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::tay(){
    Y = A;
    if(Y==0) P |= Z;
    else P &= ~Z;
    if(Y&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::tsx(){
    X = SP;
    if(X==0) P |= Z;
    else P &= ~Z;
    if(X&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::txa(){
    A = X;
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::txs(){
    SP = X;
    if(SP==0) P |= Z;
    else P &= ~Z;
    if(SP&0x80) P |= N;
    else P &= ~N;
}

void cpu6502::tya(){
    A = Y;
    if(A==0) P |= Z;
    else P &= ~Z;
    if(A&0x80) P |= N;
    else P &= ~N;
}









