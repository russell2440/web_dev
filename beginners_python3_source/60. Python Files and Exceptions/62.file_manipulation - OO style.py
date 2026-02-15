# file manipulation

'''with open('system_config.txt') as file_object:
        contents = file_object.read()
        print(contents)'''

#open file function hard code file name
def openFile():
    with open('system_config.txt') as file_object:
        contents = file_object.read()
        print(contents)

#open file function with argument
def openFile2(file):
    with open(file) as file_object:
        contents = file_object.read()
        print(contents)     

class Filer():

    def openFile(self):
        with open('system_config.txt') as file_object:
            contents = file_object.read()
            print(contents)

    #open file function with argument
    def openFile2(self,file):
        with open(file) as file_object:
            contents = file_object.read()
            print(contents)     

myFiler = Filer()
myFiler.openFile()
myFiler.openFile2('system_config.txt')
