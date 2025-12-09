#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""修正生成的 myNS.h 头文件中的 include 路径"""

import sys

if len(sys.argv) != 2:
    print("Usage: fix_header.py <header_file>")
    sys.exit(1)

header_file = sys.argv[1]

with open(header_file, 'r', encoding='utf-8') as f:
    content = f.read()

# 替换 include 路径
content = content.replace('#include <open62541/server.h>', '#include "open62541.h"')

with open(header_file, 'w', encoding='utf-8') as f:
    f.write(content)

print(f"Fixed header file: {header_file}")
