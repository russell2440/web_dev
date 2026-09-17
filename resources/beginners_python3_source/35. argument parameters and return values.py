# arguments, parameter, variable scope and return values


def myFunction(name):
    print("You're name is: " + name)


def getName():
    name = input("What is your name:")
    return name


def runit():
    print("Start the app ...")
    myFunction(getName())

# run the program
runit()

# global variable scope 
name = getName()
print(name)
