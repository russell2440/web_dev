# 38. This lesson teaches: multiple parameters in functions,
# more flow control with elif, using functions in functions
# and (data)type conversion.

def multiPara(name, last):
    print("You first name is " + name + " and you last name is " + last)
    if name == "Stefan":
        print("Whoa! You're a nerd!")
    elif name == "Jimmy":
        print("Since your name is " + name + ", do you play guitar?")
    elif name == "Jethro":
        print("You say your name is " + name + ", are you in a band?")
    else:
        print("This is a catch all!")
        

multiPara("Red", "Mischook")
