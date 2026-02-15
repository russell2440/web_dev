'''
Types of Operators:

Arithmetic
Comparison
Assignment
Logical

Bitwise
Membership
Identity

'''


import turtle
t = turtle.Pen()

t.color('blue','green')
t.begin_fill()

count = 0

#draw a star
for x in range(1,333):
    t.forward(300)
    t.left(225)
    count = count+1
    print("Count is: " + str(count))
    
    #stop drawing after 8 loops
    #if count > 7 and count > 12:
    if count > 7:
        print("The star pattern is complete!")
        break
    
    
t.end_fill()
print("A star is born!")
