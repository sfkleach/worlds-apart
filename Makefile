
CXXFLAGS=-std=c++11 -O

OBJS=main.o world.o team.o unit.o hex.o faction.o goal.o mishap.o link.o common.o findroute.o

simulation: $(OBJS)
	$(CXX) $(CFLAGS) -o $@ $(OBJS) -lsqlite3

.PHONY: clean
clean:
	rm -f simulation
	rm -f *.o