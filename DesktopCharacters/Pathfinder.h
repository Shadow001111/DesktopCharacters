#pragma once
#include "Obstacle.h"

#include "Core/Vec2.h"

struct JumpPlan
{
	Vec2 delta, takeoff, landing;
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
		const Range* segment;

		std::vector<NextNode> nextNodes;

		Node(const Obstacle* obstacle, const Range* segment);
	};
public:
	void buildAllPaths();

	const std::vector<Node>& getNodes() const;
private:
	std::vector<Node> nodes;

	JumpPlan computeJump(const Obstacle& obstA, const Obstacle& obstB) const;
};

