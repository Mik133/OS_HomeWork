# Michael & Yuval Makefile
CXX=g++
CXXLINK=$(CXX)
CXXFLAGS=-std=c++11 -Wall -Werror -pedantic-errors -DNDDEBUG
OBJS=smash.o
EXEC=smash
RM=rm -rf

$(EXEC): $(OBJS)
	$(CXXLINK) $(OBJS) -o $(EXEC)

smash.o:smash.cpp
	$(CXX) $(CXXFLAGS) -c smash.cpp 

clean:
	$(RM) $(OBJS) $(EXEC)










