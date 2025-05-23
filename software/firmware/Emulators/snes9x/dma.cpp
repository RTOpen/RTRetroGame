/*****************************************************************************\
     Snes9x - Portable Super Nintendo Entertainment System (TM) emulator.
                This file is licensed under the Snes9x License.
   For further information, consult the LICENSE file in the root directory.
\*****************************************************************************/

#include "snes9x.h"
#include "memmap.h"
#include "dma.h"
#include "apu/apu.h"
#ifdef DEBUGGER
#include "missing.h"
#endif

#define ADD_CYCLES(n)	{ CPU.Cycles += (n); }
#define GET_MEM_PTR(Address) ({ \
	uint32 a = (Address); \
	uint8 *ptr = S9xGetBasePointer(a); \
	ptr ? (ptr + (a & 0xffff)) : NULL; \
})

struct SDMA	DMA[8];


static inline bool8 addCyclesInDMA (uint32 dma_channel)
{
	// Add 8 cycles per byte, sync APU, and do HC related events.
	// If HDMA was done in S9xDoHEventProcessing(), check if it used the same channel as DMA.
	ADD_CYCLES(SLOW_ONE_CYCLE);
	if (CPU.Cycles >= CPU.NextEvent)
		S9xDoHEventProcessing();

	if (CPU.HDMARanInDMA & (1 << dma_channel))
	{
		CPU.HDMARanInDMA = 0;
	#ifdef DEBUGGER
		printf("HDMA and DMA use the same channel %d!\n", dma_channel);
	#endif
		// If HDMA triggers in the middle of DMA transfer and it uses the same channel,
		// it kills the DMA transfer immediately. $43x2 and $43x5 stop updating.
		return (FALSE);
	}

	CPU.HDMARanInDMA = 0;
	return (TRUE);
}

