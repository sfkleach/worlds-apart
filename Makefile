
CXXFLAGS=-std=c++11 -g

OBJS=main.o world.o team.o unit.o hex.o faction.o goal.o mishap.o

simulation: $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) -lsqlite3

.PHONY: clean
clean:
	rm -f simulation
	rm -f *.o