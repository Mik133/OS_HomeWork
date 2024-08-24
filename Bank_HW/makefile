CXX=g++
CXXLINK=$(CXX) -pthread
CXXFLAGS= -std=c++11 -Wall -Werror -pedantic-errors -DNDEBUG -pthread
OBJS= main.o logger.o ATM.o Account.o bank.o
EXEC=Bank
RM=rm -rf

$(EXEC) : $(OBJS)
	$(CXXLINK) $(OBJS) -o $(EXEC)

main.o : main.cpp logger.o ATM.o Account.o bank.o
	$(CXX) $(CXXFLAGS) -c main.cpp

bank.o : bank.cpp bank.h
	$(CXX) $(CXXFLAGS) -c bank.cpp

logger.o : logger.cpp logger.h
	$(CXX) $(CXXFLAGS) -c logger.cpp

ATM.o : ATM.cpp ATM.h
	$(CXX) $(CXXFLAGS) -c ATM.cpp

Account.o : Account.cpp Account.h
	$(CXX) $(CXXFLAGS) -c Account.cpp

clean:
	$(RM) $(OBJS) $(EXEC)

