
CPPFLAGS=-g -pthread -std=c++11
LDFLAGS=-g
LDLIBS=-L/usr/lib/x86_64-linux-gnu -lcurl

test: test.o google_analytics.o
	clang-6.0 $(LDFLAGS) -o test test.o google_analytics.o $(LDLIBS)

test.o: test.cpp google_analytics.h
	clang-6.0 $(CPPFLAGS) -c test.cpp

google_analytics.o: google_analytics.h google_analytics.cpp
	clang-6.0 $(CPPFLAGS) -c google_analytics.cpp

clean:
	rm *.o test