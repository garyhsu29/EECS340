#include "messages.h"


#if defined(GENERIC)
ostream &RoutingMessage::Print(ostream &os) const
{
  os << "RoutingMessage()";
  return os;
}
#endif


#if defined(LINKSTATE)


ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage(map<int, map<int, double> > g, map<int, map<int, int> > sn, unsigned int s){
    graph = g;
    seqNum = sn;
    src = s;
}

RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{
    graph = rhs.graph;
    seqNum = rhs.seqNum;
    src = rhs.src;
}
#endif


#if defined(DISTANCEVECTOR)

ostream &RoutingMessage::Print(ostream &os) const
{
  return os;
}

RoutingMessage::RoutingMessage(unsigned int _src, unsigned int _dst, map<int, pair<int, double> > _myTable){
    src = _src;
    dst = _dst;
    myTable = _myTable;
}


RoutingMessage::RoutingMessage(const RoutingMessage &rhs)
{
	  src = rhs.src;
    dst = rhs.dst;
    myTable = rhs.myTable;
}


#endif

