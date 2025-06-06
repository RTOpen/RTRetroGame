/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#ifdef DEBUGGER

#include <stdarg.h>
#include "snes9x.h"
#include "memmap.h"
#include "dma.h"
#include "apu/apu.h"
#include "display.h"
#include "debug.h"
#include "missing.h"

// #include "apu/apu.hpp"

extern SDMA	DMA[8];
extern FILE	*apu_trace;
FILE		*trace = NULL, *trace2 = NULL;

struct SBreakPoint	S9xBreakpoint[6];

struct SDebug
{
	struct
	{
		uint8	Bank;
		uint16	Address;
	}	Dump;

	struct
	{
		uint8	Bank;
		uint16	Address;
	}	Unassemble;
};

static struct SDebug	Debug = { { 0, 0 }, { 0, 0 } };

static const char	*HelpMessage[] =
{
	"Command Help:",
	"?, help                - Shows this command help",
	"r                      - Shows the registers",
	"i                      - Shows the interrupt vectors",
	"t                      - Trace current instruction   [step-into]",
	"p                      - Proceed to next instruction [step-over]",
	"s                      - Skip to next instruction    [skip]",
	"T                      - Toggle CPU instruction tracing to trace.log",
	"TS                     - Toggle SA-1 instruction tracing to trace_sa1.log",
	"E                      - Toggle HC-based event tracing to trace.log",
	"V                      - Toggle non-DMA V-RAM read/write tracing to stdout",
	"D                      - Toggle on-screen DMA tracing",
	"H                      - Toggle on-screen HDMA tracing",
	"U                      - Toggle on-screen unknown register read/write tracing",
	"P                      - Toggle on-screen DSP tracing",
	"S                      - Dump sprite (OBJ) status",
	"g [Address]            - Go or go to [Address]",
	"u [Address]            - Disassemble from PC or [Address]",
	"d [Address]            - Dump from PC or [Address]",
	"bv [Number]            - View breakpoints or view breakpoint [Number]",
	"bs [Number] [Address]  - Enable/disable breakpoint",
	"                         [enable example: bs #2 $02:8002]",
	"                         [disable example: bs #2]",
	"c                      - Dump SNES colour palette",
	"W                      - Show what SNES hardware features the ROM is using",
	"                         which might not be implemented yet",
	"w                      - Show some SNES hardware features used so far in this frame",
	"R                      - Reset SNES",
	"q                      - Quit emulation",
//	"ai                     - Shou APU vectors",
//	"a                      - Show APU status",
//	"x                      - Show Sound DSP status",
	"A                      - Toggle APU instruction tracing to trace.log",
//	"B                      - Toggle sound DSP register tracing to aputrace.log",
//	"C                      - Dump sound sample addresses",
//	"ad [Address]           - Dump APU RAM from PC or [Address]",
	"",
	"[Address]              - $Bank:Address or $Address",
	"                         [for example: $01:8123]",
	"[Number]               - #Number",
	"                         [for example: #1]",
	"z                      - ",
	"f                      - ",
	"dump                   - ",
	"",
	NULL
};

static const char	*S9xMnemonics[256] =
{
	"BRK", "ORA", "COP", "ORA", "TSB", "ORA", "ASL", "ORA",
	"PHP", "ORA", "ASL", "PHD", "TSB", "ORA", "ASL", "ORA",
	"BPL", "ORA", "ORA", "ORA", "TRB", "ORA", "ASL", "ORA",
	"CLC", "ORA", "INC", "TCS", "TRB", "ORA", "ASL", "ORA",
	"JSR", "AND", "JSL", "AND", "BIT", "AND", "ROL", "AND",
	"PLP", "AND", "ROL", "PLD", "BIT", "AND", "ROL", "AND",
	"BMI", "AND", "AND", "AND", "BIT", "AND", "ROL", "AND",
	"SEC", "AND", "DEC", "TSC", "BIT", "AND", "ROL", "AND",
	"RTI", "EOR", "WDM", "EOR", "MVP", "EOR", "LSR", "EOR",
	"PHA", "EOR", "LSR", "PHK", "JMP", "EOR", "LSR", "EOR",
	"BVC", "EOR", "EOR", "EOR", "MVN", "EOR", "LSR", "EOR",
	"CLI", "EOR", "PHY", "TCD", "JMP", "EOR", "LSR", "EOR",
	"RTS", "ADC", "PER", "ADC", "STZ", "ADC", "ROR", "ADC",
	"PLA", "ADC", "ROR", "RTL", "JMP", "ADC", "ROR", "ADC",
	"BVS", "ADC", "ADC", "ADC", "STZ", "ADC", "ROR", "ADC",
	"SEI", "ADC", "PLY", "TDC", "JMP", "ADC", "ROR", "ADC",
	"BRA", "STA", "BRL", "STA", "STY", "STA", "STX", "STA",
	"DEY", "BIT", "TXA", "PHB", "STY", "STA", "STX", "STA",
	"BCC", "STA", "STA", "STA", "STY", "STA", "STX", "STA",
	"TYA", "STA", "TXS", "TXY", "STZ", "STA", "STZ", "STA",
	"LDY", "LDA", "LDX", "LDA", "LDY", "LDA", "LDX", "LDA",
	"TAY", "LDA", "TAX", "PLB", "LDY", "LDA", "LDX", "LDA",
	"BCS", "LDA", "LDA", "LDA", "LDY", "LDA", "LDX", "LDA",
	"CLV", "LDA", "TSX", "TYX", "LDY", "LDA", "LDX", "LDA",
	"CPY", "CMP", "REP", "CMP", "CPY", "CMP", "DEC", "CMP",
	"INY", "CMP", "DEX", "WAI", "CPY", "CMP", "DEC", "CMP",
	"BNE", "CMP", "CMP", "CMP", "PEI", "CMP", "DEC", "CMP",
	"CLD", "CMP", "PHX", "STP", "JML", "CMP", "DEC", "CMP",
	"CPX", "SBC", "SEP", "SBC", "CPX", "SBC", "INC", "SBC",
	"INX", "SBC", "NOP", "XBA", "CPX", "SBC", "INC", "SBC",
	"BEQ", "SBC", "SBC", "SBC", "PEA", "SBC", "INC", "SBC",
	"SED", "SBC", "PLX", "XCE", "JSR", "SBC", "INC", "SBC"
};

