# Object Oriented Python - Video #45

class Car():
    ''' A simple class that describes a car '''

    def __init__(self, model, cost):
        ''' Initialize the object '''
        self.model = model
        self.cost = cost

    def start(self):
        ''' starts the car '''
        print(self.model.title() + " is now starting.")


# class vs object

myCar = Car("audi", "$98k")
myCar.start()

print("\nCreating new car ... \n")

my2ndCar = Car("ford", "$28k")
my2ndCar.start()
