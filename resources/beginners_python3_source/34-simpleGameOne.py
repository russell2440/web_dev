import time

# define functions:

def displayLesson():
    time.sleep(1)
    print('''
This is a multiline text string ...
I can write it across several lines. You use triple quotes
to make it happen.

.... The program has ended.''')

def useTime():
    print("Shut down requested.")
    time.sleep(1)
    print("3 seconds to shutdown ...")
    time.sleep(2)
    print("Going offline ...")

def flowControl():
    answer = input("Do you want to learn about multiline text strings? (yes or no)\n -> ")

    #if answer == "yes" or answer == "y":
    if answer == ("yes" or "y"):
        displayLesson()
    else:
        useTime()
        print("End program")
       
# execute program:
flowControl()


    

    
