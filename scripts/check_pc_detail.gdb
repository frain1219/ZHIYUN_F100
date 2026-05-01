set pagination off
set confirm off

target extended-remote localhost:4242

echo \n========================================\n
echo F103 PC Location Analysis\n
echo ========================================\n\n

echo [1] Current PC Location:\n
info registers pc
echo \n

echo [2] Disassemble around PC (20 instructions):\n
x/20i $pc-40
echo \n

echo [3] Check if in infinite loop:\n
x/10i $pc
echo \n

echo [4] Check stack pointer:\n
info registers sp
echo \n

echo [5] Check link register:\n
info registers lr
echo \n

echo [6] Check if in exception handler:\n
info registers xpsr
echo \n

echo [7] Check vector table:\n
x/16xw 0x08000000
echo \n

echo ========================================\n

quit
