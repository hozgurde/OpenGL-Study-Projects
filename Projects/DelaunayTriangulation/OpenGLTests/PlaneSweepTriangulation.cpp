#include "PlaneSweepTriangulation.h"
#include <cmath>
#include <iostream>

PlaneSweepTriangulation::PlaneSweepTriangulation(Points* points)
{
	this->points = points;
	graph = new Graph(points->GetPointsSize());
	boundary = new Boundary(points, graph);
	triples = new Triples(boundary);

	//Example Structure
	graph->Add_Edge(0, 1);
	graph->Add_Edge(0, 2);
	graph->Add_Edge(1, 2);
	graph->Add_Edge(1, 4);
	graph->Add_Edge(2, 4);
	graph->Add_Edge(0, 3);
	graph->Add_Edge(2, 3);

	boundary->Initialize(3);
	boundary->InsertNewOnBoundary(2, 0);
	boundary->InsertNewOnBoundary(4, 2);
	boundary->PrintBoundary();
	std::cout << "size: " << boundary->GetBoundarySize() << std::endl;

	triples->InsertInTriples(2);

	for (int i = 0; i < graph->GetNoOfEdges(); i++) {
		std::cout << graph->GetEdges()[2 * i] << " " << graph->GetEdges()[2 * i + 1] << std::endl;
	}

	//Generate and bind Vertex Array
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	//Generate Index Buffer
	glGenBuffers(1, &IBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(graph->GetEdges()[0]) * 3 * graph->GetMaxNoOfEdges(), graph->GetEdges(), GL_STATIC_DRAW);
	glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, sizeof(graph->GetEdges()[0]) * 2 * graph->GetMaxNoOfEdges(), sizeof(graph->GetEdges()[0]) * graph->GetMaxNoOfEdges(), boundary->GetBoundary());

	//Generate and bind Vetex Buffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	//Put vertices into buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(points->GetPoints()[0]) * points->GetPointsSize() * 3, points->GetPoints(), GL_STATIC_DRAW);

	//
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)(sizeof(graph->GetEdges()[0]) * 2 * graph->GetMaxNoOfEdges()));
	glEnableVertexAttribArray(0);

	//
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

}

void PlaneSweepTriangulation::Render(GLuint uniformMyColor)
{
	glUniform1i(uniformMyColor, 0);
	RenderPoints();
	RenderGraph();
	glUniform1i(uniformMyColor, 1);
	RenderBoundary();
}

void PlaneSweepTriangulation::RenderPoints()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glDrawArrays(GL_POINTS, 0, points->GetPointsSize());

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PlaneSweepTriangulation::RenderGraph()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_LINES, graph->GetNoOfEdges() * 2, GL_UNSIGNED_INT, 0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void PlaneSweepTriangulation::RenderBoundary()
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glDrawElements(GL_POINTS, boundary->GetBoundarySize(), GL_UNSIGNED_INT, (void*)(sizeof(graph->GetEdges()[0]) * 2 * graph->GetMaxNoOfEdges()));

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Graph::Graph()
{
	Graph(2);
}

Graph::Graph(int noOfPoints)
{
	maxNoOfEdges = 3 * noOfPoints - 6;
	noOfEdges = 0;
	edges = new unsigned int[2 * noOfEdges];
}

Graph::~Graph()
{
	delete[] edges;
}

void Graph::Add_Edge(unsigned int p, unsigned int q)
{
	edges[2 * noOfEdges] = p;
	edges[2 * noOfEdges + 1] = q;
	noOfEdges++;
}

Boundary::Boundary()
{

}

Boundary::Boundary(Points* points, Graph* graph)
{
	boundary = new int[3 * points->GetPointsSize() - 6];
	boundarySize = 0;
	this->points = points;
	this->graph = graph;
}

int Boundary::Before(int q)
{
	int start = 0;
	int end = boundarySize - 1;
	while (start != end) {
		int mid = (start + end) / 2;
		if (boundary[mid] == q) {
			if (mid != 0) {
				return boundary[mid - 1];
			}
			else {
				return -1;
			}
		}
		else if (points->GetPoint(boundary[mid])[0] < points->GetPoint(q)[0]) {
			start = mid;
		}
		else {
			end = mid;
		}
	}
	return boundary[((start + end) / 2) - 1];
}

