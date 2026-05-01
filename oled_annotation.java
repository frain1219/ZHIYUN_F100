//OLED硬件闭合验证标注脚本
//添加注释和书签标记OLED显示部分硬件层逆向验证完成

import ghidra.app.script.GhidraScript;
import ghidra.program.model.listing.BookmarkManager;
import ghidra.program.model.listing.BookmarkType;

public class OLED_Hardware_Closure_Annotation extends GhidraScript {
    @Override
    public void run() throws Exception {
        println("开始添加OLED硬件闭合验证标注...");
        
        //定义关键地址
        String[][] annotations = {
            {"0x080041B0", "OLED 23字节初始化序列", "OLED 23字节初始化序列 - 硬件闭合验证完成 (2026-05-02)\n驱动芯片: SSD1306族 (128×64 SPI)\n序列: AE 00 10 40 81 FF A1 A8 3F AD 8B C8 D3 00 DA 12 D9 22 DB 2B A4 A6 AF\n验证: 命令语义与SSD1306数据手册高度匹配"},
            {"0x08004184", "SPI1_StartupSequence", "SPI1_StartupSequence - OLED初始化序列发送函数\n硬件闭合验证完成 (2026-05-02)\n功能: 通过SPI1发送23字节SSD1306初始化命令\n调用者: main (唯一)"},
            {"0x08004258", "SPI1_PumpEightFramebufferSlices", "SPI1_PumpEightFramebufferSlices - OLED帧缓冲刷新函数\n硬件闭合验证完成 (2026-05-02)\n功能: 运行期刷新OLED显示, 8×(3B前缀+96B数据)\n格式: 符合SPI OLED典型事务模式"},
            {"0x0800D89C", "UI_OLED_MainStateMachine", "UI_OLED_MainStateMachine - OLED UI主状态机\n硬件闭合验证完成 (2026-05-02)\n功能: 管理OLED显示状态切换和模式渲染\n验证: 状态机逻辑已深入分析"}
        };
        
        BookmarkManager bookmarkMgr = currentProgram.getBookmarkManager();
        
        for (String[] ann : annotations) {
            String addrStr = ann[0];
            String name = ann[1];
            String comment = ann[2];
            
            //添加plate注释
            setPlateComment(toAddr(addrStr), comment);
            println("已添加注释: " + name + " @ " + addrStr);
            
            //添加书签
            bookmarkMgr.setBookmark(toAddr(addrStr), "Analysis", "OLED硬件闭合", 
                "OLED显示部分硬件层逆向验证完成 (2026-05-02)");
            println("已添加书签: " + name + " @ " + addrStr);
        }
        
        println("\nOLED硬件闭合验证标注完成!");
        println("共添加 " + annotations.length + " 个注释和书签");
    }
}
