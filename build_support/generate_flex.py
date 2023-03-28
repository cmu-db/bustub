# ===----------------------------------------------------------------------===//
# Copyright 2018-2022 Stichting DuckDB Foundation
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:

# The above copyright notice and this permission notice (including the next paragraph)
# shall be included in all copies or substantial portions of the Software.

# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
# ===----------------------------------------------------------------------===//

# use flex to generate the scanner file for the parser
# the following version of bison is used:
# flex 2.5.35 Apple(flex-32)
import os
import subprocess
import re
from sys import platform
import sys


def open_utf8(fpath, flags):
    import sys
    if sys.version_info[0] < 3:
        return open(fpath, flags)
    else:
        return open(fpath, flags, encoding="utf8")


flex_bin = 'flex'
for arg in sys.argv[1:]:
    if arg.startswith("--flex="):
        flex_bin = arg.replace("--flex=", "")

pg_path = os.path.join('third_party', 'libpg_query')
flex_file_path = os.path.join(pg_path, 'scan.l')
target_file = os.path.join(pg_path, 'src_backend_parser_scan.cpp')

proc = subprocess.Popen([flex_bin, '--nounistd', '-o', target_file,
                        flex_file_path], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
stdout = proc.stdout.read().decode('utf8')
stderr = proc.stderr.read().decode('utf8')
if proc.returncode != None or len(stderr) > 0:
    print("Flex failed")
    print("stdout: ", stdout)
    print("stderr: ", stderr)
    exit(1)

with open_utf8(target_file, 'r') as f:
    text = f.read()

# convert this from 'int' to 'yy_size_t' to avoid triggering a warning
text = text.replace('int yy_buf_size;\n', 'yy_size_t yy_buf_size;\n')

# add the libpg_query namespace
text = text.replace('''
#ifndef FLEXINT_H
#define FLEXINT_H
''', '''
#ifndef FLEXINT_H
#define FLEXINT_H
namespace duckdb_libpgquery {
''')
text = text.replace('register ', '')

text = text + "\n} /* duckdb_libpgquery */\n"

text = re.sub('(?:[(]void[)][ ]*)?fprintf', '//', text)
text = re.sub('exit[(]', 'throw std::runtime_error(msg); //', text)
text = re.sub(
    r'\n\s*if\s*[(]\s*!\s*yyin\s*[)]\s*\n\s*yyin\s*=\s*stdin;\s*\n', '\n', text)
text = re.sub(
    r'\n\s*if\s*[(]\s*!\s*yyout\s*[)]\s*\n\s*yyout\s*=\s*stdout;\s*\n', '\n', text)

file_null = 'NULL' if platform == 'linux' else '[(]FILE [*][)] 0'

text = re.sub(
    rf'[#]ifdef\s*YY_STDINIT\n\s*yyin = stdin;\n\s*yyout = stdout;\n[#]else\n\s*yyin = {file_null};\n\s*yyout = {file_null};\n[#]endif', '    yyin = (FILE *) 0;\n    yyout = (FILE *) 0;', text)

if 'stdin;' in text:
    print("STDIN not removed!")
    # exit(1)

if 'stdout' in text:
    print("STDOUT not removed!")
    # exit(1)

if 'fprintf(' in text:
    print("PRINTF not removed!")
    # exit(1)

if 'exit(' in text:
    print("EXIT not removed!")
    # exit(1)

with open_utf8(target_file, 'w+') as f:
    f.write(text)
