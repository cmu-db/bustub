#!/usr/bin/env python3
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

import argparse
import codecs
import difflib
import fnmatch
import os
import subprocess
import sys


def check(arguments, source_dir):
    formatted_filenames = []
    error = False
    for directory, subdirs, filenames in os.walk(source_dir):
        fullpaths = (os.path.join(directory, filename)
                     for filename in filenames)
        source_files = [x for x in fullpaths
                        if x.endswith(".h") or x.endswith(".cpp")]
        formatted_filenames.extend(
            # Filter out files that match the globs in the globs file
            [filename for filename in source_files
             if not any((fnmatch.fnmatch(filename, exclude_glob)
                         for exclude_glob in exclude_globs))])

    if arguments.fix:
        if not arguments.quiet:
            # Print out each file on its own line, but run
            # clang format once for all of the files
            print("\n".join(map(lambda x: "Formatting {}".format(x),
                                formatted_filenames)))
        subprocess.check_call([arguments.clang_format_binary,
                               "-i"] + formatted_filenames)
    else:
        for filename in formatted_filenames:
            if not arguments.quiet:
                print("Checking {}".format(filename))
            #
            # Due to some incompatibilities between Python 2 and
            # Python 3, there are some specific actions we take here
            # to make sure the difflib.unified_diff call works.
            #
            # In Python 2, the call to subprocess.check_output return
            # a 'str' type. In Python 3, however, the call returns a
            # 'bytes' type unless the 'encoding' argument is
            # specified. Unfortunately, the 'encoding' argument is not
            # in the Python 2 API. We could do an if/else here based
            # on the version of Python we are running, but it's more
            # straightforward to read the file in binary and do utf-8
            # conversion. In Python 2, it's just converting string
            # types to unicode types, whereas in Python 3 it's
            # converting bytes types to utf-8 encoded str types. This
            # approach ensures that the arguments to
            # difflib.unified_diff are acceptable string types in both
            # Python 2 and Python 3.
            with open(filename, "rb") as reader:
                # Run clang-format and capture its output
                formatted = subprocess.check_output(
                    [arguments.clang_format_binary,
                     filename])
                formatted = codecs.decode(formatted, "utf-8")
                # Read the original file
                original = codecs.decode(reader.read(), "utf-8")
                # Run the equivalent of diff -u
                diff = list(difflib.unified_diff(
                    original.splitlines(True),
                    formatted.splitlines(True),
                    fromfile=filename,
                    tofile="{} (after clang format)".format(
                        filename)))
                if diff:
                    print("{} had clang-format style issues".format(filename))
                    # Print out the diff to stderr
                    error = True
                    sys.stderr.writelines(diff)
    return error


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Runs clang format on all of the source "
        "files. If --fix is specified,  and compares the output "
        "with the existing file, outputting a unifiied diff if "
        "there are any necessary changes")
    parser.add_argument("clang_format_binary",
                        help="Path to the clang-format binary")
    parser.add_argument("exclude_globs",
                        help="Filename containing globs for files "
                        "that should be excluded from the checks")
    parser.add_argument("--source_dirs",
                        help="Comma-separated root directories of the code")
    parser.add_argument("--fix", default=False,
                        action="store_true",
                        help="If specified, will re-format the source "
                        "code instead of comparing the re-formatted "
                        "output, defaults to %(default)s")
    parser.add_argument("--quiet", default=False,
                        action="store_true",
                        help="If specified, only print errors")

    args = parser.parse_args()

    had_err = False
    exclude_globs = [line.strip() for line in open(args.exclude_globs)]
    for source_dir in args.source_dirs.split(','):
        if len(source_dir) > 0:
            had_err = check(args, source_dir)

    sys.exit(1 if had_err else 0)
