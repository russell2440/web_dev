

# Introduction to Drawing with Turtle
import turtle

print("Introduction to Drawing with Turtle")
t = turtle.Pen()

t.forward(200)
t.left(45)
t.forward(150)
t.left(45)
t.forward(340)
t.left(180)
t.forward(340)
t.right(90)
t.forward(234)
t.reset()
print("done\n")

'''
turtle.exitonclick()
while True:
     t.hideturtle()
     t.goto(-100, 100)
     t.showturtle()
     t.write("Hello World!")
     t.left(90)
'''

# Python loops Introduction
name = "Russell"
for x in name:
    print("x is: ", x)
print("done\n")

# Python loops with range
for x in range(5):
    print("x is: ", x)
print("done\n")

# Python loops with range
for x in range(1, 10):
    print("x is: ", x)

# Draw a box.
print("Drawing a box")
for x in range(1,5):
    t.forward(50)
    t.left(90)
    t.forward(100)
t.reset()
print("done\n")

# Draw a mystery object.
print("Drawing a mystery object")
for x in range(1,9):
    t.forward(100)
    t.left(225)
t.reset()
print("done\n")

# Drawing a star with loops and colors
print("Drawing a star with loops and colors")

t.color("red", "blue")
t.begin_fill()

count = 0
for x in range(1, 777):
    t.forward(300)
    t.left(225)
    count += 1
    print("count is: ", str(count))
    print("x is: ", str(x))

    # stop drawing after 8 loops
    if count > 7 and count < 9:
        print("the star pattern is complete")
        break

print("count is: ", str(count))
print("x is: ", str(x))

t.end_fill()

t.reset()
print("done\n")

