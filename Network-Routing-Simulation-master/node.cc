#include "node.h"
#include "context.h"
#include "error.h"


Node::Node(const unsigned n, SimulationContext *c, double b, double l) : 
    number(n), context(c), bw(b), lat(l) 
{}

Node::Node() 
{ throw GeneralException(); }

Node::Node(const Node &rhs) : 
  number(rhs.number), context(rhs.context), bw(rhs.bw), lat(rhs.lat) {}

Node & Node::operator=(const Node &rhs) 
{
  return *(new(this)Node(rhs));
}

void Node::SetNumber(const unsigned n) 
{ number=n;}

unsigned Node::GetNumber() const 
{ return number;}

void Node::SetLatency(const double l)
{ lat=l;}

double Node::GetLatency() const 
{ return lat;}

void Node::SetBW(const double b)
{ bw=b;}

double Node::GetBW() const 
{ return bw;}

Node::~Node()
{}

// Implement these functions  to post an event to the event queue in the event simulator
// so that the corresponding node can recieve the ROUTING_MESSAGE_ARRIVAL event at the proper time
void Node::SendToNeighbors(const RoutingMessage *m)
{
	context->SendToNeighbors(this, m);
}

void Node::SendToNeighbor(const Node *n, const RoutingMessage *m)
{
    context->SendToNeighbor(this, n, m);
}

deque<Node*> *Node::GetNeighbors()
{
  return context->GetNeighbors(this);
}

void Node::SetTimeOut(const double timefromnow)
{
  context->TimeOut(this,timefromnow);
}


bool Node::Matches(const Node &rhs) const
{
  return number==rhs.number;
}


#if defined(GENERIC)
void Node::LinkHasBeenUpdated(const Link *l)
{
  cerr << *this << " got a link update: "<<*l<<endl;
  //Do Something generic:
  SendToNeighbors(new RoutingMessage);

}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
  cerr << *this << " got a routing messagee: "<<*m<<" Ignored "<<endl;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
  return 0;
}

Table *Node::GetRoutingTable() const
{
  return new Table;
}


ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}

#endif

#if defined(LINKSTATE)


void Node::LinkHasBeenUpdated(const Link *l)
{
    cerr << *this<<": Link Update: "<<*l<<endl;
    unsigned int dst = l->GetDest();
    if(neighbors.find(dst) == neighbors.end())
        neighbors.insert(dst);
    table.graph[this->GetNumber()][dst] = l->GetLatency();
    table.seqNum[this->GetNumber()][dst] += 1;
    RoutingMessage* m = new RoutingMessage(table.graph, table.seqNum, this->GetNumber());
    SendToNeighbors(m);
    Dijkstra();
}


void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{
    cerr << *this << " Routing Message: "<<*m<<endl;
    bool is_update = false;
    for(map<int, map<int, int> >::const_iterator it1 = m->seqNum.begin(); it1 != m->seqNum.end(); it1++){
        for(map<int, int>::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++){
            int src = it1->first;
            int dst = it2->first;
            int sn = it2->second;
            if(sn > table.seqNum[src][dst]){
                table.seqNum[src][dst] = sn;
                map<int, map<int, double> >::const_iterator it1_graph = m->graph.find(src);
                map<int, double>::const_iterator it2_graph = it1_graph->second.find(dst);
                table.graph[src][dst] = it2_graph->second;
                is_update = true;
            }
        }
    }
    if(is_update){
        RoutingMessage* m = new RoutingMessage(table.graph, table.seqNum, this->GetNumber());
        for(set<unsigned int>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
            if(m->src != *it){
                Node* n = new Node(*it, context, 0, 0);
                SendToNeighbor(n, m);
            }
        }
    }
    Dijkstra();
}

Node::Edge::Edge(int s, int d, double l){
    src = s;
    dst = d;
    latency = l;
}

