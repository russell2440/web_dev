# 57 inheritance
# Python conventions:
# 1. Class names should normally use the CapWords convention.
# 2. Function names should be lowercase, with words separated by underscores as necessary to improve readability.
# 3. Always use self for the first argument to instance methods.
# 4. Mutliline comments should use the # symbol for each line. Don't use docstring - I cheated before! 

class AutoMobile:
    '''-> Automobile base / parent class'''     

    model_year = "2019"

    # special method
    def __str__(self):
        return "2019 vehicle sold by StudioWeb."
    
    def start(self):
        print("Automobile is starting ... vroom, vroom!")

    def turn_off(self):
        '''-> shut off auto ...'''
        print("Click, pud, pud ... thud. Vehicle is off.")
        

class Truck(AutoMobile):
    '''-> Truck - a type of automobile. '''     

    def dumpload(self):
        print("Truck is dumping load")

    # overriding methods
    def start(self):
        print("Truck is starting ... puda, puda, vroom!")

    def turn_off(self):
        '''-> shut off truck ...'''
        print("Click, puda, puda ... thud. Truck is off.")

    


my_truck = truck()
print("My trucks model year: " + my_truck.model_year)

# overriding default behavior with special __str__ method
print(my_truck)

my_truck.start()

# method of sub-class only
my_truck.dumpload()

# inherited method
my_truck.turn_off()