static int	AddrModes[256] =
{
  // 0   1   2   3   4   5   6   7   8   9   A   B   C   D   E   F
	 3, 10,  3, 19,  6,  6,  6, 12,  0,  1, 24,  0, 14, 14, 14, 17, // 0
	 4, 11,  9, 20,  6,  7,  7, 13,  0, 16, 24,  0, 14, 15, 15, 18, // 1
	14, 10, 17, 19,  6,  6,  6, 12,  0,  1, 24,  0, 14, 14, 14, 17, // 2
	 4, 11,  9, 20,  7,  7,  7, 13,  0, 16, 24,  0, 15, 15, 15, 18, // 3
	 0, 10,  3, 19, 25,  6,  6, 12,  0,  1, 24,  0, 14, 14, 14, 17, // 4
	 4, 11,  9, 20, 25,  7,  7, 13,  0, 16,  0,  0, 17, 15, 15, 18, // 5
	 0, 10,  5, 19,  6,  6,  6, 12,  0,  1, 24,  0, 21, 14, 14, 17, // 6
	 4, 11,  9, 20,  7,  7,  7, 13,  0, 16,  0,  0, 23, 15, 15, 18, // 7
	 4, 10,  5, 19,  6,  6,  6, 12,  0,  1,  0,  0, 14, 14, 14, 17, // 8
	 4, 11,  9, 20,  7,  7,  8, 13,  0, 16,  0,  0, 14, 15, 15, 18, // 9
	 2, 10,  2, 19,  6,  6,  6, 12,  0,  1,  0,  0, 14, 14, 14, 17, // A
	 4, 11,  9, 20,  7,  7,  8, 13,  0, 16,  0,  0, 15, 15, 16, 18, // B
	 2, 10,  3, 19,  6,  6,  6, 12,  0,  1,  0,  0, 14, 14, 14, 17, // C
	 4, 11,  9,  9, 27,  7,  7, 13,  0, 16,  0,  0, 22, 15, 15, 18, // D
	 2, 10,  3, 19,  6,  6,  6, 12,  0,  1,  0,  0, 14, 14, 14, 17, // E
	 4, 11,  9, 20, 26,  7,  7, 13,  0, 16,  0,  0, 23, 15, 15, 18  // F
};

static uint8 S9xDebugGetByte (uint32);
static uint16 S9xDebugGetWord (uint32);
static uint8 S9xDebugSA1GetByte (uint32);
static uint16 S9xDebugSA1GetWord (uint32);
static uint8 debug_cpu_op_print (char *, uint8, uint16);
static uint8 debug_sa1_op_print (char *, uint8, uint16);
static void debug_line_print (const char *);
static int debug_get_number (char *, uint16 *);
static short debug_get_start_address (char *, uint8 *, uint32 *);
static void debug_print_window (uint8 *);
static const char * debug_clip_fn (int);
static void debug_whats_used (void);
static void debug_whats_missing (void);


static uint8 S9xDebugGetByte (uint32 Address)
{
	int		block = (Address & 0xffffff) >> MEMMAP_SHIFT;
	uint8	*GetAddress = Memory.ReadMap[block];
	uint8	byte = 0;

	if (GetAddress >= (uint8 *) CMemory::MAP_LAST)
	{
		byte = *(GetAddress + (Address & 0xffff));
		return (byte);
	}

	switch ((pint) GetAddress)
	{
		case CMemory::MAP_LOROM_SRAM:
			byte = *(Memory.SRAM + ((((Address & 0xff0000) >> 1) | (Address & 0x7fff)) & Memory.SRAMMask));
			return (byte);

		case CMemory::MAP_HIROM_SRAM:
		case CMemory::MAP_RONLY_SRAM:
			byte = *(Memory.SRAM + (((Address & 0x7fff) - 0x6000 + ((Address & 0xf0000) >> 3)) & Memory.SRAMMask));
			return (byte);

		default:
			return (byte);
	}
}

static uint16 S9xDebugGetWord (uint32 Address)
{
	uint16	word;

	word  = S9xDebugGetByte(Address);
	word |= S9xDebugGetByte(Address + 1) << 8;

	return (word);
}

