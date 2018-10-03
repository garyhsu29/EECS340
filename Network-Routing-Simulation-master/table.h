#ifndef _table
#define _table


#include <iostream>
#include <float.h>

using namespace std;

#if defined(GENERIC)
class Table {
  // Students should write this class

 public:
  ostream & Print(ostream &os) const;
};
#endif


#if defined(LINKSTATE)
#include<map>

class Table {
	// Students should write this class
	public:

		Table(){};
		Table(const Table* t);
		map<int, int> prev;
		map<int, map<int, double> > graph;
		map<int, map<int, int> > seqNum;
		ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)

#include<map>
#include<vector>

class Table {
	
	public:
		Table(){};
		Table(const Table &t);
		ostream & Print(ostream &os) const;
		
		// dst, vector<next, latency>
		map<int, vector<pair<int, double> > > myTable;
		// src, dst, pair<next, latancy> 
		map<int, map<int, pair<int, double> > > nbTable;

		int getBestRouteIdx(unsigned int dst);
};
#endif

inline ostream & operator<<(ostream &os, const Table &t) { return t.Print(os);}

#endif
