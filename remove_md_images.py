#!/usr/bin/env python3
import os
import re
import glob

def remove_images_from_md(file_path):
    with open(file_path, 'r', encoding='utf-8') as f:
        content = f.read()
    
    pattern = r'!\[.*?\]\(data:image/[^)]+\)'
    new_content = re.sub(pattern, '', content)
    
    with open(file_path, 'w', encoding='utf-8') as f:
        f.write(new_content)
    
    original_size = len(content)
    new_size = len(new_content)
    removed = original_size - new_size
    return original_size, new_size, removed

def main():
    md_dir = '/Users/milocheung/Documents/codeProject/ZHIYUN_F100/Document/Markdown'
    md_files = glob.glob(os.path.join(md_dir, '*.md'))
    
    print(f"找到 {len(md_files)} 个Markdown文件\n")
    
    total_original = 0
    total_new = 0
    total_removed = 0
    
    for md_file in md_files:
        filename = os.path.basename(md_file)
        original, new, removed = remove_images_from_md(md_file)
        total_original += original
        total_new += new
        total_removed += removed
        print(f"处理: {filename}")
        print(f"  原大小: {original:,} 字节")
        print(f"  新大小: {new:,} 字节")
        print(f"  删除: {removed:,} 字节\n")
    
    print("=" * 50)
    print(f"总计:")
    print(f"  原大小: {total_original:,} 字节")
    print(f"  新大小: {total_new:,} 字节")
    print(f"  删除: {total_removed:,} 字节")
    print(f"  压缩率: {total_removed/total_original*100:.1f}%")

if __name__ == '__main__':
    main()
