
print("//===========================================================================")
print("//===========================================================================\n")

#
# 1. Opening Files with Python 
# 2. Opening Files - creating functions 
#


with open('system_config.txt', 'r') as file_object:
    contents = file_object.read()
    print(contents)

print("//===========================================================================\n")

def print_file_contents_hardcoded():
    with open('system_config.txt', 'r') as file_object:
        contents = file_object.read()
        print(contents)

def print_file_contents(file_name):
    with open(file_name, 'r') as file_object:
        contents = file_object.read()
        print(contents)


print_file_contents_hardcoded()
print_file_contents('system_config.txt')

print("//===========================================================================")
print("//===========================================================================\n")

#
# 3. Opening Files - creating a class
# 4. Opening Files - exceptions
#

class file_ops:
    def print_file_contents_hardcoded(self):
        with open('system_config.txt', 'r') as file_object:
            contents = file_object.read()
            print(contents)
    
    def print_file_contents(self, file_name):
        try:
            with open(file_name, 'r') as file_object:
                contents = file_object.read()
                print(contents)
        except Exception as e:
            print("\nError: we had trouble reading the file.")
            print(e)
        else:
            print("\nCool: We were able to access the file!")
    
    def write_file_contents(self, file_name, text):
        try:
            with open(file_name, 'w') as file_object:
                file_object.write(text)
        except Exception as e:
            print("\nError: we had trouble reading the file.")
            print(e)
        else:
            print("\nCool: We were able to access the file!")

    def append_file_contents(self, file_name, text):
        try:
            with open(file_name, 'a') as file_object:
                file_object.write(text)
        except Exception as e:
            print("\nError: we had trouble reading the file.")
            print(e)
        else:
            print("\nCool: We were able to access the file!")

my_file_ops = file_ops()
print("\n>>> Calling print_file_contents_hardcoded...")
my_file_ops.print_file_contents_hardcoded()
print("\n>>> Calling print_file_contents...")
my_file_ops.print_file_contents('system_config.txt')
print("\n>>> Calling print_file_contents...")
my_file_ops.print_file_contents('SomeFileThatDoesNotExist.txt')
print("\n>>> Calling print_file_contents...")
my_file_ops.print_file_contents('people.txt')
print("\n>>> Calling write_file_contents...")
my_file_ops.write_file_contents('my_file.txt', 'Here is some text to write to the file.\n')
print("\n>>> Calling append_file_contents...")
my_file_ops.append_file_contents('my_file.txt', 'Here is some text to append to the file.\n')
print("\n>>> Calling print_file_contents...")
my_file_ops.print_file_contents('my_file.txt')

print("//===========================================================================")
print("//===========================================================================\n")



#
# 6. Advanced exceptions / error handling 
# 7. Reading files line by line
# 9. Searching files - regular expressions 
#

def reading_lines():
    count = 0
    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                count += 5
                print("Line #" + str(count) + ":" + line)
    except Exception as e:
        print("\nError: we had trouble reading the file.")
        print(e)
    else:
        print("\nCool: We were able to access the file!")
        print("Total number of lines: "+str(count))



def reading_lines_with_find():
    count = 0
    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                count += 1
                if line.find('Lucy') == -1:
                    print("NOPE:    Line #" + str(count) + ":" + line)
                else:
                    print("FOUND:   Line #" + str(count) + ":" + line)
    except Exception as e:
        print("\nError: we had trouble reading the file.")
        print(e)
    else:
        print("\nCool: We were able to access the file!")
        print("Total number of lines: "+str(count))



import re
def reading_lines_with_regex():
    count = 0
    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                count += 1
                pattern = 'Lucy'
                if re.search(pattern, line) == None:
                    print("NOPE:    Line #" + str(count) + ":" + line)
                else:
                    print("FOUND:   Line #" + str(count) + ":" + line)
                if re.search('^'+pattern, line) == None:
                    print("NOPE:    Line #" + str(count) + ":" + line)
                else:
                    print("FOUND:   Line #" + str(count) + ":" + line)
                if re.search(pattern+'$', line) == None:
                    print("NOPE:    Line #" + str(count) + ":" + line)
                else:
                    print("FOUND:   Line #" + str(count) + ":" + line)
                print("---")
    except Exception as e:
        print("\nError: we had trouble reading the file.")
        print(e)
    else:
        print("\nCool: We were able to access the file!")
        print("Total number of lines: "+str(count))

print("\n>>> Calling reading_lines...")
reading_lines()
print("\n>>> Calling reading_lines_with_find...")
reading_lines_with_find()
print("\n>>> Calling reading_lines_with_find...")
reading_lines_with_find()
print("\n>>> Calling reading_lines_with_regex...")
reading_lines_with_regex()

print("//===========================================================================")
print("//===========================================================================\n")
