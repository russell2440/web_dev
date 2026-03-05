
print("//===========================================================================")
print("//===========================================================================\n")


#
# 1. OOP basic concepts 
# 2. Creating our first class
# 3. OOP - methods and init
#

class car:
    ''' A simple class that describes a car. '''

    def __init__(self, model, cost):
        ''' Initialize the car object. '''
        self.model = model
        self.cost = cost

    def start(self):
        ''' Starts the car. '''
        print(self.model.title() + " is now starting")


# class vs instance
car1 = car("toyota", 10000)
car1.start()

print("\nCreating a new car ... \n")

car2 = car("honda", 20000)
car2.start()

print("//===========================================================================")
print("//===========================================================================\n")


#
# 4. OOP - tkinter - basic gui and objects
# 5. OOP - tkinter gui with an image
#

# from tkinter import *
# import time
# 
# root = Tk()
# 
# python_course_logo = PhotoImage(file="python.gif")
# right_label = Label(root, image=python_course_logo)
# right_label.pack(side=RIGHT)
# 
# python_course_text = """With Tkinter, you can only use GIF images. There are other more powerfull Python packages that allow you use other types of images.?"""
# left_label = Label(root,
#                    justify=RIGHT,
#                    padx = 10,
#                    text=python_course_text).pack(side=LEFT)
# 
# print("Launching the window...")
# root.mainloop()
# time.sleep(1)
# 
# print("//===========================================================================")
# print("//===========================================================================\n")


#
# 6: MyDoctor - class skeleton
# 7. MyDoctor - instantiate the class
# 8. MyDoctor - nerd class details
# 9. MyDoctor - docstring and __doc__
# 

class my_doctor:
    ''' A class of Doctor that has a name. '''
    def __init__(self, name):
        self.name = name

    def say_hi(self):
        print("Hi, I'm " + self.name)

    def say_goodbye(self):
        print("Goodbye, I'm " + self.name)

    def ask_doc_question(self):
        ''' Asks a question of the doctor. '''
        question = input("Do you want a health tip? y/n: ")
        if question == "y":
            print("Good! Eat less sugar and more protein!")
        else:
            print("Ok, I'll leave you alone.")

doctor = my_doctor("Dr. Smith")
doctor.say_hi()
doctor.ask_doc_question()
doctor.say_goodbye()
print("//===========================================================================")

print(my_doctor.__doc__)
print(my_doctor.ask_doc_question.__doc__)
print(my_doctor.ask_doc_question)

print("//===========================================================================")
print("//===========================================================================\n")


#
# 11. OO - inheritance basics
# 12. OO overriding __str__ special method
# 13. Method and function default values
#

class autombile:
    ''' Automobile base/parent class. '''

    model_year = 2019

    def start(self):
        ''' Starts the autombile. '''
        #print(self.model.title() + " is now starting")
        print("Auto is now starting")

    def turn_off(self):
        ''' Turns off the autombile. '''
        #print(self.model.title() + " is now off")
        print("Auto is now off")


class truck(autombile):
    ''' A truck class that inherits from the autombile class. '''

    def __init__(self, year=None):
        ''' Initialize the truck object. '''
        if year is None:
            self.year = 2014
        else:
            self.year = year

    def __str__(self):
        ''' Returns the truck's title. '''
        return str(self.year) + " Chevy Silverado 1500 LT"

    def truck_year(self):
        ''' Returns the truck's year. '''
        print("Truck's year is " + str(self.year))

    def dump_load(self, weight=None):
        ''' Dumps the truck load. '''
        #print(self.model.title() + " is now dumping it's load")
        if weight is None:
            print("Truck is now dumping it's load of nothing: what a waste!")
        else:
            print("Truck is now dumping it's load with a weight of " + str(weight))



my_truck = truck(2018)
my_truck.truck_year()
my_truck.start()
my_truck.dump_load(1024)
my_truck.turn_off()
print(my_truck)
print(type(my_truck))
print("//===========================================================================")

my_truck = truck()
my_truck.truck_year()
my_truck.start()
my_truck.dump_load()
my_truck.turn_off()
print(my_truck)
print(type(my_truck))

print("//===========================================================================")
print("//===========================================================================\n")


# 14. Creating and using Modules 

''' Program controller Python file. '''
import module_one, module_two, time

print("Three modules have been imported.")

# calling functions directly from a module
module_one.a_function()

module_two.a_function()

time.sleep(1)

# using a class from a module
my_dog = module_one.dog()

my_dog.bark()
my_dog.spawn_window()


