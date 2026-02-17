#!/usr/bin/env python3
# encoding: utf-8

# ref: https://github.com/cmu-db/bustub/blob/master/script/formatting/formatter.py

'''
Usage: example
    python format_header.py src/include/storage/page/b_plus_tree_page.h \
        src/include/storage/page/b_plus_tree_leaf_page.h

then you will get the header in b_plus_tree_page.h.     
//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// b_plus_tree_page.h
//
// Identification: src/include/storage/page/b_plus_tree_page.h
//
// Copyright (c) 2015-2023, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

similar results insert in b_plus_tree_leaf_page.h.

'''


import os
import datetime
import sys
BUSTUB_DIR = os.path.abspath(os.path.dirname(
    __file__)).replace('/build_support', '')
BUSTUB_SRC_DIR = os.path.join(BUSTUB_DIR, "src")
BUSTUB_TESTS_DIR = os.path.join(BUSTUB_DIR, "test")

# DEFAULT DIRS
DEFAULT_DIRS = []
DEFAULT_DIRS.append(BUSTUB_SRC_DIR)
DEFAULT_DIRS.append(BUSTUB_TESTS_DIR)


# header framework, dynamic information will be added inside function
header_comment_line_1 = "//===----------------------------------------------------------------------===//\n"
header_comment_line_1 += "//\n"
header_comment_line_1 += "//                         BusTub\n"
header_comment_line_2 = "//\n"
header_comment_line_3 = "// "
header_comment_line_4 = "//\n"
header_comment_line_5 = "// Identification: "
header_comment_line_6 = "//\n"
header_comment_line_7 = "// Copyright (c) 2015-%d, Carnegie Mellon University Database Group\n" % datetime.datetime.now().year
header_comment_line_8 = "//\n"
header_comment_line_9 = "//===----------------------------------------------------------------------===//\n\n"

header_comment_line_6 = header_comment_line_6 + \
    header_comment_line_7+header_comment_line_8+header_comment_line_9

header_comment_line_1 += header_comment_line_2


def add_file_header(file: str):
    """add header to this file."""
    if not file.endswith('.h'):
        return
    file_path = os.path.join(BUSTUB_DIR, file)

    if not os.path.isfile(file_path):
        return

    dir_flag = False
    for deafult_dir in DEFAULT_DIRS:
        if file_path.startswith(deafult_dir):
            dir_flag = True
            break
    if not dir_flag:
        return

    file_name = os.path.basename(file)

    header_comment_filename = header_comment_line_3+file_name+'\n'
    header_comment_relpath = header_comment_line_5+file + '\n'
    header_commnt = header_comment_line_1 + header_comment_filename + \
        header_comment_line_4+header_comment_relpath+header_comment_line_6

    with open(file_path, 'r+') as f:
        # maybe use sed -i '' file
        old = f.read()
        f.seek(0)
        f.write(header_commnt)
        f.write(old)


if __name__ == '__main__':
    for file in sys.argv[1:]:
        add_file_header(file)
