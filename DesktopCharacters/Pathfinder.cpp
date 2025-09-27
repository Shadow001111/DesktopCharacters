#include "Pathfinder.h"
#include "Character.h"

#include <iostream>

void Pathfinder::buildAllPaths()
{
	const auto& obstacles = Character::obstacles;
	size_t obstaclesCount = obstacles.size();

	//
	nodes.clear();
	for (size_t i = 0; i < obstaclesCount; i++)
	{
		const Obstacle* obstacle = &obstacles[i];
		const Range* segment = &obstacle->segments[0];
		
		nodes.emplace_back(obstacle, segment);
	}

	//
	for (size_t i = 0; i < obstaclesCount; i++)
	{
		const auto& obstacleA = obstacles[i];
		if (obstacleA.type != Obstacle::Type::Horizontal || obstacleA.segments.size() != 1)
		{
			continue;
		}
		Node* nodeA = &nodes[i];

		for (size_t j = 0; j < obstaclesCount; j++)
		{
			if (i == j)
			{
				continue;
			}

			const auto& obstacleB = obstacles[j];
			if (obstacleB.type != Obstacle::Type::Horizontal || obstacleB.segments.size() != 1)
			{
				continue;
			}
			Node* nodeB = &nodes[j];

			JumpPlan jumpPlan = computeJump(obstacleA, obstacleB);

			nodeA->nextNodes.emplace_back(nodeB, jumpPlan);
		}
	}
}

const std::vector<Pathfinder::Node>& Pathfinder::getNodes() const
{
	return nodes;
}

JumpPlan Pathfinder::computeJump(const Obstacle& obstA, const Obstacle& obstB) const
{
	Range rangeA = obstA.segments[0];
	Range rangeB = obstB.segments[0];

	float landingX;
	if (rangeB.min > rangeA.max)
	{
		landingX = rangeB.min;
	}
	else if (rangeB.max < rangeA.min)
	{
		landingX = rangeB.max;
	}
	else
	{
		// overlap in horizontal range
		landingX = fmaxf(rangeA.min, rangeB.min);
	}

	float takeoffX;
	if (landingX < rangeA.min)
	{
		takeoffX = rangeA.min;
	}
	else if (landingX > rangeA.max)
	{
		takeoffX = rangeA.max;
	}
	else
	{
		takeoffX = landingX; // vertical jump if ranges overlap
	}

	float dx = landingX - takeoffX;
	float dy = obstB.perpOffset - obstA.perpOffset;

	JumpPlan jumpPlan;
	jumpPlan.delta = { dx, dy };
	jumpPlan.takeoff = { takeoffX, obstA.perpOffset };
	jumpPlan.landing = { landingX, obstB.perpOffset };
	return jumpPlan;
}

Pathfinder::Node::Node(const Obstacle* obstacle, const Range* segment) :
	obstacle(obstacle), segment(segment)
{
}

Pathfinder::Node::NextNode::NextNode(const Node* node, const JumpPlan& jumpPlan) :
	node(node), jumpPlan(jumpPlan)
{
}
