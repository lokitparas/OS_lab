import io
import random
import string
for x in range(1000):
	f = open("./files/foo" + str(x) + ".txt","wb")
	f.seek(2097152-1)
	f.write(random.choice(string.letters)+" ")
	f.close()
