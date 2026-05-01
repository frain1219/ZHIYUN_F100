set pagination off
set confirm off

target extended-remote localhost:4242

echo \n========================================\n
echo F103 .bss Clear Loop Analysis\n
echo ========================================\n\n

echo [1] Check loop variables:\n
echo r1 (destination pointer):\n
info registers r1
echo r2 (counter):\n
info registers r2
echo r3 (value to store):\n
info registers r3
echo \n

echo [2] Calculate remaining iterations:\n
echo r2 / 4 = remaining words to clear\n
echo \n

echo [3] Check destination address:\n
echo r1 points to the memory being cleared\n
echo \n

echo [4] Check if memory is accessible:\n
x/10xw $r1
echo \n

echo [5] Try to skip the loop:\n
echo Setting r2 = 0 to exit loop...\n
set $r2 = 0
echo \n

echo [6] Continue execution:\n
continue
echo \n

echo [7] Check new PC after 2 seconds:\n
info registers pc

quit
