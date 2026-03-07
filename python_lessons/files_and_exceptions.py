
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
        except FileNotFoundError:
            print("\nError: we had trouble reading the file.")
        except:
            print("\nError: Unexpected error:", sys.exc_info()[0])
            raise
        else:
            print("\nCool: We were able to access the file!")
    
    def write_file_contents(self, file_name, text):
        try:
            with open(file_name, 'w') as file_object:
                file_object.write(text)
        except FileNotFoundError:
            print("\nError: we had trouble writing to the file.")
        except:
            print("\nError: Unexpected error:", sys.exc_info()[0])
            raise
        else:
            print("\nCool: We were able to access the file!")

    def append_file_contents(self, file_name, text):
        try:
            with open(file_name, 'a') as file_object:
                file_object.write(text)
        except FileNotFoundError:
            print("\nError: we had trouble appending to the file.")
        except:
            print("\nError: Unexpected error:", sys.exc_info()[0])
            raise
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


