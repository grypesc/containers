#include <list>
#include <vector>
#include <stack>

struct Edge
{
	int v1, v2;
	Edge() {
		v1=0;
		v2=0;
	}
	Edge(int v1Arg, int v2Arg){
		v1=v1Arg;
		v2=v2Arg;
	}
};


class Graph
{
	std::vector<std::list <int> > graph;
	size_t vertexCounter;

public:
	Graph(size_t n){
	vertexCounter=n;
	for (int i=0;i<n;i++){
		std::list<int> emptyList;
		graph.push_back(emptyList);
		}
	}

	size_t size(){
		return vertexCounter;
	}

	void add(Edge e)
	{
		while (graph.size()<=e.v1){
			std::list<int> emptyList;
			graph.push_back(emptyList);
		}
		while (graph.size()<=e.v2){
			std::list<int> emptyList;
			graph.push_back(emptyList);
		}
		graph[e.v1].push_back(e.v2);
		graph[e.v2].push_back(e.v1);
	}

	int isCoherent(int startingVertex)
	{
		if (startingVertex>=(int)graph.size())
			throw std::out_of_range ("Graph doesn't have that vertex");
		std::stack <int> stck;
		int visited[vertexCounter];
		for (size_t i=0;i<vertexCounter;i++)
			visited[i]=0;
		stck.push(startingVertex);
		size_t noOfVisitedVertexes=0;
		while(!stck.empty())
		{
			int current=stck.top();
			stck.pop();
			if(visited[current]==1)
				continue;

			visited[current]=1;
			noOfVisitedVertexes++;
			for (std::list<int>::iterator it=graph[current].begin();it!=graph[current].end();it++)
				if(visited[*it]==0) stck.push(*it);
		}
		return noOfVisitedVertexes==vertexCounter;
	}

		int isCoherentWithout2Vertexes(int startingVertex, int v1, int v2)
	{
		if (startingVertex>=(int)graph.size())
			throw std::out_of_range ("Graph doesn't have that vertex");
		std::stack <int> stck;
		int visited[vertexCounter];
		for (size_t i=0;i<vertexCounter;i++)
			visited[i]=0;
		stck.push(startingVertex);
		size_t noOfVisitedVertexes=0;
		while(!stck.empty())
		{
			int current=stck.top();
			stck.pop();
			if(visited[current]==1)
				continue;

			visited[current]=1;
			noOfVisitedVertexes++;
			for (std::list<int>::iterator it=graph[current].begin();it!=graph[current].end();it++)
				if(visited[*it]==0 && *it != v1 && *it != v2) stck.push(*it);
		}
		return noOfVisitedVertexes==vertexCounter-2;
	}

	std::vector<Edge> findBridges()
	{
		std::vector<Edge> bridges;
		Edge newBridge;
		if (graph.size()<=2)
			return bridges;

			for (int currentVertex=1; currentVertex<graph.size();currentVertex++)
				for (std::list<int>::iterator hisNeighbour=graph[currentVertex].begin();hisNeighbour!=graph[currentVertex].end();hisNeighbour++)
					{
						int startDFS;
						for (startDFS=0; startDFS<graph.size();startDFS++)
							{
								if (startDFS!=currentVertex && startDFS!=*hisNeighbour)
									break;
							}
						if (!isCoherentWithout2Vertexes(startDFS,currentVertex,*hisNeighbour) && *hisNeighbour<currentVertex)
							{
								newBridge.v1=currentVertex;
								newBridge.v2=*hisNeighbour;
								bridges.push_back(newBridge);
							}
					}
		return bridges;
	}

	std::list<int> neighbours(int vertex)
	{
		if (vertex>=(int)graph.size())
			throw std::out_of_range ("Graph doesnt have that vertex");
		return graph[vertex];
	}
};
