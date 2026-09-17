''' Program controller Python file'''

import module_one, module_two, time

print("3 modules have been imported.")

#calling functions directly from a module
module_one.a_function()

#calling a function directly from module 2
module_two.another_function()
time.sleep(1)

#using a class from a module
my_dog = module_one.Dog()
my_dog.bark()
my_dog.dog_spawn_window()

