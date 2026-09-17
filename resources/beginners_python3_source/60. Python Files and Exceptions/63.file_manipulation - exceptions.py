# file manipulation and exception handling: try, except and else

# Mac path: /Users/stefanmischook/Desktop/python ch8/people2.txt
# Win path: c:\Users\(username)\Desktop\python ch8\people2.txt

class Filer():

    def openFile(self):
        with open('system_config.txt') as file_object:
            contents = file_object.read()
            print(contents)

    #open file function with argument
    def openFile2(self,file):
        try:
            with open(file) as file_object:
                contents = file_object.read()
                print(contents)
        except FileNotFoundError:
                print("\nERROR: We had trouble reading the file.")
        else:
                print("\nWe were able to access the file. Cool!")



myFiler = Filer()
myFiler.openFile2('/Users/stefanmischook/Desktop/python ch8/people.txt')
