
COMPILER=g++-9
CPPFLAGS=-g -pedantic -Wall -pthread -std=c++17
LDFLAGS=-g
LDLIBS=-L/usr/lib/x86_64-linux-gnu -lcurl

test: test.o google_analytics.o
	$(COMPILER) $(LDFLAGS) -o test test.o google_analytics.o $(LDLIBS)

test.o: test.cpp google_analytics.h
	$(COMPILER) $(CPPFLAGS) -c test.cpp

google_analytics.o: google_analytics.h google_analytics.cpp
	$(COMPILER) $(CPPFLAGS) -c google_analytics.cpp

clean:
	rm *.o test