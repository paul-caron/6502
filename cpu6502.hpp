#ifndef CPU6502_HPP
#define CPU6502_HPP
#include <cstdint>
#include <vector>
#include <array>
#include <string>


class cpu6502{
public:
    cpu6502();
    struct Instruction{
        std::string Mnemonic = "";
        void (cpu6502::*operation)();
        void (cpu6502::*addressing_mode)();
        uint8_t cycles;
    };

    int cycles=0x00;
    uint8_t opcode=0x00;
    uint16_t address=0x0000;
    int8_t offset=0x00;

    uint8_t A=0x00;
    uint8_t X=0x00;
    uint8_t Y=0x00;
    uint8_t P=0x00;
    uint8_t SP=0x00;
    uint16_t PC=0x0200;
    std::string addressing_mode = "";
    std::string mnemonic ="";
    std::array<uint8_t,64*1024>memory;
    bool extra_cycle=0;

    //flags
    const uint8_t C = 0b00000001;
    const uint8_t Z = 0b00000010;
    const uint8_t I = 0b00000100;
    const uint8_t D = 0b00001000;
    const uint8_t B = 0b00010000;
    const uint8_t U = 0b00100000;
    const uint8_t V = 0b01000000;
    const uint8_t N = 0b10000000;

    //
    uint8_t read(uint16_t address);
    void write(uint16_t address, uint8_t data);

    //signals
    void emulate_cycle();
    void reset();
    void irq(); // interrupt request
    void nmi(); // non maskable interrupt

    //addressing modes
    void a();
    void abs();
    void absX();
    void absY();
    void imm();
    void imp();
    void ind();
    void xInd();
    void indY();
    void rel();
    void zpg();
    void zpgX();
    void zpgY();

    //operation codes
    void adc();
    void and_();
    void asl();
    void bcc();
    void bcs();
    void beq();
    void bit();
    void bmi();
    void bne();
    void bpl();
    void brk();
    void bvc();
    void bvs();
    void clc();
//
    void cld();
    void cli();
    void clv();
    void cmp();
    void cpx();
    void cpy();
    void dec();
    void dex();
    void dey();
    void eor();
    void inc();
    void inx();
    void iny();
    void jmp();
//
    void jsr();
    void lda();
    void ldx();
    void ldy();
    void lsr();
    void nop();
    void ora();
    void pha();
    void php();
    void pla();
    void plp();
    void rol();
    void ror();
    void rti();
//
    void rts();
    void sbc();
    void sec();
    void sed();
    void sei();
    void sta();
    void stx();
    void sty();
    void tax();
    void tay();
    void tsx();
    void txa();
    void txs();
    void tya();
//
    void illegal(); // all illegal opcodes

    //opcode table
    std::vector<Instruction> table;
    void populate_table();

};


#endif



