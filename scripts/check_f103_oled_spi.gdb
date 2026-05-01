set pagination off
set confirm off

target extended-remote localhost:4242

echo \n========================================\n
echo F103 OLED SPI Status Check\n
echo ========================================\n\n

echo [1] Checking CPU Status...\n
info registers pc
echo \n

echo [2] Checking SPI1 Registers...\n
echo SPI1_CR1 (0x40013000):\n
x/1xw 0x40013000
echo SPI1_CR2 (0x40013004):\n
x/1xw 0x40013004
echo SPI1_SR (0x40013008):\n
x/1xw 0x40013008
echo SPI1_DR (0x4001300C):\n
x/1xw 0x4001300C
echo \n

echo [3] Checking GPIOA Registers (PA5/PA6/PA7)...\n
echo GPIOA_CRL (0x40010800):\n
x/1xw 0x40010800
echo GPIOA_CRH (0x40010804):\n
x/1xw 0x40010804
echo GPIOA_IDR (0x40010808):\n
x/1xw 0x40010808
echo GPIOA_ODR (0x4001080C):\n
x/1xw 0x4001080C
echo \n

echo [4] Checking GPIOB Registers (PB2/PB10)...\n
echo GPIOB_CRL (0x40010C00):\n
x/1xw 0x40010C00
echo GPIOB_CRH (0x40010C04):\n
x/1xw 0x40010C04
echo GPIOB_IDR (0x40010C08):\n
x/1xw 0x40010C08
echo GPIOB_ODR (0x40010C0C):\n
x/1xw 0x40010C0C
echo \n

echo [5] Checking RCC Registers...\n
echo RCC_APB2ENR (0x40021018):\n
x/1xw 0x40021018
echo RCC_APB1ENR (0x4002101C):\n
x/1xw 0x4002101C
echo RCC_AHBENR (0x40021014):\n
x/1xw 0x40021014
echo \n

echo [6] Checking AFIO Registers...\n
echo AFIO_MAPR (0x40010004):\n
x/1xw 0x40010004
echo \n

echo ========================================\n
echo Analysis Complete\n
echo ========================================\n

quit
