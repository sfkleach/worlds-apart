
CXXFLAGS=-std=c++11

OBJS=sqlite_test.o world.o team.o unit.o hex.o

sqlite_test: $(OBJS)
	$(CXX) $(CFLAGS) -O -o $@ $(OBJS) -lsqlite3

.PHONY: clean
clean:
	rm -f sqlite_test
	rm -f *.o