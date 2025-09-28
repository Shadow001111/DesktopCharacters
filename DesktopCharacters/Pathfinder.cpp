#include "Pathfinder.h"
#include "Character.h"

#include <iostream>

void Pathfinder::buildAllPaths()
{
	const auto& obstacles = Character::obstacles;

	// Build nodes for each segment of each obstacle
	nodes.clear();
	for (const Obstacle& obstacle : obstacles)
	{
		// Create a node for each segment
		for (size_t segIdx = 0; segIdx < obstacle.segments.size(); segIdx++)
		{
			nodes.emplace_back(&obstacle, segIdx);
		}
	}

	// Build connections between horizontal platform segments
	for (auto& nodeA : nodes)
	{
		// Only consider horizontal platforms for jumping
		if (nodeA.obstacle->type != Obstacle::Type::Horizontal)
		{
			continue;
		}

		for (auto& nodeB : nodes)
		{
			if (&nodeA == &nodeB) // Don't connect to self
			{
				continue;
			}

			// Only connect to other horizontal platforms
			if (nodeB.obstacle->type != Obstacle::Type::Horizontal)
			{
				continue;
			}

			JumpPlan jumpPlan = computeJump(*nodeA.obstacle, nodeA.getSegment(),
				*nodeB.obstacle, nodeB.getSegment());

			nodeA.nextNodes.emplace_back(&nodeB, jumpPlan);
		}
	}
}

const std::vector<Pathfinder::Node>& Pathfinder::getNodes() const
{
	return nodes;
}

JumpPlan Pathfinder::computeJump(const Obstacle& obstA, const Range& segmentA, const Obstacle& obstB, const Range& segmentB) const
{
	Range landingRange;
	if (segmentB.min > segmentA.max)
	{
		landingRange = { segmentB.min, segmentB.min };
	}
	else if (segmentB.max < segmentA.min)
	{
		landingRange = { segmentB.max, segmentB.max };
	}
	else
	{
		landingRange = { fmaxf(segmentA.min, segmentB.min), fminf(segmentA.max, segmentB.max) };
	}

	Range takeoffRange;
	if (landingRange.max < segmentA.min)
	{
		takeoffRange = { segmentA.min, segmentA.min };
	}
	else if (landingRange.min > segmentA.max)
	{
		takeoffRange = { segmentA.max, segmentA.max };
	}
	else
	{
		takeoffRange = landingRange; // vertical jump if ranges overlap
	}

	JumpPlan jumpPlan;

	jumpPlan.takeoffRange = takeoffRange;
	jumpPlan.takeoffY = obstA.perpOffset;

	jumpPlan.landingRange = landingRange;
	jumpPlan.landingY = obstB.perpOffset;

	return jumpPlan;
}

Pathfinder::Node::Node(const Obstacle* obstacle, size_t segmentIndex) :
	obstacle(obstacle), segmentIndex(segmentIndex)
{
}

const Range& Pathfinder::Node::getSegment() const
{
	return obstacle->segments[segmentIndex];
}

Pathfinder::Node::NextNode::NextNode(const Node* node, const JumpPlan& jumpPlan) :
	node(node), jumpPlan(jumpPlan)
{
}
