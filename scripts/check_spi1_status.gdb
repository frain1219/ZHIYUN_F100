set pagination off
set confirm off

echo \n========================================\n
echo SPI1初始化状态检查\n
echo ========================================\n\n

echo [1] 检查RCC_APB2ENR (SPI1时钟使能)\n
echo RCC_APB2ENR地址: 0x40021018\n
x/1xw 0x40021018
echo \n

echo [2] 检查SPI1_CR1 (控制寄存器1)\n
echo SPI1_CR1地址: 0x40013000\n
x/1xw 0x40013000
echo \n

echo [3] 检查SPI1_CR2 (控制寄存器2)\n
echo SPI1_CR2地址: 0x40013004\n
x/1xw 0x40013004
echo \n

echo [4] 检查SPI1_SR (状态寄存器)\n
echo SPI1_SR地址: 0x40013008\n
x/1xw 0x40013008
echo \n

echo [5] 检查SPI1_DR (数据寄存器)\n
echo SPI1_DR地址: 0x4001300C\n
x/1xw 0x4001300C
echo \n

echo [6] 检查GPIOA配置 (PA5/PA6/PA7 - SPI1引脚)\n
echo GPIOA_CRL (PA0-PA7配置):
x/1xw 0x40010800
echo GPIOA_IDR (输入数据):
x/1xw 0x40010808
echo GPIOA_ODR (输出数据):
x/1xw 0x4001080C
echo \n

echo [7] 检查GPIOA引脚模式\n
echo PA5 (SCK): 
echo PA6 (MISO):
echo PA7 (MOSI):
echo 需要根据CRL寄存器值解析\n
echo \n

echo [8] 解析GPIOA_CRL\n
echo CRL = 0x44444444\n
echo 每个引脚4位配置:\n
echo PA0: 0x4 (通用推挽输出)
echo PA1: 0x4 (通用推挽输出)
echo PA2: 0x4 (通用推挽输出)
echo PA3: 0x4 (通用推挽输出)
echo PA4: 0x4 (通用推挽输出)
echo PA5: 0x4 (通用推挽输出) ← SCK
echo PA6: 0x4 (通用推挽输出) ← MISO
echo PA7: 0x4 (通用推挽输出) ← MOSI
echo \n

echo [9] 检查AFIO配置\n
echo AFIO_MAPR (复用重映射):
x/1xw 0x40010004
echo \n

echo [10] 检查DMA配置\n
echo DMA1_Channel3 (SPI1_RX):
echo DMA_CCR3:
x/1xw 0x40020038
echo DMA_CNDTR3:
x/1xw 0x4002003C
echo DMA_CPAR3:
x/1xw 0x40020040
echo DMA_CMAR3:
x/1xw 0x40020044
echo \n

echo ========================================\n
echo SPI1状态检查完成\n
echo ========================================\n
