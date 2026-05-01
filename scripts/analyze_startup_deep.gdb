set pagination off
set confirm off

echo \n========================================\n
echo F103启动流程深度分析\n
echo ========================================\n\n

echo [1] 分析当前PC位置 (0x0800570C)\n
echo 当前指令:\n
x/1i $pc
echo \n

echo [2] 反汇编当前函数 (向前50条指令)\n
x/50i $pc-100
echo \n

echo [3] 反汇编当前函数 (向后50条指令)\n
x/50i $pc
echo \n

echo [4] 检查栈帧和返回地址\n
echo SP: 
info registers sp
echo LR: 
info registers lr
echo \n

echo [5] 检查栈内容 (查看调用链)\n
echo Stack contents (SP-16 to SP+64):
x/20xw $sp-16
echo \n

echo [6] 检查Reset_Handler\n
echo Reset_Handler address (from vector table):
x/1xw 0x08000004
echo Reset_Handler code:
x/20i 0x08006be8
echo \n

echo [7] 检查main函数\n
echo main函数地址 (from previous analysis): 0x08002E74\n
echo main函数反汇编:
x/30i 0x08002e74
echo \n

echo [8] 检查SystemInit函数\n
echo SystemInit疑似地址: 0x080069DC\n
echo SystemInit反汇编:
x/30i 0x080069dc
echo \n

echo [9] 检查中断向量表\n
echo 前16个中断向量:
x/16xw 0x08000000
echo \n

echo [10] 检查RCC复位标志\n
echo RCC_CSR (Reset flags):
x/1xw 0x40021024
echo \n

echo [11] 检查PWR状态\n
echo PWR_CSR:
x/1xw 0x40007004
echo \n

echo [12] 检查是否在循环中\n
echo 当前PC附近是否有循环结构?
echo 检查分支指令:
x/30i $pc-60
echo \n

echo ========================================\n
echo 分析完成\n
echo ========================================\n
