name = "stefan mischook"
result = name.endswith('ook')
print(result)

# This is just a quick way (using the . operator,)
# to apply a method to an object. That's why you can't do this:
# name.upper()
# print(name)
# ... It will NOT be uppercase. 
#
nameCap = name.upper()
print("Did I capitalize: " + nameCap)


# strings are objects in python
#strings are immutable in python
print(id(name))
print(type(name))

print("\n New string created ...")

name = name + "Jimmys fish"
print(id(name))
print(type(name))
