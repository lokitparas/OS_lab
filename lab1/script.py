import io
import random
for x in range(10000):
	f = open("foo" + str(x) + ".txt","wb")
	f.seek(2097152-1)
	f.write(str(random.random()) + " ")
	f.close()
