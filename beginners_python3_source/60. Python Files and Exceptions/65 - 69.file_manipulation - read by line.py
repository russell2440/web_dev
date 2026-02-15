import re

def readingLines():
    count = 0

    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                count += 5
                print("Line " + str(count) + ": " + line)
    except OSError as booboo:
         
          print("We had a booboo!!")
          print(booboo)
          
def readingLinesAndSearch():

    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                if line.rstrip() == "Lucy":
                    print("-> We found Lucy!\n")                   
                else:
                    print(line)
                    
    except OSError as booboo:
         
          print("We had a booboo!!")
          print(booboo)


def readingLinesWithFind():

    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            for line in contents:
                hit = line.find("Lucy")
                
                if hit != -1:
                    print("-> We found Lucy in the line of text!\n")                   
                else:
                    print(line)
                    
    except OSError as booboo:
         
          print("We had a booboo!!")
          print(booboo)


def regExMagic(pattern, string):
# regEx - the search() function checks for a match anywhere in the string
   objectMatch = re.search(pattern, string)
   return objectMatch
         
def readingLinesWithRegEx():

    try:
        with open('people.txt') as file_object:
            contents = file_object.readlines()
            print("\n")
            for line in contents:

                pattern = 'LUcy$'
                if str(regExMagic(pattern, line)) == "None": 
                    print(line)
                else:
                    print("-- > We the text: " + pattern + ", in this line: " + line + "\n")
                    
    except OSError as booboo:
         
          print("We had a booboo!!")
          print(booboo) 

readingLinesWithRegEx()
