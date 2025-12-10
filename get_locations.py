"""
TODO: Make this script make it own venv
"""

import os
import subprocess
import sys


def main(target):
    res = subprocess.run(
        ["venv/bin/python3", "get_locations.py", f"{target}"],
        stdout=subprocess.PIPE,
        cwd="GoogleFindMyTools",
    )
    return res.stdout.decode("utf-8").replace("\n", "")


if __name__ == "__main__":
    try:
        assert len(sys.argv) == 2, "Wrong amount of arguments"
        assert sys.argv[1].isdigit(), "Second argument is not valid"
    except AssertionError as e:
        print("Expected input to be a single int to specify which index the thing need")
        raise e

    print(main(int(sys.argv[1])))