void Node::Dijkstra(){
    table.prev.clear();
    int curNode = this->GetNumber();
    priority_queue<Edge, vector<Edge>, Compare> pq;
    set<int> visited;
    visited.insert(curNode);
    for(set<unsigned int>::iterator it = neighbors.begin(); it != neighbors.end(); it++){
        Edge e(curNode, *it, table.graph[curNode][*it]);
        pq.push(e);
    }
    while(!pq.empty()){
        Edge e = pq.top();
        pq.pop();
        curNode = e.dst;
        if(visited.find(curNode) != visited.end())
            continue;
        visited.insert(curNode);
        for(map<int, double>::iterator it = table.graph[curNode].begin(); it != table.graph[curNode].end(); it++){
            int neighbor = it->first;
            Edge e_temp(curNode, neighbor, e.latency + table.graph[curNode][neighbor]);
            pq.push(e_temp);
        }
        table.prev[curNode] = e.src;
    }

}

void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}

Node *Node::GetNextHop(const Node *destination) const
{
    //cerr<<this->GetNumber()<<endl;
    //cerr<<table<<endl;
    unsigned int curNode = this->GetNumber();
    unsigned int dstHop = destination->GetNumber();
    map<int, int>::const_iterator it = table.prev.find(dstHop);
    if(it == table.prev.end()) return NULL;
    while(it->second != curNode){
        it = table.prev.find(it->second);
    }
    Node *n = new Node(it->first, context, 0, 0);
    return n;
}

Table *Node::GetRoutingTable() const
{
  // WRITE
  return new Table(&(this->table));
}

ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw<<")";
  return os;
}
#endif


#if defined(DISTANCEVECTOR)

void Node::LinkHasBeenUpdated(const Link *l)
{
  // update our table
  // send out routing mesages
  cerr << *this<<": Link Update: "<<*l<<endl;

  // update the route in current myTable
  unsigned int dstNum = l->GetDest();
  bool direct_dst_found = false;
  for(int i = 0; i < table.myTable[dstNum].size(); i++){
      if(table.myTable[dstNum][i].first == dstNum){
          table.myTable[dstNum][i].second = l->GetLatency();
          direct_dst_found = true;
      }
  }
  if(!direct_dst_found)
      table.myTable[dstNum].push_back({dstNum, l->GetLatency()});

  for(map<int, vector<pair<int, double> > >::iterator it = table.myTable.begin(); it != table.myTable.end(); it++){
      // it->second: possible route, possible route dst: it->first
      for(int i = 0; i < it->second.size(); i++){
	      if(it->second[i].first == l->GetDest())
	          it->second[i].second = l->GetLatency() + table.nbTable[dstNum][it->first].second;
      }
  }
  updateTable();
  RoutingMessage *m = new RoutingMessage(this->GetNumber(), -1, preprocess_myTable());
  SendToNeighbors(m);
}


bool Node::updateTable(){

    //table.tableContent[this->GetNumber()][this->GetNumber()] = {this->GetNumber(), 0};
    bool is_update = false;
    // Run through dst in myTable
    for(map<int, vector<pair<int, double> > >::iterator it = table.myTable.begin(); it != table.myTable.end(); it++){
        // Check if I can build a new dst or update existed dst
        // it->first is a middle node
        if(table.nbTable.find(it->first) != table.nbTable.end() && it->first != this->GetNumber()){
            // Run through all possible routes starting from it->first
            for(map<int, pair<int, double> >::iterator it_nb = table.nbTable[it->first].begin(); it_nb != table.nbTable[it->first].end(); it_nb++){
            	//Obtain the best route from current node to it->first
            	int idx_best = table.getBestRouteIdx(it->first);
            	// calculate new latency from current node to a dst
            	double new_latency = table.myTable[it->first][idx_best].second + table.nbTable[it->first][it_nb->first].second;
                if(table.nbTable[it->first][it_nb->first].second == -1) new_latency = -1;
                if(new_latency != -1){
                	bool hop_found = false;
                    unsigned int nextHop = table.myTable[it->first][idx_best].first;
                	for(int j = 0; j < table.myTable[it_nb->first].size(); j++){
                		if(table.myTable[it_nb->first][j].first == nextHop){
                			hop_found = true;
                	        if(table.myTable[it_nb->first][j].second > new_latency){
                	        	table.myTable[it_nb->first][j].second = new_latency;
                	        	is_update = true;
                	        }
                	        break;
                		}
                	}
                	if(!hop_found){
                		table.myTable[it_nb->first].push_back({table.myTable[it->first][idx_best].first, new_latency});
                		is_update = true;
                	}
                	
                }
            }
        }
    }
    return is_update;
}