static uint8 debug_cpu_op_print (char *Line, uint8 Bank, uint16 Address)
{
	uint8	S9xOpcode;
	uint8	Operant[3];
	uint16	Word;
	uint8	Byte;
	int16	SWord;
	int8	SByte;
	uint8	Size = 0;

	S9xOpcode = S9xDebugGetByte((Bank << 16) + Address);
	sprintf(Line, "$%02X:%04X %02X ", Bank, Address, S9xOpcode);

	Operant[0] = S9xDebugGetByte((Bank << 16) + Address + 1);
	Operant[1] = S9xDebugGetByte((Bank << 16) + Address + 2);
	Operant[2] = S9xDebugGetByte((Bank << 16) + Address + 3);

	switch (AddrModes[S9xOpcode])
	{
		case 0:
			// Implied
			sprintf(Line, "%s         %s",
					Line,
					S9xMnemonics[S9xOpcode]);
			Size = 1;
			break;

		case 1:
			// Immediate[MemoryFlag]
			if (!CheckFlag(MemoryFlag))
			{
				// Accumulator 16 - Bit
				sprintf(Line, "%s%02X %02X    %s #$%02X%02X",
						Line,
						Operant[0],
						Operant[1],
						S9xMnemonics[S9xOpcode],
						Operant[1],
						Operant[0]);
				Size = 3;
			}
			else
			{
				// Accumulator 8 - Bit
				sprintf(Line, "%s%02X       %s #$%02X",
						Line,
						Operant[0],
						S9xMnemonics[S9xOpcode],
						Operant[0]);
				Size = 2;
			}

			break;

		case 2:
			// Immediate[IndexFlag]
			if (!CheckFlag(IndexFlag))
			{
				// X / Y 16 - Bit
				sprintf(Line, "%s%02X %02X    %s #$%02X%02X",
				        Line,
				        Operant[0],
				        Operant[1],
				        S9xMnemonics[S9xOpcode],
				        Operant[1],
				        Operant[0]);
				Size = 3;
			}
			else
			{
				// X / Y 8 - Bit
				sprintf(Line, "%s%02X       %s #$%02X",
				        Line,
				        Operant[0],
				        S9xMnemonics[S9xOpcode],
				        Operant[0]);
				Size = 2;
			}

			break;

		case 3:
			// Immediate[Always 8 - Bit]
			sprintf(Line, "%s%02X       %s #$%02X",
					Line,
					Operant[0],
					S9xMnemonics[S9xOpcode],
					Operant[0]);
			Size = 2;
			break;

		case 4:
			// Relative
			sprintf(Line, "%s%02X       %s $%02X",
					Line,
					Operant[0],
					S9xMnemonics[S9xOpcode],
					Operant[0]);
			SByte = Operant[0];
			Word = Address;
			Word += SByte;
			Word += 2;
			sprintf(Line, "%-32s[$%04X]", Line, Word);
			Size = 2;
			break;

		case 5:
			// Relative Long
			sprintf(Line, "%s%02X %02X    %s $%02X%02X",
					Line,
					Operant[0],
					Operant[1],
					S9xMnemonics[S9xOpcode],
					Operant[1],
					Operant[0]);
			SWord = (Operant[1] << 8) | Operant[0];
			Word = Address;
			Word += SWord;
			Word += 3;
			sprintf(Line, "%-32s[$%04X]", Line, Word);
			Size = 3;
			break;

		case 6:
			// Direct
			sprintf(Line, "%s%02X       %s $%02X",
					Line,
					Operant[0],
					S9xMnemonics[S9xOpcode],
					Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			sprintf(Line, "%-32s[$00:%04X]", Line, Word);
			Size = 2;
			break;

		case 7:
			// Direct Indexed (with X)
			sprintf(Line, "%s%02X       %s $%02X,x",
					Line,
					Operant[0],
					S9xMnemonics[S9xOpcode],
					Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word += Registers.X.W;
			sprintf(Line, "%-32s[$00:%04X]", Line, Word);
			Size = 2;
			break;

		case 8:
			// Direct Indexed (with Y)
			sprintf(Line, "%s%02X       %s $%02X,y",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word += Registers.Y.W;
			sprintf(Line, "%-32s[$00:%04X]", Line, Word);
			Size = 2;
			break;

		case 9:
			// Direct Indirect
			sprintf(Line, "%s%02X       %s ($%02X)",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 2;
			break;

		case 10:
			// Direct Indexed Indirect
			sprintf(Line, "%s%02X       %s ($%02X,x)",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word += Registers.X.W;
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 2;
			break;

		case 11:
			// Direct Indirect Indexed
			sprintf(Line, "%s%02X       %s ($%02X),y",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word = S9xDebugGetWord(Word);
			Word += Registers.Y.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 2;
			break;

		case 12:
			// Direct Indirect Long
			sprintf(Line, "%s%02X       %s [$%02X]",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Byte = S9xDebugGetByte(Word + 2);
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Byte, Word);
			Size = 2;
			break;

		case 13:
			// Direct Indirect Indexed Long
			sprintf(Line, "%s%02X       %s [$%02X],y",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Byte = S9xDebugGetByte(Word + 2);
			Word = S9xDebugGetWord(Word);
			Word += Registers.Y.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Byte, Word);
			Size = 2;
			break;

		case 14:
			// Absolute
			sprintf(Line, "%s%02X %02X    %s $%02X%02X",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 3;
			break;

		case 15:
			// Absolute Indexed (with X)
			sprintf(Line, "%s%02X %02X    %s $%02X%02X,x",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Word += Registers.X.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 3;
			break;

		case 16:
			// Absolute Indexed (with Y)
			sprintf(Line, "%s%02X %02X    %s $%02X%02X,y",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Word += Registers.Y.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 3;
			break;

		case 17:
			// Absolute Long
			sprintf(Line, "%s%02X %02X %02X %s $%02X%02X%02X",
			        Line,
			        Operant[0],
			        Operant[1],
			        Operant[2],
			        S9xMnemonics[S9xOpcode],
			        Operant[2],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Operant[2], Word);
			Size = 4;
			break;

		case 18:
			// Absolute Indexed Long
			sprintf(Line, "%s%02X %02X %02X %s $%02X%02X%02X,x",
			        Line,
			        Operant[0],
			        Operant[1],
			        Operant[2],
			        S9xMnemonics[S9xOpcode],
			        Operant[2],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Word += Registers.X.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Operant[2], Word);
			Size = 4;
			break;

		case 19:
			// Stack Relative
			sprintf(Line, "%s%02X       %s $%02X,s",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Registers.S.W;
			Word += Operant[0];
			sprintf(Line, "%-32s[$00:%04X]", Line, Word);
			Size = 2;
			break;

		case 20:
			// Stack Relative Indirect Indexed
			sprintf(Line, "%s%02X       %s ($%02X,s),y",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Registers.S.W;
			Word += Operant[0];
			Word = S9xDebugGetWord(Word);
			Word += Registers.Y.W;
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.DB, Word);
			Size = 2;
			break;

		case 21:
			// Absolute Indirect
			sprintf(Line, "%s%02X %02X    %s ($%02X%02X)",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.PB, Word);
			Size = 3;
			break;

		case 22:
			// Absolute Indirect Long
			sprintf(Line, "%s%02X %02X    %s [$%02X%02X]",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Byte = S9xDebugGetByte(Word + 2);
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Byte, Word);
			Size = 3;
			break;

		case 23:
			// Absolute Indexed Indirect
			sprintf(Line, "%s%02X %02X    %s ($%02X%02X,x)",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Word = (Operant[1] << 8) | Operant[0];
			Word += Registers.X.W;
			Word = S9xDebugGetWord(ICPU.ShiftedPB + Word);
			sprintf(Line, "%-32s[$%02X:%04X]", Line, Registers.PB, Word);
			Size = 3;
			break;

		case 24:
			// Implied Accumulator
			sprintf(Line, "%s         %s A",
			        Line,
			        S9xMnemonics[S9xOpcode]);
			Size = 1;
			break;

		case 25:
			// MVN/MVP SRC DST
			sprintf(Line, "%s%02X %02X    %s %02X %02X",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Size = 3;
			break;

		case 26:
			// PEA
			sprintf(Line, "%s%02X %02X    %s $%02X%02X",
			        Line,
			        Operant[0],
			        Operant[1],
			        S9xMnemonics[S9xOpcode],
			        Operant[1],
			        Operant[0]);
			Size = 3;
			break;

		case 27:
			// PEI Direct Indirect
			sprintf(Line, "%s%02X       %s ($%02X)",
			        Line,
			        Operant[0],
			        S9xMnemonics[S9xOpcode],
			        Operant[0]);
			Word = Operant[0];
			Word += Registers.D.W;
			Word = S9xDebugGetWord(Word);
			sprintf(Line, "%-32s[$%04X]", Line, Word);
			Size = 2;
			break;
	}

	sprintf(Line, "%-44s A:%04X X:%04X Y:%04X D:%04X DB:%02X S:%04X P:%c%c%c%c%c%c%c%c%c HC:%04ld VC:%03ld FC:%02d %c%c%c %c%c%c %c%c HT:%d VT:%d C:%d",
	        Line, Registers.A.W, Registers.X.W, Registers.Y.W,
	        Registers.D.W, Registers.DB, Registers.S.W,
	        CheckEmulation() ? 'E' : 'e',
	        CheckNegative() ? 'N' : 'n',
	        CheckOverflow() ? 'V' : 'v',
	        CheckMemory() ? 'M' : 'm',
	        CheckIndex() ? 'X' : 'x',
	        CheckDecimal() ? 'D' : 'd',
	        CheckIRQ() ? 'I' : 'i',
	        CheckZero() ? 'Z' : 'z',
	        CheckCarry() ? 'C' : 'c',
	        (long) CPU.Cycles,
	        (long) CPU.V_Counter,
	        IPPU.FrameCount,
	        CPU.IRQExternal ?  'E' : ' ', PPU.HTimerEnabled ? 'H' : ' ', PPU.VTimerEnabled ? 'V' : ' ',
	        CPU.NMIPending ? 'N' : '.',
	        Memory.PPU_IO[0x200] & 0x80 ? 'n' : '.',
	        Memory.PPU_IO[0x210] & 0x80 ? '+' : '.',
	        CPU.IRQTransition ? 'T' : ' ',
	        CPU.IRQLine ? 'L' : ' ',
	        PPU.HTimerPosition, PPU.VTimerPosition, CPU.NextIRQTimer);

	return (Size);
}

static void debug_line_print (const char *Line)
{
	printf("%s\n", Line);
}

static int debug_get_number (char *Line, uint16 *Number)
{
	int	i;

	if (sscanf(Line, " #%d", &i) == 1)
	{
		*Number = i;
		return (1);
	}

	return (-1);
}

static short debug_get_start_address (char *Line, uint8 *Bank, uint32 *Address)
{
	uint32	a, b;

	if (sscanf(Line + 1, " $%x:%x", &b, &a) != 2)
		return (-1);

	*Bank = b;
	*Address = a;

	return (1);
}

void S9xDebugProcessCommand(char *Line)
{
	uint8	Bank = Registers.PB;
	uint32	Address = Registers.PCw;
	uint16	Hold = 0;
	uint16	Number;
	short	ErrorCode;
	char	string[512];

	if (strncasecmp(Line, "dump", 4) == 0)
	{
		int	Count;

		if (sscanf(&Line[4], "%x %d", &Address, &Count) == 2)
		{
			FILE	*fs;

			sprintf(string, "%06x%05d.sd2", Address, Count);
			fs = fopen(string, "wb");
			if (fs)
			{
				for (int i = 0; i < Count; i++)
					putc(S9xDebugGetByte(Address + i), fs);
				fclose(fs);
			}
			else
				printf("Can't open %s for writing\n", string);
		}
		else
			printf("Usage: dump start_address_in_hex count_in_decimal\n");

		return;
	}

	if (*Line == 'i')
	{
		printf("Vectors:\n");
		sprintf(string, "      8 Bit   16 Bit ");
		debug_line_print(string);
		sprintf(string, "ABT $00:%04X|$00:%04X", S9xDebugGetWord(0xFFF8), S9xDebugGetWord(0xFFE8));
		debug_line_print(string);
		sprintf(string, "BRK $00:%04X|$00:%04X", S9xDebugGetWord(0xFFFE), S9xDebugGetWord(0xFFE6));
		debug_line_print(string);
		sprintf(string, "COP $00:%04X|$00:%04X", S9xDebugGetWord(0xFFF4), S9xDebugGetWord(0xFFE4));
		debug_line_print(string);
		sprintf(string, "IRQ $00:%04X|$00:%04X", S9xDebugGetWord(0xFFFE), S9xDebugGetWord(0xFFEE));
		debug_line_print(string);
		sprintf(string, "NMI $00:%04X|$00:%04X", S9xDebugGetWord(0xFFFA), S9xDebugGetWord(0xFFEA));
		debug_line_print(string);
		sprintf(string, "RES     $00:%04X", S9xDebugGetWord(0xFFFC));
		debug_line_print(string);
	}

/*
	if (strncmp(Line, "ai", 2) == 0)
	{
		printf("APU vectors:");

		for (int i = 0; i < 0x40; i += 2)
		{
			if (i % 16 == 0)
				printf("\n%04x ", 0xffc0 + i);

			printf("%04x ", APU.ExtraRAM[i]);
		}

		printf("\n");
	}
*/

	if (*Line == 's')
	{
		Registers.PCw += debug_cpu_op_print(string, Bank, Address);
		Bank = Registers.PB;
		Address = Registers.PCw;
		*Line = 'r';
	}

	if (*Line == 'z')
	{
		uint16	*p = (uint16 *) &Memory.VRAM[PPU.BG[2].SCBase << 1];

		for (int l = 0; l < 32; l++)
		{
			for (int c = 0; c < 32; c++, p++)
				printf("%04x,", *p++);

			printf("\n");
		}
	}

	if (*Line == 'c')
	{
		printf("Colours:\n");

		for (int i = 0; i < 256; i++)
			printf("%02x%02x%02x  ", PPU.CGDATA[i] & 0x1f, (PPU.CGDATA[i] >> 5) & 0x1f, (PPU.CGDATA[i] >> 10) & 0x1f);

		printf("\n");
	}

	if (*Line == 'S')
	{
		int	SmallWidth, LargeWidth, SmallHeight, LargeHeight;

		switch ((Memory.PPU_IO[0x101] >> 5) & 7)
		{

			case 0:
				SmallWidth = SmallHeight = 8;
				LargeWidth = LargeHeight = 16;
				break;

			case 1:
				SmallWidth = SmallHeight = 8;
				LargeWidth = LargeHeight = 32;
				break;

			case 2:
				SmallWidth = SmallHeight = 8;
				LargeWidth = LargeHeight = 64;
				break;

			case 3:
				SmallWidth = SmallHeight = 16;
				LargeWidth = LargeHeight = 32;
				break;

			case 4:
				SmallWidth = SmallHeight = 16;
				LargeWidth = LargeHeight = 64;
				break;

			default:
			case 5:
				SmallWidth = SmallHeight = 32;
				LargeWidth = LargeHeight = 64;
				break;

			case 6:
				SmallWidth = 16;
				SmallHeight = 32;
				LargeWidth = 32;
				LargeHeight = 64;
				break;

			case 7:
				SmallWidth = 16;
				SmallHeight = 32;
				LargeWidth = LargeHeight = 32;
				break;
		}

		printf("Sprites: Small: %dx%d, Large: %dx%d, OAMAddr: 0x%04x, OBJNameBase: 0x%04x, OBJNameSelect: 0x%04x, First: %d\n",
		       SmallWidth, SmallHeight, LargeWidth, LargeHeight, PPU.OAMAddr, PPU.OBJNameBase, PPU.OBJNameSelect, PPU.FirstSprite);

		for (int i = 0; i < 128; i++)
		{
			printf("X:%3d Y:%3d %c%c%d%c ",
			       PPU.OBJ[i].HPos,
			       PPU.OBJ[i].VPos,
			       PPU.OBJ[i].VFlip ? 'V' : 'v',
			       PPU.OBJ[i].HFlip ? 'H' : 'h',
			       PPU.OBJ[i].Priority,
			       PPU.OBJ[i].Size ? 'S' : 's');

			if (i % 4 == 3)
				printf("\n");
		}
	}

	if (*Line == 'T')
	{
		CPU.Flags ^= TRACE_FLAG;

		if (CPU.Flags & TRACE_FLAG)
		{
			printf("CPU instruction tracing enabled.\n");
			ENSURE_TRACE_OPEN(trace, "trace.log", "wb")
		}
		else
		{
			printf("CPU instruction tracing disabled.\n");
			fclose(trace);
			trace = NULL;
		}
	}

	if (*Line == 'E')
	{
		Settings.TraceHCEvent = !Settings.TraceHCEvent;
		printf("HC event tracing %s.\n", Settings.TraceHCEvent ? "enabled" : "disabled");
	}

	// TODO: reactivate once APU debugger works again
	if (*Line == 'A')
	{
		Settings.TraceSMP = !Settings.TraceSMP;
		printf("SMP tracing %s\n", Settings.TraceSMP ? "enabled" : "disabled");
	}

/*
	if (*Line == 'B')
	{
		Settings.TraceSoundDSP = !Settings.TraceSoundDSP;
		printf("Sound DSP register tracing %s.\n", Settings.TraceSoundDSP ? "enabled" : "disabled");
	}

	if (*Line == 'x')
		S9xPrintSoundDSPState();

	if (*Line == 'C')
	{
		printf("SPC700 sample addresses at 0x%04x:\n", APU.DSP[APU_DIR] << 8);

		for (int i = 0; i < 256; i++)
		{
			uint8	*dir = IAPU.RAM + (((APU.DSP[APU_DIR] << 8) + i * 4) & 0xffff);
			int		addr = *dir + (*(dir + 1) << 8);
			int		addr2 = *(dir + 2) + (*(dir + 3) << 8);
			printf("%04X %04X;", addr, addr2);

			if (i % 8 == 7)
				printf("\n");
		}
	}
*/

	if (*Line == 'R')
	{
		S9xReset();
		printf("SNES reset.\n");
		CPU.Flags |= DEBUG_MODE_FLAG;
	}

/*
	if (strncmp(Line, "ad", 2) == 0)
	{
		uint32	Count = 16;
		Address = 0;

		if (sscanf(Line + 2, "%x,%x", &Address, &Count) != 2)
		{
			if (sscanf(Line + 2, "%x", &Address) == 1)
				Count = 16;
		}

		printf("APU RAM dump:\n");

		for (uint32 l = 0; l < Count; l += 16)
		{
			printf("%04X ", Address);

			for (int i = 0; i < 16; i++)
				printf("%02X ", IAPU.RAM[Address++]);

			printf("\n");
		}

		*Line = 0;
	}*/



	if (*Line == 'a')
	{
		// printf("S-CPU-side ports S-CPU writes these, S-SMP reads: %02X %02X %02X %02X\n", SNES::cpu.port_read(0), SNES::cpu.port_read(1), SNES::cpu.port_read(2), SNES::cpu.port_read(3));
		// printf("S-SMP-side ports S-SMP writes these, S-CPU reads: %02X %02X %02X %02X\n", SNES::smp.port_read(0), SNES::smp.port_read(1), SNES::smp.port_read(2), SNES::smp.port_read(3));
	}
/*
	if (*Line == 'P')
	{
		Settings.TraceDSP = !Settings.TraceDSP;
		printf("DSP tracing %s.\n", Settings.TraceDSP ? "enabled" : "disabled");
	}
*/

	if (*Line == 'p')
	{
		S9xBreakpoint[5].Enabled = FALSE;
		Address += debug_cpu_op_print(string, Bank, Address);

		if (strncmp(&string[18], "JMP", 3) != 0 &&
		    strncmp(&string[18], "JML", 3) != 0 &&
		    strncmp(&string[18], "RT" , 2) != 0 &&
		    strncmp(&string[18], "BRA", 3))
		{
			S9xBreakpoint[5].Enabled = TRUE;
			S9xBreakpoint[5].Bank = Bank;
			S9xBreakpoint[5].Address = Address;
		}
		else
		{
			CPU.Flags |= SINGLE_STEP_FLAG;
			CPU.Flags &= ~DEBUG_MODE_FLAG;
		}
	}

	if (*Line == 'b')
	{
		if (Line[1] == 's')
		{
			debug_get_number(Line + 2, &Hold);

			if (Hold > 4)
				Hold = 0;

			if (Hold < 5)
			{
				if (debug_get_start_address(Line + 5, &Bank, &Address) == -1)
					S9xBreakpoint[Hold].Enabled = FALSE;
				else
				{
					S9xBreakpoint[Hold].Enabled = TRUE;
					S9xBreakpoint[Hold].Bank = Bank;
					S9xBreakpoint[Hold].Address = Address;
					CPU.Flags |= BREAK_FLAG;
				}
			}

			Line[1] = 'v';
		}

		if (Line[1] == 'v')
		{
			Number = 0;

			if (debug_get_number(Line + 2, &Number) == -1 && Number < 5)
			{
				debug_line_print("Breakpoints:");

				for (Number = 0; Number != 5; Number++)
				{
					if (S9xBreakpoint[Number].Enabled)
						sprintf(string, "%i @ $%02X:%04X", Number, S9xBreakpoint[Number].Bank, S9xBreakpoint[Number].Address);
					else
						sprintf(string, "%i @ Disabled", Number);

					debug_line_print(string);
				}
			}
			else
			{
				debug_line_print("Breakpoint:");

				if (S9xBreakpoint[Number].Enabled)
					sprintf(string, "%i @ $%02X:%04X", Number, S9xBreakpoint[Number].Bank, S9xBreakpoint[Number].Address);
				else
					sprintf(string, "%i @ Disabled", Number);

				debug_line_print(string);
			}
		}
	}

	if (*Line == '?' || strcasecmp(Line, "help") == 0)
	{
		for (int i = 0; HelpMessage[i] != NULL; i++)
			debug_line_print(HelpMessage[i]);
	}

	if (*Line == 't')
	{
		CPU.Flags |= SINGLE_STEP_FLAG;
		CPU.Flags &= ~DEBUG_MODE_FLAG;
	}

	if (*Line == 'f')
	{
		CPU.Flags |= FRAME_ADVANCE_FLAG;
		CPU.Flags &= ~DEBUG_MODE_FLAG;

		IPPU.RenderThisFrame = TRUE;
		IPPU.FrameSkip = 0;

		if (sscanf(&Line[1], "%u", &ICPU.FrameAdvanceCount) != 1)
			ICPU.Frame = 0;
	}

	if (*Line == 'g')
	{
		S9xBreakpoint[5].Enabled = FALSE;

		bool8	found = FALSE;

		for (int i = 0; i < 5; i++)
		{
			if (S9xBreakpoint[i].Enabled)
			{
				found = TRUE;

				if (S9xBreakpoint[i].Bank == Registers.PB && S9xBreakpoint[i].Address == Registers.PCw)
				{
					S9xBreakpoint[i].Enabled = 2;
					break;
				}
			}
		}

		if (!found)
			CPU.Flags &= ~BREAK_FLAG;

		ErrorCode = debug_get_start_address(Line, &Bank, &Address);

		if (ErrorCode == 1)
		{
			S9xBreakpoint[5].Enabled = TRUE;
			S9xBreakpoint[5].Bank = Bank;
			S9xBreakpoint[5].Address = Address;
			CPU.Flags |= BREAK_FLAG;
		}

		CPU.Flags &= ~DEBUG_MODE_FLAG;
	}

	if (*Line == 'D')
	{
		Settings.TraceDMA = !Settings.TraceDMA;
		printf("DMA tracing %s.\n", Settings.TraceDMA ? "enabled" : "disabled");
	}

	if (*Line == 'V')
	{
		Settings.TraceVRAM = !Settings.TraceVRAM;
		printf("Non-DMA VRAM write tracing %s.\n", Settings.TraceVRAM ? "enabled" : "disabled");
	}

	if (*Line == 'H')
	{
		Settings.TraceHDMA = !Settings.TraceHDMA;
		printf("HDMA tracing %s.\n", Settings.TraceHDMA ? "enabled" : "disabled");
	}

	if (*Line == 'U')
	{
		Settings.TraceUnknownRegisters = !Settings.TraceUnknownRegisters;
		printf("Unknown registers read/write tracing %s.\n", Settings.TraceUnknownRegisters ? "enabled" : "disabled");
	}

	if (*Line == 'd')
	{
		int		CLine;
		int		CByte;
		uint8	MemoryByte;

		if (Debug.Dump.Bank != 0 || Debug.Dump.Address != 0)
		{
			Bank = Debug.Dump.Bank;
			Address = Debug.Dump.Address;
		}

		ErrorCode = debug_get_start_address(Line, &Bank, &Address);

		for (CLine = 0; CLine != 10; CLine++)
		{
			sprintf(string, "$%02X:%04X", Bank, Address);

			for (CByte = 0; CByte != 16; CByte++)
			{
				if (Address + CByte == 0x2140 ||
				    Address + CByte == 0x2141 ||
				    Address + CByte == 0x2142 ||
				    Address + CByte == 0x2143 ||
				    Address + CByte == 0x4210)
					MemoryByte = 0;
				else
					MemoryByte = S9xDebugGetByte((Bank << 16) + Address + CByte);

				sprintf(string, "%s %02X", string, MemoryByte);
			}

			sprintf(string, "%s-", string);

			for (CByte = 0; CByte != 16; CByte++)
			{
				if (Address + CByte == 0x2140 ||
				    Address + CByte == 0x2141 ||
				    Address + CByte == 0x2142 ||
				    Address + CByte == 0x2143 ||
				    Address + CByte == 0x4210)
					MemoryByte = 0;
				else
					MemoryByte = S9xDebugGetByte((Bank << 16) + Address + CByte);

				if (MemoryByte < 32 || MemoryByte >= 127)
					MemoryByte = '?';

				sprintf(string, "%s%c", string, MemoryByte);
			}

			Address += 16;

			debug_line_print(string);
		}

		Debug.Dump.Bank = Bank;
		Debug.Dump.Address = Address;
	}

	if (*Line == 'q')
		S9xExit();

	if (*Line == 'w')
		debug_whats_used();

	if (*Line == 'r')
	{
		debug_cpu_op_print(string, Bank, Address);
		debug_line_print(string);
	}

	if (*Line == 'u')
	{
		if (Debug.Unassemble.Bank != 0 || Debug.Unassemble.Address != 0)
		{
			Bank = Debug.Unassemble.Bank;
			Address = Debug.Unassemble.Address;
		}

		ErrorCode = debug_get_start_address(Line, &Bank, &Address);

		for (int i = 0; i != 10; i++)
		{
			Address += debug_cpu_op_print(string, Bank, Address);
			debug_line_print(string);
		}

		Debug.Unassemble.Bank = Bank;
		Debug.Unassemble.Address = Address;
	}

	debug_line_print("");

	return;
}

static void debug_print_window (uint8 *window)
{
	for (int i = 0; i < 6; i++)
	{
		if (window[i])
		{
			switch (i)
			{
				case 0:
					printf("Background 0, ");
					break;

				case 1:
					printf("Background 1, ");
					break;

				case 2:
					printf("Background 2, ");
					break;

				case 3:
					printf("Background 3, ");
					break;

				case 4:
					printf("Objects, ");
					break;

				case 5:
					printf("Color window, ");
					break;
			}
		}
	}
}

static const char * debug_clip_fn (int logic)
{
	switch (logic)
	{
		case CLIP_OR:
			return ("OR");

		case CLIP_AND:
			return ("AND");

		case CLIP_XOR:
			return ("XOR");

		case CLIP_XNOR:
			return ("XNOR");

		default:
			return ("???");
	}
}

static void debug_whats_used (void)
{
	printf("V-line: %ld, H-Pos: %ld, \n", (long) CPU.V_Counter, (long) CPU.Cycles);

	printf("Screen mode: %d, ", PPU.BGMode);

	if (PPU.BGMode <= 1 && (Memory.PPU_IO[0x105] & 8))
		printf("(BG#2 Priority), ");

	printf("Brightness: %d, ", PPU.Brightness);

	if (Memory.PPU_IO[0x100] & 0x80)
		printf("(screen blanked), ");

	printf("\n");

	if (Memory.PPU_IO[0x133] & 1)
		printf("Interlace, ");

	if (Memory.PPU_IO[0x133] & 4)
		printf("240 line visible, ");

	if (Memory.PPU_IO[0x133] & 8)
		printf("Pseudo 512 pixels horizontal resolution, ");

	if (Memory.PPU_IO[0x133] & 0x40)
		printf("Mode 7 priority per pixel, ");

	printf("\n");

	if (PPU.BGMode == 7 && (Memory.PPU_IO[0x11a] & 3))
		printf("Mode 7 flipping, ");

	if (PPU.BGMode == 7)
		printf("Mode 7 screen repeat: %d, ", (Memory.PPU_IO[0x11a] & 0xc0) >> 6);

	if (Memory.PPU_IO[0x130] & 1)
		printf("32K colour mode, ");

	printf("\n");

	if (PPU.BGMode == 7)
	{
		// Sign extend 13 bit values to 16 bit values...
		if (PPU.CentreX & (1 << 12))
			PPU.CentreX |= 0xe000;

		if (PPU.CentreY & (1 << 12))
			PPU.CentreY |= 0xe000;

		printf("Matrix A: %.3f, B: %.3f, C: %.3f, D: %.3f, Centre X: %d Y:%d, \n",
		       (double) PPU.MatrixA / 256, (double) PPU.MatrixB / 256,
		       (double) PPU.MatrixC / 256, (double) PPU.MatrixD / 256,
		       PPU.CentreX, PPU.CentreY);
	}

	if ((Memory.PPU_IO[0x106] & 0xf0) && (Memory.PPU_IO[0x106] & 0x0f))
	{
		printf("Mosaic effect(%d) on, ", PPU.Mosaic);

		for (int i = 0; i < 4; i++)
			if (Memory.PPU_IO[0x106] & (1 << i))
				printf("BG%d, ", i);
	}

	printf("\n");

	if (PPU.HVBeamCounterLatched)
		printf("V and H beam pos latched, \n");

	if (Memory.PPU_IO[0x200] & 0x20)
		printf("V-IRQ enabled at %d, \n", PPU.IRQVBeamPos);

	if (Memory.PPU_IO[0x200] & 0x10)
		printf("H-IRQ enabled at %d, \n", PPU.IRQHBeamPos);

	if (Memory.PPU_IO[0x200] & 0x80)
		printf("V-blank NMI enabled, \n");

	printf("VRAM write address: 0x%04x(%s), Full Graphic: %d, Address inc: %d, \n",
	       PPU.VMA.Address,
	       PPU.VMA.High  ? "Byte" : "Word",
	       PPU.VMA.FullGraphicCount, PPU.VMA.Increment);

	for (int i = 0; i < 4; i++)
	{
		printf("BG%d: VOffset:%d, HOffset:%d, W:%d, H:%d, TS:%d, BA:0x%04x, TA:0x%04X, \n",
		       i, PPU.BG[i].VOffset, PPU.BG[i].HOffset,
		       (PPU.BG[i].SCSize & 1) * 32 + 32,
		       (PPU.BG[i].SCSize & 2) * 16 + 32,
		       PPU.BG[i].BGSize * 8 + 8,
		       PPU.BG[i].SCBase,
		       PPU.BG[i].NameBase);
	}

	const char	*s = "";

	switch ((Memory.PPU_IO[0x130] & 0xc0) >> 6)
	{
		case 0:
			s = "always on";
			break;

		case 1:
			s = "inside";
			break;

		case 2:
			s = "outside";
			break;

		case 3:
			s = "always off";
			break;
	}

	printf("Main screen (%s): ", s);

	for (int i = 0; i < 5; i++)
	{
		if (Memory.PPU_IO[0x12c] & (1 << i))
		{
			switch (i)
			{
				case 0:
					printf("BG0, ");
					break;

				case 1:
					printf("BG1, ");
					break;

				case 2:
					printf("BG2, ");
					break;

				case 3:
					printf("BG3, ");
					break;

				case 4:
					printf("OBJ, ");
					break;
			}
		}
	}

	printf("\n");

	switch ((Memory.PPU_IO[0x130] & 0x30) >> 4)
	{
		case 0:
			s = "always on";
			break;

		case 1:
			s = "inside";
			break;

		case 2:
			s = "outside";
			break;

		case 3:
			s = "always off";
			break;
	}

	printf("Subscreen (%s): ", s);

	for (int i = 0; i < 5; i++)
	{
		if (Memory.PPU_IO[0x12d] & (1 << i))
		{
			switch (i)
			{
				case 0:
					printf("BG0, ");
					break;

				case 1:
					printf("BG1, ");
					break;

				case 2:
					printf("BG2, ");
					break;

				case 3:
					printf("BG3, ");
					break;

				case 4:
					printf("OBJ, ");
					break;
			}
		}
	}

	printf("\n");

	if ((Memory.PPU_IO[0x131] & 0x3f))
	{
		if (Memory.PPU_IO[0x131] & 0x80)
		{
			if (Memory.PPU_IO[0x130] & 0x02)
				printf("Subscreen subtract");
			else
				printf("Fixed colour subtract");
		}
		else
		{
			if (Memory.PPU_IO[0x130] & 0x02)
				printf("Subscreen addition");
			else
				printf("Fixed colour addition");
		}

		if (Memory.PPU_IO[0x131] & 0x40)
			printf("(half): ");
		else
			printf(": ");

		for (int i = 0; i < 6; i++)
		{
			if (Memory.PPU_IO[0x131] & (1 << i))
			{
				switch (i)
				{
					case 0:
						printf("BG0, ");
						break;

					case 1:
						printf("BG1, ");
						break;

					case 2:
						printf("BG2, ");
						break;

					case 3:
						printf("BG3, ");
						break;

					case 4:
						printf("OBJ, ");
						break;

					case 5:
						printf("BACK, ");
						break;
				}
			}
		}

		printf("\n");
	}

	printf("Window 1 (%d, %d, %02x, %02x): ", PPU.Window1Left, PPU.Window1Right, Memory.PPU_IO[0x12e], Memory.PPU_IO[0x12f]);

	for (int i = 0; i < 6; i++)
	{
		if (PPU.ClipWindow1Enable[i])
		{
			switch (i)
			{
				case 0:
					printf("BG0(%s-%s), ", PPU.ClipWindow1Inside[0] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[0]));
					break;

				case 1:
					printf("BG1(%s-%s), ", PPU.ClipWindow1Inside[1] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[1]));
					break;

				case 2:
					printf("BG2(%s-%s), ", PPU.ClipWindow1Inside[2] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[2]));
					break;

				case 3:
					printf("BG3(%s-%s), ", PPU.ClipWindow1Inside[3] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[3]));
					break;

				case 4:
					printf("OBJ(%s-%s), ", PPU.ClipWindow1Inside[4] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[4]));
					break;

				case 5:
					printf("COL(%s-%s), ", PPU.ClipWindow1Inside[5] ? "I" : "O", debug_clip_fn(PPU.ClipWindowOverlapLogic[5]));
					break;
			}
		}
	}

	printf("\n");

	printf("Window 2 (%d, %d): ", PPU.Window2Left, PPU.Window2Right);

	for (int i = 0; i < 6; i++)
	{
		if (PPU.ClipWindow2Enable[i])
		{
			switch (i)
			{
				case 0:
					printf("BG0(%s), ", PPU.ClipWindow2Inside[0] ? "I" : "O");
					break;

				case 1:
					printf("BG1(%s), ", PPU.ClipWindow2Inside[1] ? "I" : "O");
					break;

				case 2:
					printf("BG2(%s), ", PPU.ClipWindow2Inside[2] ? "I" : "O");
					break;

				case 3:
					printf("BG3(%s), ", PPU.ClipWindow2Inside[3] ? "I" : "O");
					break;

				case 4:
					printf("OBJ(%s), ", PPU.ClipWindow2Inside[4] ? "I" : "O");
					break;

				case 5:
					printf("COL(%s), " , PPU.ClipWindow2Inside[5] ? "I" : "O");
					break;
			}
		}
	}

	printf("\n");

	printf("Fixed colour: %02x%02x%02x, \n", PPU.FixedColourRed, PPU.FixedColourGreen, PPU.FixedColourBlue);
}

void S9xDoDebug (void)
{
	char	Line[513];

	Debug.Dump.Bank = 0;
	Debug.Dump.Address = 0;
	Debug.Unassemble.Bank = 0;
	Debug.Unassemble.Address = 0;

	S9xTextMode();

	strcpy(Line, "r");
	S9xDebugProcessCommand(Line);

	while (CPU.Flags & DEBUG_MODE_FLAG)
	{
		int32	Cycles;
		char	*p;

		printf("> ");
		fflush(stdout);

		p = fgets(Line, sizeof(Line) - 1, stdin);
		Line[strlen(Line) - 1] = 0;

		Cycles = CPU.Cycles;
		S9xDebugProcessCommand(Line);
		CPU.Cycles = Cycles;
	}

	if (!(CPU.Flags & SINGLE_STEP_FLAG))
		S9xGraphicsMode();
}

void S9xTrace (void)
{
	char	msg[512];

	ENSURE_TRACE_OPEN(trace, "trace.log", "a")

	debug_cpu_op_print(msg, Registers.PB, Registers.PCw);
	fprintf(trace, "%s\n", msg);
}

void S9xTraceMessage (const char *s)
{
	if (s)
	{
		if (trace)
			fprintf(trace, "%s\n", s);
		else
		if (trace2)
			fprintf(trace2, "%s\n", s);
	}
}

void S9xTraceFormattedMessage (const char *s, ...)
{
	char	msg[512];

	if (s)
	{
		va_list	argptr;

		va_start(argptr, s);
		vsprintf(msg, s, argptr);
		va_end(argptr);

		S9xTraceMessage(msg);
	}
}

void S9xPrintHVPosition (char *s)
{
	sprintf(s, "HC:%04ld VC:%03ld FC:%02d", (long) CPU.Cycles, (long) CPU.V_Counter, IPPU.FrameCount);
}

#endif
