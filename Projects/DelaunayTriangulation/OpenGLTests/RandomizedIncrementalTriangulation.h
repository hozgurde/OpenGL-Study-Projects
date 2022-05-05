
#pragma once
#include "Points.h"

class RandomizedIncrementalTriangulation
{
public:
	RandomizedIncrementalTriangulation(Points points);
	void AdvanceTriangulation();
	void CompleteTriangulation();
	void Render();
private:
	Points points;
};