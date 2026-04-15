#!/usr/bin/env python3
import os
import mammoth
from pathlib import Path

def convert_docx_to_markdown(docx_path, output_path):
    with open(docx_path, "rb") as docx_file:
        result = mammoth.convert_to_markdown(docx_file)
        markdown = result.value
        messages = result.messages
        
        with open(output_path, "w", encoding="utf-8") as markdown_file:
            markdown_file.write(markdown)
        
        if messages:
            print(f"转换 {docx_path} 时的警告:")
            for message in messages:
                print(f"  - {message}")
        
        return True

def main():
    input_dir = Path("/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Word")
    output_dir = Path("/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Markdown")
    
    output_dir.mkdir(parents=True, exist_ok=True)
    
    docx_files = list(input_dir.glob("*.docx"))
    
    print(f"找到 {len(docx_files)} 个 Word 文档")
    
    success_count = 0
    for docx_file in docx_files:
        try:
            output_file = output_dir / f"{docx_file.stem}.md"
            print(f"\n正在转换: {docx_file.name}")
            convert_docx_to_markdown(docx_file, output_file)
            print(f"✓ 已生成: {output_file.name}")
            success_count += 1
        except Exception as e:
            print(f"✗ 转换失败 {docx_file.name}: {e}")
    
    print(f"\n转换完成！成功: {success_count}/{len(docx_files)}")

if __name__ == "__main__":
    main()