bool8 S9xDoDMA (uint8 Channel)
{
	CPU.InDMA = TRUE;
    CPU.InDMAorHDMA = TRUE;
	CPU.CurrentDMAorHDMAChannel = Channel;

    SDMA	*d = &DMA[Channel];

	// Check invalid DMA first
	if ((d->ABank == 0x7E || d->ABank == 0x7F) && d->BAddress == 0x80 && !d->ReverseTransfer)
	{
		// Attempting a DMA from WRAM to $2180 will not work, WRAM will not be written.
		// Attempting a DMA from $2180 to WRAM will similarly not work,
		// the value written is (initially) the OpenBus value.
		// In either case, the address in $2181-3 is not incremented.

		// Does an invalid DMA actually take time?
		// I'd say yes, since 'invalid' is probably just the WRAM chip
		// not being able to read and write itself at the same time
		// And no, PPU.WRAM should not be updated.

		int32	c = d->TransferBytes;
		// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
		if (c == 0)
			c = 0x10000;

		// 8 cycles per channel
		ADD_CYCLES(SLOW_ONE_CYCLE);
		// 8 cycles per byte
		while (c)
		{
			d->TransferBytes--;
			d->AAddress++;
			c--;
			if (!addCyclesInDMA(Channel))
			{
				CPU.InDMA = FALSE;
				CPU.InDMAorHDMA = FALSE;
				CPU.CurrentDMAorHDMAChannel = -1;
				return (FALSE);
			}
		}

	#ifdef DEBUGGER
		if (Settings.TraceDMA)
		{
			sprintf(String, "DMA[%d]: WRAM Bank:%02X->$2180", Channel, d->ABank);
			S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
		}
	#endif

		CPU.InDMA = FALSE;
		CPU.InDMAorHDMA = FALSE;
		CPU.CurrentDMAorHDMAChannel = -1;
		return (TRUE);
	}

	// Prepare for accessing $2118-2119
	switch (d->BAddress)
	{
		case 0x18:
		case 0x19:
			if (IPPU.RenderThisFrame)
				FLUSH_REDRAW();
			break;
	}

	int32	inc = d->AAddressFixed ? 0 : (!d->AAddressDecrement ? 1 : -1);
	int32	count = d->TransferBytes;
	// Writing $0000 to $43x5 actually results in a transfer of $10000 bytes, not 0.
	if (count == 0)
		count = 0x10000;

	// Prepare for custom chip DMA

#ifdef DEBUGGER
	if (Settings.TraceDMA)
	{
		sprintf(String, "DMA[%d]: %s Mode:%d 0x%02X%04X->0x21%02X Bytes:%d (%s) V:%03d",
			Channel, d->ReverseTransfer ? "PPU->CPU" : "CPU->PPU", d->TransferMode, d->ABank, d->AAddress, d->BAddress,
			d->TransferBytes, d->AAddressFixed ? "fixed" : (d->AAddressDecrement ? "dec" : "inc"), CPU.V_Counter);

		if (d->BAddress == 0x18 || d->BAddress == 0x19 || d->BAddress == 0x39 || d->BAddress == 0x3a)
			sprintf(String, "%s VRAM: %04X (%d,%d) %s", String,
				PPU.VMA.Address, PPU.VMA.Increment, PPU.VMA.FullGraphicCount, PPU.VMA.High ? "word" : "byte");
		else
		if (d->BAddress == 0x22 || d->BAddress == 0x3b)
			sprintf(String, "%s CGRAM: %02X (%x)", String, PPU.CGADD, PPU.CGFLIP);
		else
		if (d->BAddress == 0x04 || d->BAddress == 0x38)
			sprintf(String, "%s OBJADDR: %04X", String, PPU.OAMAddr);

		S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
	}
#endif

	// Do Transfer

	uint8	Work;

	// 8 cycles per channel
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!d->ReverseTransfer)
    {
		// CPU -> PPU
		int32	b = 0;
		uint16	p = d->AAddress;
		uint8	*base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
		bool8	inWRAM_DMA;

		int32	rem = count;
		// Transfer per block if d->AAdressFixed is FALSE
		count = d->AAddressFixed ? rem : (d->AAddressDecrement ? ((p & MEMMAP_MASK) + 1) : (MEMMAP_BLOCK_SIZE - (p & MEMMAP_MASK)));

		inWRAM_DMA = ((d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));

		// 8 cycles per byte
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			p += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		while (1)
		{
			if (count > rem)
				count = rem;
			rem -= count;

			CPU.InWRAMDMAorHDMA = inWRAM_DMA;

			if (!base)
			{
				// DMA SLOW PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					do
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
					} while (--count > 0);
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					// This is a variation on Duff's Device. It is legal C/C++.
					switch (b)
					{
						default:
						while (count > 1)
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							count--;
						// Fall through
						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							count--;
						}
					}

					if (count == 1)
					{
						Work = S9xGetByte((d->ABank << 16) + p);
						S9xSetPPU(Work, 0x2100 + d->BAddress);
						UPDATE_COUNTERS;
						b = 1;
					}
					else
						b = 0;
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}
						// Fall through
						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}
						// Fall through
						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}
						// Fall through
						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}
						// Fall through
						case 1:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}
						// Fall through
						case 2:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}
						// Fall through
						case 3:
							Work = S9xGetByte((d->ABank << 16) + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			#ifdef DEBUGGER
				else
				{
					sprintf(String, "Unknown DMA transfer mode: %d on channel %d\n", d->TransferMode, Channel);
					S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
				}
			#endif
			}
			else
			{
				// DMA FAST PATH
				if (d->TransferMode == 0 || d->TransferMode == 2 || d->TransferMode == 6)
				{
					switch (d->BAddress)
					{
						case 0x04: // OAMDATA
							do
							{
								Work = *(base + p);
								REGISTER_2104(Work);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x18: // VMDATAL
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
									REGISTER_2118_linear(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x19: // VMDATAH
							if (!PPU.VMA.FullGraphicCount)
							{
								do
								{
									Work = *(base + p);
									REGISTER_2119_linear(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									Work = *(base + p);
									REGISTER_2119_tile(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						case 0x22: // CGDATA
							do
							{
								Work = *(base + p);
								REGISTER_2122(Work);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;

						case 0x80: // WMDATA
							if (!CPU.InWRAMDMAorHDMA)
							{
								do
								{
									Work = *(base + p);
									REGISTER_2180(Work);
									UPDATE_COUNTERS;
								} while (--count > 0);
							}
							else
							{
								do
								{
									UPDATE_COUNTERS;
								} while (--count > 0);
							}

							break;

						default:
							do
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
							} while (--count > 0);

							break;
					}
				}
				else
				if (d->TransferMode == 1 || d->TransferMode == 5)
				{
					if (d->BAddress == 0x18)
					{
						// VMDATAL
						if (!PPU.VMA.FullGraphicCount)
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
									REGISTER_2118_linear(Work);
									UPDATE_COUNTERS;
									count--;
								// Fall through
								case 1:
									OpenBus = *(base + p);
									REGISTER_2119_linear(OpenBus);
									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
								REGISTER_2118_linear(Work);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
						else
						{
							switch (b)
							{
								default:
								while (count > 1)
								{
									Work = *(base + p);
									REGISTER_2118_tile(Work);
									UPDATE_COUNTERS;
									count--;
								// Fall through
								case 1:
									Work = *(base + p);
									REGISTER_2119_tile(Work);
									UPDATE_COUNTERS;
									count--;
								}
							}

							if (count == 1)
							{
								Work = *(base + p);
								REGISTER_2118_tile(Work);
								UPDATE_COUNTERS;
								b = 1;
							}
							else
								b = 0;
						}
					}
					else
					{
						// DMA mode 1 general case
						switch (b)
						{
							default:
							while (count > 1)
							{
								Work = *(base + p);
								S9xSetPPU(Work, 0x2100 + d->BAddress);
								UPDATE_COUNTERS;
								count--;
							// Fall through
							case 1:
								Work = *(base + p);
								S9xSetPPU(Work, 0x2101 + d->BAddress);
								UPDATE_COUNTERS;
								count--;
							}
						}

						if (count == 1)
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							b = 1;
						}
						else
							b = 0;
					}
				}
				else
				if (d->TransferMode == 3 || d->TransferMode == 7)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}
						// Fall through
						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}
						// Fall through
						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}
						// Fall through
						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
				else
				if (d->TransferMode == 4)
				{
					switch (b)
					{
						default:
						do
						{
							Work = *(base + p);
							S9xSetPPU(Work, 0x2100 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 1;
								break;
							}
						// Fall through
						case 1:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2101 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 2;
								break;
							}
						// Fall through
						case 2:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2102 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 3;
								break;
							}
						// Fall through
						case 3:
							Work = *(base + p);
							S9xSetPPU(Work, 0x2103 + d->BAddress);
							UPDATE_COUNTERS;
							if (--count <= 0)
							{
								b = 0;
								break;
							}
						} while (1);
					}
				}
			#ifdef DEBUGGER
				else
				{
					sprintf(String, "Unknown DMA transfer mode: %d on channel %d\n", d->TransferMode, Channel);
					S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
				}
			#endif
			}

			if (rem <= 0)
				break;

			base = S9xGetBasePointer((d->ABank << 16) + d->AAddress);
			count = MEMMAP_BLOCK_SIZE;
			inWRAM_DMA = ((d->ABank == 0x7e || d->ABank == 0x7f || (!(d->ABank & 0x40) && d->AAddress < 0x2000)));
		}

		#undef UPDATE_COUNTERS
	}
    else
    {
		// PPU -> CPU

		// 8 cycles per byte
		#define	UPDATE_COUNTERS \
			d->TransferBytes--; \
			d->AAddress += inc; \
			if (!addCyclesInDMA(Channel)) \
			{ \
				CPU.InDMA = FALSE; \
				CPU.InDMAorHDMA = FALSE; \
				CPU.InWRAMDMAorHDMA = FALSE; \
				CPU.CurrentDMAorHDMAChannel = -1; \
				return (FALSE); \
			}

		if (d->BAddress > 0x80 - 4 && d->BAddress <= 0x83 && !(d->ABank & 0x40))
		{
			// REVERSE-DMA REALLY-SLOW PATH
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						CPU.InWRAMDMAorHDMA = (d->AAddress < 0x2000);
						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
					#ifdef DEBUGGER
						sprintf(String, "Unknown DMA transfer mode: %d on channel %d\n", d->TransferMode, Channel);
						S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
					#endif
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
		else
		{
			// REVERSE-DMA FASTER PATH
			CPU.InWRAMDMAorHDMA = (d->ABank == 0x7e || d->ABank == 0x7f);
			do
			{
				switch (d->TransferMode)
				{
					case 0:
					case 2:
					case 6:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 1:
					case 5:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 3:
					case 7:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					case 4:
						Work = S9xGetPPU(0x2100 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2101 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2102 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						if (!--count)
							break;

						Work = S9xGetPPU(0x2103 + d->BAddress);
						S9xSetByte(Work, (d->ABank << 16) + d->AAddress);
						UPDATE_COUNTERS;
						count--;

						break;

					default:
					#ifdef DEBUGGER
						sprintf(String, "Unknown DMA transfer mode: %d on channel %d\n", d->TransferMode, Channel);
						S9xMessage(S9X_TRACE, S9X_DMA_TRACE, String);
					#endif
						while (count)
						{
							UPDATE_COUNTERS;
							count--;
						}

						break;
				}
			} while (count);
		}
	}

	if (CPU.NMIPending && (CPU.NMITriggerPos != 0xffff))
	{
		CPU.NMITriggerPos = CPU.Cycles + SNES_NMI_DMA_DELAY;
	}

#if 0
	// sanity check
    if (d->TransferBytes != 0)
		fprintf(stderr,"DMA[%d] TransferBytes not 0! $21%02x Reverse:%d %04x\n", Channel, d->BAddress, d->ReverseTransfer, d->TransferBytes);
#endif

	CPU.InDMA = FALSE;
	CPU.InDMAorHDMA = FALSE;
	CPU.InWRAMDMAorHDMA = FALSE;
	CPU.CurrentDMAorHDMAChannel = -1;

	return (TRUE);
}

static inline bool8 HDMAReadLineCount (int d)
{
	// CPU.InDMA is set, so S9xGetXXX() / S9xSetXXX() incur no charges.

	uint8	line;

	line = S9xGetByte((DMA[d].ABank << 16) + DMA[d].Address);
	ADD_CYCLES(SLOW_ONE_CYCLE);

	if (!line)
	{
		DMA[d].Repeat = FALSE;
		DMA[d].LineCount = 128;

		if (DMA[d].HDMAIndirectAddressing)
		{
			if (PPU.HDMA & (0xfe << d))
			{
				DMA[d].Address++;
				ADD_CYCLES(SLOW_ONE_CYCLE << 1);
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);

			DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address);
			DMA[d].Address++;
		}

		DMA[d].Address++;
		DMA[d].MemPointer = NULL;

		return (FALSE);
	}
	else
	if (line == 0x80)
	{
		DMA[d].Repeat = TRUE;
		DMA[d].LineCount = 128;
	}
	else
	{
		DMA[d].Repeat = !(line & 0x80);
		DMA[d].LineCount = line & 0x7f;
	}

	DMA[d].Address++;
	DMA[d].DoTransfer = TRUE;

	if (DMA[d].HDMAIndirectAddressing)
	{
		ADD_CYCLES(SLOW_ONE_CYCLE << 1);
		DMA[d].IndirectAddress = S9xGetWord((DMA[d].ABank << 16) + DMA[d].Address);
		DMA[d].Address += 2;
		DMA[d].MemPointer = GET_MEM_PTR((DMA[d].IndirectBank << 16) + DMA[d].IndirectAddress);
	}
	else
		DMA[d].MemPointer = GET_MEM_PTR((DMA[d].ABank << 16) + DMA[d].Address);

	return (TRUE);
}

