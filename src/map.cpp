#include "../include/map.h"

Graph graph;

void Map::reset() {
	graph.clear();
	graph.create();
	for (int i = 0; i < GRID_COUNT_X * GRID_COUNT_X; i++) {
		clearPosition(i);
	}
	catPosition = 60; // center of the map = 60 = 11*5 + 5
	setCatPosition(catPosition);
	// create walls on random positions
	for (size_t i = 0; i < 13; i++){
		int random = rand() % 121;
		setWallPosition(random);
	}
	numOfMoves = 0;
	shouldMove = false;
	moveAnimation = false;
	moveAnimationFrame = 0;
}

double Map::getFieldX(int position) {
	double base_x;
	if ((position / 11) % 2 == 0) base_x = 77;
	else base_x = 108;
	base_x += (61.8 * (position % 11));
	return base_x;
}

double Map::getFieldY(int position) {
	double base_y = 163;
	base_y += 45.8 * (position / 11);
	return base_y;
}

void Map::setCatPosition(int position) {
	positions[position] = CAT;
	catPosition = position;
}

void Map::setWallPosition(int position) {
	if (positions[position] != EMPTY) return;
	positions[position] = WALL;
	shouldMove = true;
	graph.deleteVertex(position);
}

void Map::clearPosition(int position) {
	positions[position] = EMPTY;
}

void Map::move() {
	int newCatPosition = graph.findShortestPath(catPosition);
	if (newCatPosition == -1) {
		randomMove();
		return;
	}
	if (newCatPosition == 121) return;

	setAnimationDirection(catPosition, newCatPosition);

	clearPosition(catPosition);
	setCatPosition(newCatPosition);
	numOfMoves++;
}

void Map::randomMove() {
	if (graph.graph[catPosition].size() == 0) 
		return;
	int random = rand() % graph.graph[catPosition].size();
	clearPosition(catPosition);
	int newCatPosition = graph.graph[catPosition][random];

	setAnimationDirection(catPosition, newCatPosition);

	setCatPosition(newCatPosition);
	numOfMoves++;
}

void Map::gameOverCheck() {
	// if the cat is on the border of the map
	if ((catPosition + 1) % GRID_COUNT_X == 0
		|| catPosition % GRID_COUNT_X == 0
		|| catPosition < GRID_COUNT_X
		|| catPosition > 109) {
		// xAxis and yAxis for animation will be the same as in the previous move
		Sleep(200);
		moveAnimation = true;
		gameOver = true;
	}
	// if the cat vertex is not connected to any other vertex
	if (graph.graph[catPosition].size() == 0) {
		gameOver = true;
	}
}

void Map::setAnimationDirection(int prevPosition, int newPosition) {
	if (getFieldX(newPosition) > getFieldX(prevPosition)) xAxisMove = 1;
	else if (getFieldX(newPosition) == getFieldX(prevPosition)) xAxisMove = 0;
	else xAxisMove = -1;

	if (getFieldY(newPosition) < getFieldY(prevPosition)) yAxisMove = 1;
	else if (getFieldY(newPosition) == getFieldY(prevPosition)) yAxisMove = 0;
	else yAxisMove = -1;

	if (yAxisMove != 0) xAxisMove /= 2;
	moveAnimation = true;
}
// Graph functions

void Graph::create(){
	// create a graph that will be used to find the shortest path using the BFS algorithm
	for (int i = 0; i < NUM_OF_FIELDS; ++i) {
		if ((i + 1) % 11 != 0)
			addEdge(i, (i + 1));
		if ((i + 1) % 22 != 0 && i < 110) {
			if ((i / 11) % 2 == 0)
				addEdge(i, (i + 11));
			else
				addEdge(i, (i + 12));
		}
		if (i % 22 != 0 && i < 110) {
			if ((i / 11) % 2 == 0)
				addEdge(i, (i + 10));
			else
				addEdge(i, (i + 11));
		}
		// add a winning vertex connected to the borders
		if (i < 11 || i > 109 || i % 11 == 0 || (i + 1) % 11 == 0)
			addEdge(i, 121);
	}
}

void Graph::clear(){
	for (int i = 0; i < NUM_OF_FIELDS; ++i) {
		graph[i].clear();
	}
}

void Graph::addEdge(int src, int dest){
	graph[src].push_back(dest);
	graph[dest].push_back(src);
}

void Graph::deleteVertex(int src){
	for (int i = 0; i < graph[src].size(); ++i) {
		std::vector<int>* adjVertex = &graph[graph[src][i]];

		// remove the connection at each adjacent vertex
		for (int j = 0; j < adjVertex->size(); ++j) {
			if ((*adjVertex)[j] == src) {
				adjVertex->erase(adjVertex->begin() + j);
				break;
			}
		}
	}
}

int Graph::findShortestPath(int src) {
	const int v = NUM_OF_FIELDS + 1;
	int pred[v], dist[v];

	if (BFS(src, pred, dist) == false) {
		return -1;
	}

	std::vector<int> path;
	int crawl = 121;
	path.push_back(crawl);
	while (pred[crawl] != -1) {
		path.push_back(pred[crawl]);
		crawl = pred[crawl];
	}

	return path[path.size() - 2];
}

bool Graph::BFS(int src, int pred[], int dist[]){
	std::list<int> queue;
	const int v = NUM_OF_FIELDS + 1;
	bool visited[v];

	for (int i = 0; i < v; i++) {
		visited[i] = false;
		dist[i] = INT_MAX;
		pred[i] = -1;
	}

	visited[src] = true;
	dist[src] = 0;
	queue.push_back(src);

	// standard BFS algorithm
	while (!queue.empty()) {
		int u = queue.front();
		queue.pop_front();
		for (int i = 0; i < graph[u].size(); i++) {
			if (visited[graph[u][i]] == false) {
				visited[graph[u][i]] = true;
				dist[graph[u][i]] = dist[u] + 1;
				pred[graph[u][i]] = u;
				queue.push_back(graph[u][i]);

				if (graph[u][i] == 121)
					return true;
			}
		}
	}

	return false;
}
