import os
import subprocess


def main(target):
    res = subprocess.run(
        ["venv/bin/python3", "get_locations.py", f"{target}"],
        stdout=subprocess.PIPE,
        cwd="GoogleFindMyTools",
    )
    return res.stdout.decode("utf-8").replace("\n", "")


if __name__ == "__main__":
    print(main(4))