void S9xStartHDMA (void)
{
	PPU.HDMA = Memory.CPU_IO[0x20c];
	PPU.HDMAEnded = 0;

	int32	tmpch;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	if (PPU.HDMA != 0)
	{
		ADD_CYCLES(SNES_DMA_CPU_SYNC_CYCLES);
	}

	for (int i = 0; i < 8; i++)
	{
		if (PPU.HDMA & (1 << i))
		{
			CPU.CurrentDMAorHDMAChannel = i;

			DMA[i].Address = DMA[i].AAddress;

			if (!HDMAReadLineCount(i))
			{
				PPU.HDMA &= ~(1 << i);
				PPU.HDMAEnded |= (1 << i);
			}
		}
		else
			DMA[i].DoTransfer = FALSE;
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.HDMARanInDMA = CPU.InDMA ? PPU.HDMA : 0;
	CPU.CurrentDMAorHDMAChannel = tmpch;
}

uint8 S9xDoHDMA (uint8 byte)
{
	const uint8 HDMA_ModeByteCounts[8] = {1, 2, 2, 4, 4, 4, 2, 4};
	struct SDMA *p;

	uint32	ShiftedIBank;
	uint16	IAddr;
	bool8	temp;
	int32	tmpch;
	int	d;
	uint8	mask;

	CPU.InHDMA = TRUE;
	CPU.InDMAorHDMA = TRUE;
	CPU.HDMARanInDMA = CPU.InDMA ? byte : 0;
	temp = CPU.InWRAMDMAorHDMA;
	tmpch = CPU.CurrentDMAorHDMAChannel;

	// XXX: Not quite right...
	ADD_CYCLES(SNES_DMA_CPU_SYNC_CYCLES);

	for (mask = 1, p = &DMA[0], d = 0; mask; mask <<= 1, p++, d++)
	{
		if (byte & mask)
		{
			CPU.InWRAMDMAorHDMA = FALSE;
			CPU.CurrentDMAorHDMAChannel = d;

			if (p->HDMAIndirectAddressing)
			{
				ShiftedIBank = (p->IndirectBank << 16);
				IAddr = p->IndirectAddress;
			}
			else
			{
				ShiftedIBank = (p->ABank << 16);
				IAddr = p->Address;
			}

			if (!DMA[d].MemPointer)
				DMA[d].MemPointer = GET_MEM_PTR(ShiftedIBank + IAddr);

			if (p->DoTransfer)
			{
			#ifdef DEBUGGER
				if (Settings.TraceHDMA && p->DoTransfer)
				{
					sprintf(String, "H-DMA[%d] %s (%d) 0x%06X->0x21%02X %s, Count: %3d, Rep: %s, V-LINE: %3ld %02X%04X",
							p-DMA, p->ReverseTransfer? "read" : "write",
							p->TransferMode, ShiftedIBank+IAddr, p->BAddress,
							p->HDMAIndirectAddressing ? "ind" : "abs",
							p->LineCount,
							p->Repeat ? "yes" : "no ", (long) CPU.V_Counter,
							p->ABank, p->Address);
					S9xMessage(S9X_TRACE, S9X_HDMA_TRACE, String);
				}
			#endif

				if (!p->ReverseTransfer)
				{
					if ((IAddr & MEMMAP_MASK) + HDMA_ModeByteCounts[p->TransferMode] >= MEMMAP_BLOCK_SIZE)
					{
						// HDMA REALLY-SLOW PATH
						DMA[d].MemPointer = NULL;

						#define DOBYTE(Addr, RegOff) \
							CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
								(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
							S9xSetPPU(S9xGetByte(ShiftedIBank + ((uint16) (Addr))), 0x2100 + p->BAddress + (RegOff));

						switch (p->TransferMode)
						{
							case 0:
								DOBYTE(IAddr, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 5:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 1:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 2:
							case 6:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 3:
							case 7:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;

							case 4:
								DOBYTE(IAddr + 0, 0);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 1, 1);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 2, 2);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								DOBYTE(IAddr + 3, 3);
								ADD_CYCLES(SLOW_ONE_CYCLE);
								break;
						}

						#undef DOBYTE
					}
					else
					{
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 ||
							(!(ShiftedIBank & 0x400000) && IAddr < 0x2000));

						if (!DMA[d].MemPointer)
						{
							// HDMA SLOW PATH
							uint32	Addr = ShiftedIBank + IAddr;

							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(S9xGetByte(Addr), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									Addr += 2;
									/* fall through */
								case 1:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 2:
								case 6:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 3:
								case 7:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 4:
									S9xSetPPU(S9xGetByte(Addr + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(S9xGetByte(Addr + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;
							}
						}
						else
						{
							// HDMA FAST PATH
							switch (p->TransferMode)
							{
								case 0:
									S9xSetPPU(*DMA[d].MemPointer++, 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									break;

								case 5:
									S9xSetPPU(*(DMA[d].MemPointer + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									DMA[d].MemPointer += 2;
									/* fall through */
								case 1:
									S9xSetPPU(*(DMA[d].MemPointer + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									// XXX: All HDMA should read to MDR first. This one just
									// happens to fix Speedy Gonzales.
									OpenBus = *(DMA[d].MemPointer + 1);
									S9xSetPPU(OpenBus, 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									DMA[d].MemPointer += 2;
									break;

								case 2:
								case 6:
									S9xSetPPU(*(DMA[d].MemPointer + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									DMA[d].MemPointer += 2;
									break;

								case 3:
								case 7:
									S9xSetPPU(*(DMA[d].MemPointer + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 1), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 2), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 3), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									DMA[d].MemPointer += 4;
									break;

								case 4:
									S9xSetPPU(*(DMA[d].MemPointer + 0), 0x2100 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 1), 0x2101 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 2), 0x2102 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									S9xSetPPU(*(DMA[d].MemPointer + 3), 0x2103 + p->BAddress);
									ADD_CYCLES(SLOW_ONE_CYCLE);
									DMA[d].MemPointer += 4;
									break;
							}
						}
					}
				}
				else
				{
					// REVERSE HDMA REALLY-SLOW PATH
					// anomie says: Since this is apparently never used
					// (otherwise we would have noticed before now), let's not bother with faster paths.
					DMA[d].MemPointer = NULL;

					#define DOBYTE(Addr, RegOff) \
						CPU.InWRAMDMAorHDMA = (ShiftedIBank == 0x7e0000 || ShiftedIBank == 0x7f0000 || \
							(!(ShiftedIBank & 0x400000) && ((uint16) (Addr)) < 0x2000)); \
						S9xSetByte(S9xGetPPU(0x2100 + p->BAddress + (RegOff)), ShiftedIBank + ((uint16) (Addr)));

					switch (p->TransferMode)
					{
						case 0:
							DOBYTE(IAddr, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 5:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 1:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 2:
						case 6:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 3:
						case 7:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;

						case 4:
							DOBYTE(IAddr + 0, 0);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 1, 1);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 2, 2);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							DOBYTE(IAddr + 3, 3);
							ADD_CYCLES(SLOW_ONE_CYCLE);
							break;
					}

					#undef DOBYTE
				}
			}
		}
	}

	for (mask = 1, p = &DMA[0], d = 0; mask; mask <<= 1, p++, d++)
	{
		if (byte & mask)
		{
			if (p->DoTransfer)
			{
				if (p->HDMAIndirectAddressing)
					p->IndirectAddress += HDMA_ModeByteCounts[p->TransferMode];
				else
					p->Address += HDMA_ModeByteCounts[p->TransferMode];
			}

			p->DoTransfer = !p->Repeat;

			if (!--p->LineCount)
			{
				if (!HDMAReadLineCount(d))
				{
					byte &= ~mask;
					PPU.HDMAEnded |= mask;
					p->DoTransfer = FALSE;
				}
			}
			else
				ADD_CYCLES(SLOW_ONE_CYCLE);
		}
	}

	CPU.InHDMA = FALSE;
	CPU.InDMAorHDMA = CPU.InDMA;
	CPU.InWRAMDMAorHDMA = temp;
	CPU.CurrentDMAorHDMAChannel = tmpch;

	return (byte);
}

void S9xResetDMA (void)
{
	for (int d = 0; d < 8; d++)
	{
		DMA[d].ReverseTransfer = TRUE;
		DMA[d].HDMAIndirectAddressing = TRUE;
		DMA[d].AAddressFixed = TRUE;
		DMA[d].AAddressDecrement = TRUE;
		DMA[d].TransferMode = 7;
		DMA[d].BAddress = 0xff;
		DMA[d].AAddress = 0xffff;
		DMA[d].ABank = 0xff;
		DMA[d].DMACount_Or_HDMAIndirectAddress = 0xffff;
		DMA[d].IndirectBank = 0xff;
		DMA[d].Address = 0xffff;
		DMA[d].Repeat = FALSE;
		DMA[d].LineCount = 0x7f;
		DMA[d].UnknownByte = 0xff;
		DMA[d].DoTransfer = FALSE;
		DMA[d].UnusedBit43x0 = 1;
	}
}
