### CPP compiler wrapper
CXX=mpicxx

### Optimization flags
FFLAGS=-O3

#######################################################################

all: allreduce.exec

#######################################################################

## BS
allreduce.exec: allreduce.cpp
	$(CXX) $(FFLAGS) allreduce.cpp -o allreduce.exec

#######################################################################

clean :
	rm -f *.exec

#######################################################################
