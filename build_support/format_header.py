#!/usr/bin/env python3
# encoding: utf-8
#
# Licensed to the Apache Software Foundation (ASF) under one
# or more contributor license agreements.  See the NOTICE file
# distributed with this work for additional information
# regarding copyright ownership.  The ASF licenses this file
# to you under the Apache License, Version 2.0 (the
# "License"); you may not use this file except in compliance
# with the License.  You may obtain a copy of the License at
#
#   http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing,
# software distributed under the License is distributed on an
# "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
# KIND, either express or implied.  See the License for the
# specific language governing permissions and limitations
# under the License.
#
# Modified from the Apache Arrow project for the Terrier project.

"""Format the ill-formatted code."""
# ==============================================
# GOAL : Format code, Update headers
# ==============================================

# ===----------------------------------------------------------------------===//
#
#                          CMU-DB Project (15-445/645)
#                          ***DO NO SHARE PUBLICLY***
#
#  Identification: src/include/page/b_plus_tree_page.h
#
#  Copyright (c) 2023, Carnegie Mellon University Database Group
#
# ===----------------------------------------------------------------------===//


# ref: https://github.com/cmu-db/bustub/blob/master/script/formatting/formatter.py


import argparse
import logging
import os
import re
import sys
import datetime
import subprocess
from functools import reduce
from helpers import CLANG_FORMAT, BUSTUB_DIR, CLANG_FORMAT_FILE, LOG,\
    clang_format, hunks_from_staged_files, hunks_from_last_commits


BUSTUB_SRC_DIR = os.path.join(BUSTUB_DIR, "src")
BUSTUB_TESTS_DIR = os.path.join(BUSTUB_DIR, "test")

# DEFAULT DIRS
DEFAULT_DIRS = []
DEFAULT_DIRS.append(BUSTUB_SRC_DIR)
DEFAULT_DIRS.append(BUSTUB_TESTS_DIR)

# header framework, dynamic information will be added inside function
header_comment_line_1 = "//===----------------------------------------------------------------------===//\n"
header_comment_line_1 += "//\n"
header_comment_line_1 += "//                         CMU-DB Project (15-445/645)\n"
header_comment_line_2 = "//                         ***DO NO SHARE PUBLICLY***\n"
header_comment_line_3 = "// "
header_comment_line_4 = "//\n"
header_comment_line_5 = "// Identification: "
header_comment_line_6 = "//\n"
header_comment_line_7 = "// Copyright (c) %d, Carnegie Mellon University Database Group\n" % datetime.datetime.now().year
header_comment_line_8 = "//\n"
header_comment_line_9 = "//===----------------------------------------------------------------------===//\n\n"

header_comment_1 = header_comment_line_1 + header_comment_line_2
header_comment_3 = header_comment_line_4
header_comment_5 = header_comment_line_6 + header_comment_line_7 \
    + header_comment_line_8 + header_comment_line_9

HEADER_REGEX = re.compile(
    r"((\/\/===-*===\/\/\n(\/\/.*\n)*\/\/===-*===\/\/[\n]*)\n\n)*")


# ==============================================
# UTILITY FUNCTION DEFINITIONS
# ==============================================


def format_file(file_path, file_hunks, update_header, clang_format_code):
    """Formats the file passed as argument."""
    file_name = os.path.basename(file_path)
    abs_path = os.path.abspath(file_path)
    rel_path_from_bustub_dir = os.path.relpath(abs_path, BUSTUB_DIR)

    with open(file_path, "r+") as file:
        file_data = file.read()

        if update_header:
            # strip old header if it exists
            header_match = HEADER_REGEX.match(file_data)
            if not header_match is None:
                LOG.info("Strip header from %s", file_name)
                header_comment = header_match.group()
                LOG.debug("Header comment : %s", header_comment)
                file_data = file_data.replace(header_comment, "")

            # add new header
            LOG.info("Add header to %s", file_name)
            header_comment_2 = header_comment_line_3 + file_name + "\n"
            header_comment_4 = header_comment_line_5\
                + rel_path_from_bustub_dir + "\n"
            header_comment = header_comment_1 + header_comment_2 \
                + header_comment_3 + header_comment_4 \
                + header_comment_5
            # print header_comment

            file_data = header_comment + file_data

            file.seek(0, 0)
            file.truncate()
            file.write(file_data)

        elif clang_format_code:
            clang_format(file_path, file_hunks)

    # END WITH
# END FORMAT__FILE(FILE_NAME)


def format_dir(dir_path, update_header, clang_format_code):
    """Formats all the files in the dir passed as argument."""
    for subdir, _, files in os.walk(dir_path):  # _ is for directories.
        for file in files:
            # print os.path.join(subdir, file)
            file_path = subdir + os.path.sep + file

            if file_path.endswith(".h") or file_path.endswith(".cpp"):
                format_file(file_path, None, update_header, clang_format_code)
            # END IF
        # END FOR [file]
    # END FOR [os.walk]
# END ADD_HEADERS_DIR(DIR_PATH)


# ==============================================
# Main Function
# ==============================================

if __name__ == '__main__':

    PARSER = argparse.ArgumentParser(
        description='Update headers and/or format source code'
    )

    PARSER.add_argument(
        "-u", "--update-header",
        help='Action: Update existing headers or add new ones',
        action='store_true'
    )
    PARSER.add_argument(
        "-c", "--clang-format-code",
        help='Action: Apply clang-format to source code',
        action='store_true'
    )
    PARSER.add_argument(
        "-f", "--staged-files",
        help='Action: Apply the selected action(s) to all staged files (git). ' +
        '(clang-format will only touch the staged lines)',
        action='store_true'
    )
    PARSER.add_argument(
        "-n", "--number-commits",
        help='Action: Apply the selected action(s) to all changes of the last ' +
        '<n> commits (clang-format will only touch the changed lines)',
        type=int, default=0
    )
    PARSER.add_argument(
        'paths', metavar='PATH', type=str, nargs='*',
        help='Files or directories to (recursively) apply the actions to'
    )

    ARGS = PARSER.parse_args()

    # TARGETS is a list of files with an optional list of hunks, represented as
    # pair (start, end) of line numbers, 1 based.
    # element of TARGETS: (filename, None) or (filename, [(start,end)])

    if ARGS.staged_files:
        TARGETS = hunks_from_staged_files()

        if not TARGETS:
            LOG.error(
                "no staged files or not calling from a repository -- exiting"
            )
            sys.exit("no staged files or not calling from a repository")

    elif ARGS.number_commits > 0:
        TARGETS = hunks_from_last_commits(ARGS.number_commits)

        if not TARGETS:
            LOG.error(
                "no changes could be extracted for formatting -- exiting"
            )
            sys.exit("no changes could be extracted for formatting")

    elif not ARGS.paths:
        LOG.error("no files or directories given -- exiting")
        sys.exit("no files or directories given")

    else:
        TARGETS = [(f, None) for f in ARGS.paths]

    for f, hunks in TARGETS:
        if os.path.isfile(f):
            LOG.info("Scanning file: %s", f)
            format_file(f, hunks, ARGS.update_header, ARGS.clang_format_code)
        elif os.path.isdir(f):
            LOG.info("Scanning directory %s", f)
            format_dir(f, ARGS.update_header, ARGS.clang_format_code)
    # FOR
# IF
