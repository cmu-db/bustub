#!/usr/bin/env python3

from datetime import datetime
from pathlib import Path
import subprocess


def request_yn(buffer, msg):
    print(msg, end="")
    print(" [y/N] ", end="")
    yn = input()
    assert yn == "y"
    buffer += msg
    buffer += "\n"
    return buffer


def request_sign(buffer, msg):
    print(msg, end="")
    print(" ", end="")
    data = input()
    assert len(data) >= 2
    buffer += msg
    buffer += " "
    buffer += data
    buffer += "\n"
    return buffer


def request_date(buffer, msg):
    print(msg, end="")
    buffer += msg
    date = datetime.today().strftime("%Y-%m-%d")
    print(" " + date)
    buffer += " "
    buffer += date
    buffer += "\n"
    return buffer


def add_to_submission():
    for item in Path().iterdir():
        name = item.name
        if name.startswith("project") and name.endswith("-submission.zip"):
            print("adding GRADESCOPE.md to", name)
            subprocess.run(["zip", name, "GRADESCOPE.md"])


def main():
    if Path("GRADESCOPE.md").exists():
        print("Found existing signature GRADESCOPE.md, adding to all submissions...")
        add_to_submission()
        print(
            "If you want to make modifications to signed document, run `rm GRADESCOPE.md` and then run this script again."
        )
        return
    buffer = ""
    buffer = request_yn(
        buffer,
        "To create a fair learning environment for all students and avoid potential Academic Integrity Violations within Carnegie Mellon University, we ask you to sign this short agreement before submitting to Gradescope.",
    )
    buffer += "\n"
    buffer = request_yn(
        buffer,
        "1. I agree NOT to make my solution public. i.e., DO NOT make it public on GitHub or create videos explaining the solution code.",
    )
    buffer = request_yn(
        buffer,
        "2. I agree NOT to hack Gradescope. i.e., DO NOT retrieve private test cases from Gradescope, or bypass correctness checks.",
    )
    buffer = request_yn(
        buffer,
        "3. I affirm that the work I submit for assessment is my original work. i.e., DO NOT purchase/copy solution from others.",
    )
    buffer = request_yn(
        buffer,
        "4. I understand that the course staff does not provide official help for students outside CMU. i.e., DO NOT email the course staff or create GitHub issues for course-related questions. Use the unofficial Discord server.",
    )
    buffer = request_yn(
        buffer,
        "I understand that if I violate the rules, I will be banned from using Gradescope.",
    )
    buffer += "\n"
    buffer = request_sign(buffer, "Name:")
    buffer = request_sign(buffer, "Affiliation (School/Company):")
    buffer = request_sign(buffer, "Email:")
    buffer = request_sign(buffer, "GitHub ID:")
    buffer = request_date(buffer, "Date:")
    buffer += "\n"
    buffer = request_yn(
        buffer,
        "I understand that if I provide a fake signature, I will be banned from using Gradescope.",
    )
    print()
    print("--- THIS IS A COPY OF THE SIGNED DOCUMENT ---")
    print(buffer)
    print("--- END OF THE SIGNED DOCUMENT ---")
    print()
    print(
        "Saving the signature to GRADESCOPE.md and adding the signed document to submission zips..."
    )
    Path("GRADESCOPE.md").write_text(buffer)
    add_to_submission()
    print("Run this script again if you create new submission zips.")


if __name__ == "__main__":
    try:
        main()
    except KeyboardInterrupt:
        print("\nthe sign process was aborted")
