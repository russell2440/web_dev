# Lists introduction
shopping_list = ["apples", 15.46, "beef", "bananas","beef", 5.56, "milk", "eggs", "feb 2026"]
print(shopping_list)
# Lists append, reverse, and count
shopping_list.append("mellons")
print(shopping_list)
shopping_list.reverse()
print(shopping_list)
print(shopping_list.count("apples"))
print(shopping_list.count("beef"))

# Lists lenth and delete
names = ["John", "Jane", "Jack", "Jill"]
print(names)
dogs = ["Red", "Barfy", "Snickers", "Fido"]
print(dogs)
dogs.pop(1)
print(dogs)
dogs.append("Barfy")
print(dogs)
print(len(dogs))

dogs_and_names = dogs + names
print(dogs_and_names)
print(len(dogs_and_names))

major_names = names*10
print(major_names)
print(len(major_names))

# Tuples
my_tuple = ("apples", 15.46, "beef", "bananas","beef", 5.56, "milk", "eggs", "feb 2026")
print(my_tuple)
print(len(my_tuple))
del my_tuple
#### print(my_tuple)

# Dictionariesa / Maps
my_map = {"name_first": "Russ", "name_last": "Shahenian", "age": 66, "city": "Mesa", "state": "AZ"}
print(my_map)
print(len(my_map))
print(my_map["name_first"])
print(my_map["name_last"])
print(my_map["age"])
print(my_map["city"])

shopping_cart = {"apples": 2, "bananas": 3, "milk": 1, "eggs": 4}
print(shopping_cart)
shopping_cart["coffee"] = 10
print(shopping_cart)
shopping_cart["kale"] = 5
print(shopping_cart)
shopping_cart["apples"] = 5
print(shopping_cart)
del shopping_cart["coffee"]
print(shopping_cart)
