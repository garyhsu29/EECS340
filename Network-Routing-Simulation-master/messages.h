#ifndef _messages
#define _messages

#include <iostream>

#include "node.h"
#include "link.h"

#if defined(GENERIC)
class RoutingMessage {
 public:
  ostream & Print(ostream &os) const;
};
#endif

#if defined(LINKSTATE)

class RoutingMessage {

    public:

    	map<int, map<int, double> > graph;
    	map<int, map<int, int> > seqNum;
    	unsigned int src;
    	RoutingMessage(map<int, map<int, double> > graph, map<int, map<int, int> > seqNum, unsigned int src);
		RoutingMessage(const RoutingMessage &rhs);
		RoutingMessage &operator=(const RoutingMessage &rhs);

		ostream & Print(ostream &os) const;
};
#endif

#if defined(DISTANCEVECTOR)
class RoutingMessage {

    public:
		RoutingMessage(unsigned int src, unsigned int dst, map<int, pair<int, double> > myTable);
		RoutingMessage(const RoutingMessage &rhs);
		RoutingMessage &operator=(const RoutingMessage &rhs);
		unsigned int src;
		unsigned int dst;
		map<int, pair<int, double> > myTable;
		ostream & Print(ostream &os) const;
};
#endif


inline ostream & operator<<(ostream &os, const RoutingMessage &m) { return m.Print(os);}

#endif
