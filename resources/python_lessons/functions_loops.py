name = input("What is your name? ")
print("Hello " + name)

def greet(name):
    print("Hello " + name)
    return "Hello " + name

greet(name)



count = 0
while count < 5:
    print(name + " " + str(count))
    count += 1



import keyword
print(keyword.kwlist)






#
# Functions, simple game and flow control
#

import time

def display_lesson():
    time.sleep(1)
    print('''
This is a multi-line text string...
I can write it across several lines. You can use tripple quotes
to make it happen.

......The program has ended.''')


def use_time():
    print("Shutdown requiested.")
    time.sleep(1)
    print("3 seconds to shutdown...")
    time.sleep(3)
    print("Going offline...")

def flow_control():
    answer = input("Do you want to learn about multi-line text strings? [yes or y] ")
    print("You entered: " + answer)
    if answer == "yes" or answer == "y":
        display_lesson()
    else:
        use_time()
        print("Goodbye.")

# Execute program:
flow_control()




#
# 7/8. Functions, arguments / parameters, return values, and variable scope
#


def print_my_name(name):
    print("Hello, your name is " + name)


def get_my_name():
    name = input("What is your name? ")
    return name
    # return "Russell Shahenian"

def run_whats_my_name_app():
    print("Start the app...")
    print_my_name(get_my_name())
    print("End of the app.")

# Run the program:
run_whats_my_name_app()


# global variable scope demo
name = get_my_name()
print("Hello, your name is " + name)





#
# 9/10. Functions – multiple parameters.
#       Flow control – if/elif/else.
#       Using functions in functions and date type conversion.
#
# len(), str()
#
def multi_para(first_name, last_name):
    print("Hello, your first name is " + first_name + ", and your last name is " + last_name)
    if first_name == "Russell":
        print("Congratulations! You are a great programmer!")
    elif first_name == "Charlie":
        print("Since your name is " + first_name + ", do you play a guitar?")
    elif first_name == "Sam":
        print("You say your name is " + first_name + ", are you are in a band?")
    else:
        print("This is a catch-all case.")
        print("Your first name has " + str(len(first_name)) + " characters.")

name = input("What is your first name? ")
multi_para(name, "Shahenian")




