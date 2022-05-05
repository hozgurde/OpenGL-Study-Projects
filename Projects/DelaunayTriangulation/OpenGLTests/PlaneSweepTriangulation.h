
#pragma once
#include "Points.h"
#include "float.h"

class Graph {
public:
	Graph();
	Graph(int noOfPoints);
	~Graph();
	void Add_Edge(unsigned int p, unsigned int q);
	unsigned int* GetEdges() { return edges; };
	int GetMaxNoOfEdges() { return maxNoOfEdges; };
	int GetNoOfEdges() { return noOfEdges; };
private:
	unsigned int* edges;
	int noOfEdges;
	int maxNoOfEdges;
};

class Boundary {
public:
	Boundary();
	Boundary(Points* points, Graph* graph);
	int Before(int q);
	int After(int q);
	int GetCorrespondingPoint(int q);
	void ClosestPointTo(int p, int& q1, int& q2);
	void InsertNewOnBoundary(int p, int q);
	void UpdateOnBoundary(int q);
	void Initialize(int q);
	Points* GetPoints() { return points; };
	void PrintBoundary();
	int* GetBoundary() { return boundary; };
	int GetBoundarySize() { return boundarySize; };
private:
	int* boundary;
	int boundarySize;
	Points* points;
	Graph* graph;
};

class Triples {
public:
	Triples();
	~Triples();
	Triples(Boundary* boundary);
	float MinTop();
	int GetPointCorrToMinTop();
	void DeleteFromTriples(int q);
	void InsertInTriples(int q);
private:
	Boundary* boundary;
	int* triples;
	float* tops;
	int sizeOfTriples;
};

class PlaneSweepTriangulation
{
public:
	PlaneSweepTriangulation(Points* points);
	void AdvanceTriangulation();
	void CompleteTriangulation();
	void Render(GLuint uniformMyColor);
private:
	void RenderPoints();
	void RenderGraph();
	void RenderBoundary();
	GLuint VAO, VBO, IBO;
	Points* points;
	Graph* graph;
	Boundary* boundary;
	Triples* triples;
};