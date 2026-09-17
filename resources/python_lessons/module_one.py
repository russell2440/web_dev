
''' Module one. '''
import turtle, tkinter

def a_function():
    ''' A function in module one. '''
    print("This is a function in module one.")

    
class dog:
    ''' A class in module one. '''
    def __init__(self):
        ''' Initialize the dog object. '''
        print("This is a class in module one.")
        
    def bark(self):
        ''' Barks the dog. '''
        print("Woof!")
        
        
    def draw_square(self):
        ''' Draws a square on the screen. '''
        print("Drawing a square...")
        turtle.forward(100)
        turtle.left(90)
        turtle.forward(100)
        turtle.left(90)
        turtle.forward(100)
        turtle.left(90)
        turtle.forward(100)
        turtle.left(90)
        print("Done drawing a square.")

    def spawn_window(self):
        ''' Spawns a window. '''
        print("Spawning a window...")

        tk = tkinter.Tk()
        button = tkinter.Button(tk, text="Click to draw square!", command=self.draw_square)
        button.pack()
        tk.mainloop()


