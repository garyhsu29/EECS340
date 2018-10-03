#ifndef _node
#define _node

#include <new>
#include <iostream>
#include <deque>
#include <float.h>


class RoutingMessage;
class Table;
class Link;
class SimulationContext;

#include "table.h"
#include<set>
#include<queue>

using namespace std;

class Node {
 private:
  unsigned number;
  SimulationContext   *context;
  double   bw;
  double   lat;

#if defined(LINKSTATE)

class Edge{
    public:
        int src;
        int dst;
        double latency;
        Edge(int src, int dst, double l);
};

class Compare{
    public:
      bool operator()(Edge &l, Edge &r){
          return l.latency > r.latency;
      }
};


    private:
        Table table;
        set<unsigned int> neighbors;
        void Dijkstra();

#endif

#if defined(DISTANCEVECTOR)
    private:
        Table table;
        bool updateTable();
        map<int, pair<int, double> > preprocess_myTable();
        map<int, pair<int, double> > preprocess_recvTable(map<int, pair<int, double> > yourTable);
#endif

  // students will add protocol-specific data here

 public:
  Node(const unsigned n, SimulationContext *c, double b, double l);
  Node();
  Node(const Node &rhs);
  Node & operator=(const Node &rhs);
  virtual ~Node();

  virtual bool Matches(const Node &rhs) const;

  virtual void SetNumber(const unsigned n);
  virtual unsigned GetNumber() const;

  virtual void SetLatency(const double l);
  virtual double GetLatency() const;
  virtual void SetBW(const double b);
  virtual double GetBW() const;

  virtual void SendToNeighbors(const RoutingMessage *m);
  virtual void SendToNeighbor(const Node *n, const RoutingMessage *m);
  virtual deque<Node*> *GetNeighbors();
  virtual void SetTimeOut(const double timefromnow);

  //
  // Students will WRITE THESE
  //
  virtual void LinkHasBeenUpdated(const Link *l);
  virtual void ProcessIncomingRoutingMessage(const RoutingMessage *m);
  virtual void TimeOut();
  virtual Node *GetNextHop(const Node *destination) const;
  virtual Table *GetRoutingTable() const;

  virtual ostream & Print(ostream &os) const;

};

inline ostream & operator<<(ostream &os, const Node &n) { return n.Print(os);}


#endif
