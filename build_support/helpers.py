#!/usr/bin/env python3
"""Common helper functions to be used in different Python scripts."""
import difflib
import distutils.spawn
import logging
import os
import subprocess
import re

from functools import reduce

CODE_SOURCE_DIR = os.path.abspath(os.path.dirname(__file__))
BUSTUB_DIR = CODE_SOURCE_DIR.replace('/build_support', '')
CLANG_FORMAT_FILE = os.path.join(BUSTUB_DIR, ".clang-format")

FORMATTING_FILE_WHITELIST = [
    # Fill me
]

DIFF_FILE_PATT = re.compile(r'^\+\+\+ b\/(.*)')
DIFF_HUNK_PATT = re.compile(r'^@@ \-\d+(,\d+)? \+(\d+)(,)?(\d+)? @@.*')

# ==============================================
# LOGGING CONFIGURATION
# ==============================================

LOG = logging.getLogger(__name__)
LOG_HANDLER = logging.StreamHandler()
LOG_FORMATTER = logging.Formatter(
    fmt='%(asctime)s [%(funcName)s:%(lineno)03d] %(levelname)-5s: %(message)s',
    datefmt='%m-%d-%Y %H:%M:%S'
)
LOG_HANDLER.setFormatter(LOG_FORMATTER)
LOG.addHandler(LOG_HANDLER)
LOG.setLevel(logging.INFO)


def find_clangformat():
    """Finds appropriate clang-format executable."""
    # check for possible clang-format versions
    path = ""
    for exe in ["clang-format", "clang-format-13", "clang-format-14"]:
        path = distutils.spawn.find_executable(exe)
        if not path is None:
            break
    return path


CLANG_FORMAT = find_clangformat()
CLANG_COMMAND_PREFIX = [CLANG_FORMAT, "-style=file"]


def clang_check(file_path, hunks=None):
    """Checks and reports bad code formatting."""

    assert not file_path is None and not file_path == ""

    rel_path_from_peloton_dir = os.path.relpath(file_path, BUSTUB_DIR)

    if rel_path_from_peloton_dir in FORMATTING_FILE_WHITELIST:
        return True

    file_status = True

    # Run clang-format on the file and get output (not inline!)
    formatted_src = clang_format(file_path, None, inline=False)

    # For Python 3, the above command gives a list of binary sequences, each
    # of which has to be converted to string for diff to operate correctly.
    # Otherwise, strings would be compared with binary sequences and there
    # will always be a big difference.
    formatted_src = [line.decode('utf-8') for line in formatted_src]
    # Load source file
    with open(file_path, "r") as file:
        src = file.readlines()

        # Do the diff
        difference = difflib.Differ()
        diff = difference.compare(src, formatted_src)
        line_num = 0
        for line in diff:
            code = line[:2]
            if code in ("  ", "- "):
                line_num += 1
            if code == '- ':
                if file_status:
                    LOG.info("Invalid formatting in file : " + file_path)
                LOG.info("Line %d: %s", line_num, line[2:].strip())
                file_status = False

        return file_status


def clang_format(file_path, hunks=None, inline=True):
    """Formats the file at file_path. 
       'hunks' can be a list of pairs with (start,end) line numbers, 1 based. 
    """

    assert not file_path is None and not file_path == ""

    if CLANG_FORMAT is None:
        LOG.error("clang-format seems not installed")
        exit()

    formatting_command = CLANG_COMMAND_PREFIX + [file_path]

    if inline:
        formatting_command.append("-i")

    if not hunks is None:
        for start, end in hunks:
            if start > 0 and end > 0:
                formatting_command.append("-lines={}:{}".format(start, end))

    LOG.info(' '.join(formatting_command))
    output = subprocess.check_output(formatting_command).splitlines(True)
    return output


def hunks_from_last_commits(n):
    """ Extract hunks of the last n commits. """

    assert n > 0

    diff_output = subprocess.check_output(["git", "diff", "HEAD~"+str(n), "--diff-filter=d", "--unified=0"]
                                          ).decode("utf-8").splitlines()

    return _hunks_from_diff(diff_output)


def hunks_from_staged_files():
    diff_output = subprocess.check_output(["git", "diff", "HEAD",
                                           "--cached", "--diff-filter=d", "--unified=0"]
                                          ).decode("utf-8").splitlines()

    return _hunks_from_diff(diff_output)


def _hunks_from_diff(diff_output):
    """ Parse a diff output and extract the hunks of changed files. 
        The diff output must not have additional lines!
        (use --unified=0) """

    # TARGETS is a list of files with an optional list of hunks, represented as
    # pair (start, end) of line numbers, 1 based.
    # element of TARGETS: (filename, None) or (filename, [(start,end)])
    target_files = []

    # hunks_current_list serves as a reference to the hunks list of the
    # last added file
    hunks_current_list = []

    for line in diff_output:
        file_match = DIFF_FILE_PATT.search(line)
        hunk_match = DIFF_HUNK_PATT.search(line)
        if file_match:
            file_path = os.path.abspath(os.path.join(BUSTUB_DIR,
                                                     file_match.group(1)))

            hunks_current_list = []
            if file_path.endswith(".h") or file_path.endswith(".cpp"):
                target_files.append((file_path, hunks_current_list))
            # If this file is not .cpp/.h the hunks_current_list reference
            # will point to an empty list which will be discarded later
        elif hunk_match:
            # add entry in the hunk list of the last file
            if hunk_match.group(4) is None:
                hunk = (int(hunk_match.group(2)), int(hunk_match.group(2)))
            else:
                hunk = (int(hunk_match.group(2)), int(hunk_match.group(2)) +
                        int(hunk_match.group(4)))
            hunks_current_list.append(hunk)

    return target_files
