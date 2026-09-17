# docstrings

class MyDoctor:
    '''-> MyDoctor class docstring ... tell programmers about your class.'''     
    
    def sayHi(self):
        print("Hi!")

    def sayBye(self):
        '''-> sayBye() docstring ...'''
        print("Bye")

    def askDocQuestion(self):
        question = input("Do you want a health tip? Yes or no.")

        #if yes, python says: Good! Eat less sugar.
        #if !yes, Ok, see you next time.

        if question == "yes" or question == "Yes":
            print("Good! Eat less sugar.")
        else:
            print("Ok, see you next time.") 

myDoctor = MyDoctor()

print(myDoctor.__doc__)
print(myDoctor.sayBye.__doc__)

#myDoctor.sayHi()
#myDoctor.askDocQuestion()
print(myDoctor.sayBye)





