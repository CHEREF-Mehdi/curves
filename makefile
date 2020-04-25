
CC = g++
GFLAGS= -g -Wall -std=c++11

#openGL
lIBOPENGL= -lGL -lGLU -lGLEW -lglut -lm 

#armadilo
LIBARMADILO= -DARMA_DONT_USE_WRAPPER -lopenblas -llapack

EXEC= curve
SRC= curve.cpp


$(EXEC): $(SRC)
	$(CC) $(GFLAGS) -o $(EXEC) $(SRC) $(lIBOPENGL) $(LIBARMADILO)

clean:
	rm -rf *.o
	rm -rf $(EXEC)
