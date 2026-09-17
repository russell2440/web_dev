''' creating files on Windows '''

aFile = open("C:\\Users\\Stefan\\Desktop\\aFile.txt", "w")

for x in range(0, 10):
    aFile.write("Nick\n")
aFile.close()
