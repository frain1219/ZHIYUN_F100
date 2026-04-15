include @/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Renode/apm32_f100.resc


# 尝试运行 server 模块
python3 -m gdb_mcp.server --gdbpath arm-none-eabi-gdb --connection "localhost:3333"
