import sys
import re

directory_path = "./src/Modes/"
def to_camel_case(name):
    words = name.split('_')
    return ''.join(word.capitalize() for word in words)

if len(sys.argv) != 2:
    print("Usage: python CreateMode.py NewMode")
    sys.exit(1)

new_mode_name = to_camel_case(sys.argv[1])

# ModeTemp.hppファイルの読み込み
with open(directory_path + 'ModeTemp.hpp', 'r') as f:
    content = f.read()

# 文字列の置換
content = re.sub(r'TempMode', new_mode_name + 'Mode', content)
content = re.sub(r'tempMode', new_mode_name[0].lower() + new_mode_name[1:] + 'Mode', content)

# 新しいファイル名を作成
new_filename = 'Mode' + new_mode_name + '.hpp'
with open(directory_path + new_filename, 'w') as f:
    f.write(content)

print(f"Mode {new_mode_name} created in {new_filename}")
