#pragma once
#include "Obstacle.h"

#include "Core/Vec2.h"

struct JumpPlan
{
	Range takeoffRange;
	float takeoffY;

	Range landingRange;
	float landingY;
};

class Pathfinder
{
	struct Node
	{
		struct NextNode
		{
			const Node* node;
			JumpPlan jumpPlan;

			NextNode(const Node* node, const JumpPlan& jumpPlan);
		};

		const Obstacle* obstacle;
		size_t segmentIndex;  // Index into obstacle->segments

		std::vector<NextNode> nextNodes;

		Node(const Obstacle* obstacle, size_t segmentIndex);
		const Range& getSegment() const;
	};
public:
	void buildAllPaths();

	const std::vector<Node>& getNodes() const;
private:
	std::vector<Node> nodes;

	JumpPlan computeJump(const Obstacle& obstA, const Range& segmentA, const Obstacle& obstB, const Range& segmentB) const;
};

