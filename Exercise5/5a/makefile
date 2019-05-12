### CPP compiler wrapper
CXX=mpicxx

### Optimization flags
FFLAGS=-O3

#######################################################################

all: broadcast.exec

#######################################################################

## BS
broadcast.exec: broadcast.cpp
	$(CXX) $(FFLAGS) broadcast.cpp -o broadcast.exec

#######################################################################

clean :
	rm -f *.exec

#######################################################################
