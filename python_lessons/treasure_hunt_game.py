# Treasure Hunt Game

import random
import time

def display_game_intro():
    print('''

        -----> Welcome to the 'PythonTreasure Hunt Game!'
        .... the most amazing game in the world!

        After a long journey, you have fond yourself in front of two caves.
        One cave leads to a treasure, and the other leads to a spike filled pit.
        Being brave, and a little git greedy for trasure, you've decided to
        explore the caves and find the treasure.

        ''');


def choose_cave():
    cave = ''
    while cave != "1" and cave != "2":
        print("What cave are you going to choose? [1] or [2]")
        cave = input()
    return cave


def enter_cave(chosen_cave):
    print("\nYou have entered a cave...");
    time.sleep(1)

    random_cave = random.randint(1, 2)

    if int(chosen_cave) == random_cave:
        print("----> You have found the treasure!!!!\n")
    else:
        print("----> You were hoping to find treasure\n ...and you have found DEATH! in a spike filled pit.\n")


def main_loop():
    ''' The main loop() function controls the flow of the game by calling functions and using conditionals.'''

    play_game_again = 'yes';

    while play_game_again == 'yes' or play_game_again == 'y':

        display_game_intro()
        what_cave = choose_cave()
        enter_cave(what_cave)

        play_game_again = input("\n\nDo you want to play again? [yes or y] ")
        time.sleep(1)

        if play_game_again == 'yes' or play_game_again == 'y':
            print("\nLets play again!")
        else:
            print("\nOK, see you later!\n")


main_loop()
