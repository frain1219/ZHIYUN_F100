set pagination off
set confirm off

target extended-remote localhost:4242

echo \n========================================\n
echo F103 Full SPI Status Check\n
echo ========================================\n\n

echo [1] Checking All SPI Registers...\n
echo SPI1_CR1 (0x40013000):\n
x/1xw 0x40013000
echo SPI2_CR1 (0x40003800):\n
x/1xw 0x40003800
echo \n

echo [2] Checking All GPIO Ports...\n
echo GPIOA_CRL:\n
x/1xw 0x40010800
echo GPIOA_CRH:\n
x/1xw 0x40010804
echo GPIOA_IDR:\n
x/1xw 0x40010808
echo GPIOA_ODR:\n
x/1xw 0x4001080C
echo \n

echo GPIOB_CRL:\n
x/1xw 0x40010C00
echo GPIOB_CRH:\n
x/1xw 0x40010C04
echo GPIOB_IDR:\n
x/1xw 0x40010C08
echo GPIOB_ODR:\n
x/1xw 0x40010C0C
echo \n

echo GPIOC_CRL:\n
x/1xw 0x40011000
echo GPIOC_CRH:\n
x/1xw 0x40011004
echo GPIOC_IDR:\n
x/1xw 0x40011008
echo GPIOC_ODR:\n
x/1xw 0x4001100C
echo \n

echo [3] Checking RCC Clocks...\n
echo RCC_APB2ENR:\n
x/1xw 0x40021018
echo RCC_APB1ENR:\n
x/1xw 0x4002101C
echo \n

echo [4] Checking Current PC...\n
info registers pc
echo \n

echo ========================================\n

quit
