set pagination off
set confirm off

echo \n========================================\n
echo Main函数执行流程分析\n
echo ========================================\n\n

echo [1] Main函数起始地址\n
echo main函数地址: 0x08002E74\n
echo main函数反汇编 (前100条指令):
x/100i 0x08002e74
echo \n

echo [2] 查找SPI1_StartupSequence函数\n
echo SPI1_StartupSequence地址 (from previous analysis): 0x08004184\n
echo SPI1_StartupSequence反汇编:
x/50i 0x08004184
echo \n

echo [3] 查找SPI1初始化相关函数\n
echo 搜索包含SPI的函数调用...
echo \n

echo [4] 检查当前PC位置附近的函数\n
echo 当前PC: 0x0800570C\n
echo LR: 0x08004399\n
echo 反汇编LR附近代码:
x/30i 0x08004380
echo \n

echo [5] 检查栈上的返回地址\n
echo 栈上返回地址: 0x08002FC5\n
echo 反汇编该地址附近:
x/30i 0x08002fb0
echo \n

echo [6] 查找延时函数\n
echo 延时函数地址: 0x080056EE\n
echo 反汇编延时函数:
x/30i 0x080056ee
echo \n

echo [7] 分析main函数调用链\n
echo 从main函数开始，跟踪函数调用...\n
echo \n

echo ========================================\n
echo 分析完成\n
echo ========================================\n
