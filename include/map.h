#ifndef MAP_H
#define MAP_H


#include "libs.h"

enum mapState {
	EMPTY = 0,
	WALL = 1,
	CAT = 2,
	BACKGROUND = 3
};

class Map{
private:
	unsigned int catPosition;
	GLuint positions[GRID_COUNT_X * GRID_COUNT_Y];
	unsigned int numOfMoves;
public:
	float xAxisMove, yAxisMove;
	bool moveAnimation;
	unsigned int moveAnimationFrame;

	bool gameOver;
	bool shouldMove;

	double getFieldX(int position);
	double getFieldY(int position);

	void setCatPosition(int position);
	void setWallPosition(int position);

	void clearPosition(int position);
	void reset();

	void randomMove();
	void move();

	void gameOverCheck();

	void setAnimationDirection(int prevPosition, int newPosition);

	GLuint getMapState(int position) { return positions[position];};
	unsigned int getCatPosition() { return catPosition; };
	unsigned int getNumOfMoves() { return numOfMoves; };
};

class Graph {
public:
	std::vector<int> graph[NUM_OF_FIELDS + 1];
	void create();
	void clear();

	void addEdge(int src, int dest);
	void deleteVertex(int src);

	int findShortestPath(int src);
	bool BFS(int src, int pred[], int dist[]);
};


#endif