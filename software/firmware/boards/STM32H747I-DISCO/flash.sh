#!/bin/bash
STM32_Programmer_CLI -c port=SWD index=0 reset=HWrst -el MT25TL01G_STM32H747I-DISCO.stldr -d rtthread.bin 0x08000000 -HardRst