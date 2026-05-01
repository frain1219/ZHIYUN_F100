set pagination off
set confirm off

echo \n========================================\n
echo F103 Reset Status Check\n
echo ========================================\n\n

echo [1] Reading Current PC and Registers...\n
info registers pc sp r0 r1 r2 r3 r4 r5 r6 r7
echo \n

echo [2] Reading Stack Pointer...\n
info registers sp
echo \n

echo [3] Reading Vector Table (First 8 entries)...\n
x/8xw 0x08000000
echo \n

echo [4] Checking RCC Registers...\n
echo RCC_CR:\n
x/1xw 0x40021000
echo RCC_CFGR:\n
x/1xw 0x40021004
echo RCC_CIR:\n
x/1xw 0x40021008
echo RCC_APB2ENR:\n
x/1xw 0x40021018
echo RCC_APB1ENR:\n
x/1xw 0x4002101C
echo \n

echo [5] Checking PWR Registers...\n
echo PWR_CR:\n
x/1xw 0x40007000
echo PWR_CSR:\n
x/1xw 0x40007004
echo \n

echo [6] Checking Flash Registers...\n
echo FLASH_ACR:\n
x/1xw 0x40022000
echo FLASH_KEYR:\n
x/1xw 0x40022004
echo FLASH_SR:\n
x/1xw 0x4002200C
echo \n

echo [7] Checking GPIO Status...\n
echo GPIOA_CRL:\n
x/1xw 0x40010800
echo GPIOA_CRH:\n
x/1xw 0x40010804
echo GPIOA_IDR:\n
x/1xw 0x40010808
echo GPIOA_ODR:\n
x/1xw 0x4001080C
echo \n

echo [8] Checking Boot Pin Configuration...\n
echo BOOT0/BOOT1 status (from GPIO and RCC)\n
echo \n

echo [9] Reading Current Instruction...\n
x/1i $pc
echo \n

echo [10] Disassembling around PC...\n
x/10i $pc-20
echo \n

echo ========================================\n
echo Reset Status Check Complete\n
echo ========================================\n