map<int, pair<int, double> > Node::preprocess_myTable(){
    map<int, pair<int, double> > res;
    //cerr<<this->GetNumber()<<endl;
    //cerr<<table<<endl;
    for(map<int, vector<pair<int, double> > >::iterator it = table.myTable.begin(); it != table.myTable.end(); it++){
        if(it->first == this->GetNumber()) continue;
        int idx_best = table.getBestRouteIdx(it->first);
        res[it->first] = {it->second[idx_best].first, it->second[idx_best].second};
        //cerr<<it->first<<"  "<<idx_best<<endl;
    }
    return res;
}

void Node::ProcessIncomingRoutingMessage(const RoutingMessage *m)
{

    table.nbTable[m->src] = preprocess_recvTable(m->myTable);
    for(map<int, vector<pair<int, double> > >::iterator it = table.myTable.begin(); it != table.myTable.end(); it++){
        
        for(map<int, pair<int, double> >::iterator it_nb = table.nbTable[m->src].begin(); it_nb != table.nbTable[m->src].end(); it_nb++){
	        if(it->first != it_nb->first) continue;
	        // it->second: possible route, possible route dst: it->first
			for(int i = 0; i < it->second.size(); i++){
			    if(it->second[i].first == m->src){
			    	int idx_best = table.getBestRouteIdx(m->src);
			        it->second[i].second = table.myTable[m->src][idx_best].second + it_nb->second.second;
			        if(it_nb->second.second == -1)
			        	it->second[i].second = -1;
			    }
			}
		}
    }
    bool is_update = updateTable();
    if(!is_update) return;
    RoutingMessage *m_others = new RoutingMessage(this->GetNumber(), -1, preprocess_myTable());
    SendToNeighbors(m_others);
}

map<int, pair<int, double> > Node::preprocess_recvTable(map<int, pair<int, double> > yourTable){
    for(map<int, pair<int, double> >::iterator it = yourTable.begin(); it != yourTable.end(); it++){
        if(it->second.first == this->GetNumber()){
            it->second.second = -1;
        }
    }
    return yourTable;
}


void Node::TimeOut()
{
  cerr << *this << " got a timeout: ignored"<<endl;
}



Node *Node::GetNextHop(const Node *destination) const
{ 
    //cerr<<"test "<<this->GetNumber()<<" "<<destination->GetNumber()<<endl;
    //cerr<<this->GetNumber()<<endl;
    //cerr<<table<<endl;
    unsigned int dstNum = destination->GetNumber();
    if(table.myTable.find(dstNum) != table.myTable.end()){
        map<int, vector<pair<int, double> > >::const_iterator it1 = table.myTable.find(dstNum);
        vector<pair<int, double> > possible_route = it1->second;
        unsigned int nextHop = possible_route[0].first;
        double min_latency = DBL_MAX;
        for(int i = 0; i < possible_route.size(); i++){
			if(min_latency > possible_route[i].second && possible_route[i].second != -1){
				nextHop = possible_route[i].first;
				min_latency = possible_route[i].second;
			}
        }
        if(min_latency == DBL_MAX) return NULL;
        Node *n = new Node(nextHop, context, 0, 0);
        //cerr<<"my answer: "<<n->GetNumber()<<endl;
        return n;
    }
    else{
        return NULL;
    }
}

Table *Node::GetRoutingTable() const
{
    return new Table(this->table);
}



ostream & Node::Print(ostream &os) const
{
  os << "Node(number="<<number<<", lat="<<lat<<", bw="<<bw;
  return os;
}
#endif