int Boundary::After(int q)
{
	int start = 0;
	int end = boundarySize - 1;
	while (start != end) {
		int mid = (start + end) / 2;
		if (boundary[mid] == q) {
			if (mid != boundarySize - 1) {
				return boundary[mid + 1];
			}
			else {
				return -1;
			}
		}
		else if (points->GetPoint(boundary[mid])[0] < points->GetPoint(q)[0]) {
			start = mid;
		}
		else {
			end = mid;
		}
	}
	return boundary[((start + end) / 2) + 1];
}

int Boundary::GetCorrespondingPoint(int q)
{
	return boundary[q];
}

void Boundary::ClosestPointTo(int p, int& q1, int& q2)
{
	q1 = 0;
	q2 = boundarySize - 1;
	float x = points->GetPoint(p)[0];

	while (q1 + 1 != q2) {
		int mid = (q1 + q2) / 2;
		if (points->GetPoint(boundary[mid])[0] < x) {
			q1 = mid;
		}
		else {
			q2 = mid;
		}
	}
	return;
}

void Boundary::InsertNewOnBoundary(int p, int q)
{

	boundarySize = boundarySize + 2;
	for (int i = boundarySize - 1; i > q + 1; i--) {
		boundary[i] = boundary[i - 2];
	}
	boundary[q + 1] = p;
	graph->Add_Edge(p, q);
}

void Boundary::UpdateOnBoundary(int q)
{
	graph->Add_Edge(boundary[q - 1], boundary[q + 1]);
	boundarySize = boundarySize - 1;
	for (int i = q; i < boundarySize; i++) {
		boundary[i] = boundary[i + 1];
	}
}

void Boundary::Initialize(int q)
{
	boundarySize++;
	boundary[0] = q;
}

void Boundary::PrintBoundary()
{
	for (int i = 0; i < boundarySize; i++) {
		std::cout << boundary[i] << std::endl;
	}
}

Triples::Triples()
{
}

Triples::~Triples()
{
	delete[] triples;
	delete[] tops;
}

Triples::Triples(Boundary* boundary)
{
	this->boundary = boundary;
	triples = new int[3 * boundary->GetPoints()->GetPointsSize() - 6];
	tops = new float[3 * boundary->GetPoints()->GetPointsSize() - 6];
	sizeOfTriples = 0;
}

float Triples::MinTop()
{
	float min = FLT_MAX;
	for (int i = 0; i < sizeOfTriples; i++) {
		if (tops[i] < min) {
			min = tops[i];
		}
	}

	return min;
}

int Triples::GetPointCorrToMinTop()
{
	float min = FLT_MAX;
	int index = -1;
	for (int i = 0; i < sizeOfTriples; i++) {
		if (tops[i] < min) {
			index = i;
			min = tops[i];
		}
	}

	return index;
}

void Triples::DeleteFromTriples(int q)
{
	for (int i = 0; i < sizeOfTriples; i++) {
		if (triples[i] == q) {
			for (int j = i; i < sizeOfTriples - 1; j++) {
				triples[j] = triples[j + 1];
				tops[j] = tops[j + 1];
			}
			sizeOfTriples--;
		}
	}
}

void Triples::InsertInTriples(int q)
{
	triples[sizeOfTriples] = q;

	int p1 = boundary->GetCorrespondingPoint(q - 1);
	int p2 = boundary->GetCorrespondingPoint(q);
	int p3 = boundary->GetCorrespondingPoint(q + 1);
	float* point1 = boundary->GetPoints()->GetPoint(p1);
	float* point2 = boundary->GetPoints()->GetPoint(p2);
	float* point3 = boundary->GetPoints()->GetPoint(p3);

	float A1x = point1[0] - point2[0];
	float A1y = point1[1] - point2[1];
	float A2x = point1[0] - point3[0];
	float A2y = point1[1] - point3[1];

	float B1 = A1x * A1x + A1y * A1y;
	float B2 = A2x * A2x + A2y * A2y;

	float yc = (B2 - A2y * B1 / A1x) / (A2y - A2x * A1y / A1x);
	float xc = B1 / A1x - (A1y * yc / A1x);

	float r = sqrt((point1[0] - xc) * (point1[0] - xc) + (point1[1] - yc) * (point1[1] - yc));

	float top = yc + r;

	tops[q] = top;

}
