
CC = g++
GFLAGS= -g -Wall -std=c++11

#openGL
lIBOPENGL= -lGL -lGLU -lGLEW -lglut -lm 

#armadilo
LIBARMADILO= -DARMA_DONT_USE_WRAPPER -lopenblas -llapack

EXEC= courbes
SRC= courbes.cpp 


$(EXEC): $(SRC)
	$(CC) -g -Wall -std=c++11 -o $(EXEC) $(SRC) $(lIBOPENGL) $(LIBARMADILO)

all: 
	clean 
	$(EXEC)

clean:
	rm -rf *.o
	rm -rf $(EXEC)
