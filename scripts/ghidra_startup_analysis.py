#!/usr/bin/env python3
"""
使用Ghidra分析F103启动流程
"""

import subprocess
import json
import re

class GhidraStartupAnalyzer:
    def __init__(self):
        self.ghidra_headless = "/Applications/ghidra_11.2.1_PUBLIC/support/analyzeHeadless"
        self.project_path = "/Users/milocheung/Documents/codeProject/ZHIYUN_F100/ZY_F100"
        self.program_name = "ZHIYUN-F100-full.bin"
        
    def analyze_address(self, address):
        """分析指定地址的函数"""
        script = f"""
        // Get function at address
        func = getFunctionAt(toAddr(0x{address}))
        if func:
            print("FUNCTION_NAME:" + func.getName())
            print("FUNCTION_ENTRY:" + hex(func.getEntryPoint().getOffset()))
            print("FUNCTION_END:" + hex(func.getBody().getMaxAddress().getOffset()))
        else:
            print("NO_FUNCTION_FOUND")
        
        // Get disassembly
        addr = toAddr(0x{address})
        listing = currentProgram.getListing()
        instr = listing.getInstructionAt(addr)
        if instr:
            for i in range(20):
                print("DISASM:" + hex(instr.getAddress().getOffset()) + ":" + instr.toString())
                instr = instr.getNext()
        """
        
        # Save script
        script_path = "/tmp/ghidra_analyze.py"
        with open(script_path, 'w') as f:
            f.write(script)
        
        # Run Ghidra
        cmd = [
            self.ghidra_headless,
            self.project_path,
            "TempProject",
            "-process", self.program_name,
            "-postScript", script_path,
            "-noanalysis"
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            return result.stdout
        except Exception as e:
            return f"Error: {e}"
    
    def find_function_containing(self, address):
        """查找包含指定地址的函数"""
        script = f"""
        // Find function containing address
        addr = toAddr(0x{address})
        func = getFunctionContaining(addr)
        if func:
            print("FUNCTION_NAME:" + func.getName())
            print("FUNCTION_ENTRY:" + hex(func.getEntryPoint().getOffset()))
            print("FUNCTION_END:" + hex(func.getBody().getMaxAddress().getOffset()))
            
            // Decompile
            from ghidra.app.decompiler import DecompInterface
            decomp = DecompInterface()
            decomp.openProgram(currentProgram)
            results = decomp.decompileFunction(func, 60, monitor)
            if results.decompileCompleted():
                print("DECOMPILE_START")
                print(results.getDecompiledFunction().getC())
                print("DECOMPILE_END")
        else:
            print("NO_FUNCTION_FOUND")
        """
        
        script_path = "/tmp/ghidra_find_func.py"
        with open(script_path, 'w') as f:
            f.write(script)
        
        cmd = [
            self.ghidra_headless,
            self.project_path,
            "TempProject",
            "-process", self.program_name,
            "-postScript", script_path,
            "-noanalysis"
        ]
        
        try:
            result = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
            return result.stdout
        except Exception as e:
            return f"Error: {e}"

if __name__ == "__main__":
    analyzer = GhidraStartupAnalyzer()
    
    print("\n" + "="*60)
    print("分析当前PC位置: 0x0800570C")
    print("="*60)
    
    # 分析当前PC位置
    output = analyzer.find_function_containing(0x0800570C)
    print(output)
