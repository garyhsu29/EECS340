#include "table.h"

#if defined(GENERIC)
ostream & Table::Print(ostream &os) const
{
  // WRITE THIS
  os << "Table()";
  return os;
}
#endif

#if defined(LINKSTATE)

Table::Table(const Table* t){
    prev = t->prev;
    graph = t->graph;
    seqNum = t->seqNum;
}

ostream & Table::Print(ostream &os) const{
    os<<"My graph:"<<endl;
    for(map<int, map<int, double> >::const_iterator it1 = graph.begin(); it1 != graph.end(); it1++){
        for(map<int, double>::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++){
            int src = it1->first;
            int dst = it2->first;
            os<<src<<"->"<<dst<<":"<<it2->second<<endl;
        }
    }
    os<<"My prev:"<<endl;
    for(map<int, int>::const_iterator it = prev.begin(); it != prev.end(); it++){
        os<<it->first<<"->"<<it->second<<endl;
    }
    return os;
}

#endif

#if defined(DISTANCEVECTOR)

Table::Table(const Table &t){
    myTable = t.myTable;
    nbTable = t.nbTable;
}

int Table::getBestRouteIdx(unsigned int dst){

    vector<pair<int, double> > possible_route = myTable[dst];
    if(possible_route.size() == 0)
    	return -1;
	  unsigned int idx_best = 0;
    double min_latency = DBL_MAX;
    for(int i = 0; i < possible_route.size(); i++){
        if(min_latency > possible_route[i].second && possible_route[i].second != -1){
            idx_best = i;
			      min_latency = possible_route[i].second;
        }
    }
    return idx_best;
}

ostream & Table::Print(ostream &os) const
{
  os<<"myTable:"<<endl;
  for(map<int, vector<pair<int, double> > >::const_iterator it = myTable.begin(); it != myTable.end(); it++){
      vector<pair<int, double> > possible_route = it->second;
  	  for(int i = 0; i < possible_route.size(); i++){
          os<<"("<<it->first<<", "<<possible_route[i].first<<", "<<possible_route[i].second<<")"<<endl;
      }
  }
  os<<"nbTable:"<<endl;
  for(map<int, map<int, pair<int, double> > >::const_iterator it1 = nbTable.begin(); it1 != nbTable.end(); it1++){
      for(map<int, pair<int, double> >::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++){
          os<<"("<<it1->first<<", "<<it2->first<<", "<<it2->second.first<<", "<<it2->second.second<<")"<<endl;
      }
  }
  // WRITE THIS
  return os;
}

#endif
