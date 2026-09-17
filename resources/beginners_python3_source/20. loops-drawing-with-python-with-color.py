import turtle
t = turtle.Pen()
t.color('blue','green')


#draw a star

t.begin_fill()
count = 0

for x in range(1,19):
    t.forward(300)
    t.left(225)
    count = count+1
    print("Count is: " + str(count))
    
    #display position of turtle
    print(str(t.pos()))
    if abs(t.pos()) < 1:
        print("The star pattern is complete!")
        break
    
    
t.end_fill()
print("A star is born!")
