//(c) 2016 by Authors
//This file is a part of ABruijn program.
//Released under the BSD license (see LICENSE file)

#include <cmath>


#include "repeat_resolver.h"
#include "graph_processing.h"
#include "../common/config.h"
#include "../common/utils.h"
#include "../common/parallel.h"
#include "bipartie_mincost.h"


void RepeatResolver::separatePath(const GraphPath& graphPath, 
								  SequenceSegment readSegment, 
								  FastaRecord::Id newId)
{
	//first edge
	GraphNode* leftNode = _graph.addNode();
	vecRemove(graphPath.front()->nodeRight->inEdges, graphPath.front());
	graphPath.front()->nodeRight = leftNode;
	leftNode->inEdges.push_back(graphPath.front());
	//int32_t pathCoverage = (graphPath.front()->meanCoverage +
	//					    graphPath.back()->meanCoverage) / 2;

	//repetitive edges in the middle
	for (size_t i = 1; i < graphPath.size() - 1; ++i)
	{
		graphPath[i]->resolved = true;
		//graphPath[i]->meanCoverage = 
		//	std::max(graphPath[i]->meanCoverage - pathCoverage, 0);
	}

	GraphNode* rightNode = leftNode;
	if (graphPath.size() > 2)
	{
		rightNode = _graph.addNode();
		GraphEdge* newEdge = _graph.addEdge(GraphEdge(leftNode, rightNode,
													  newId));
		newEdge->seqSegments.push_back(readSegment);
		newEdge->meanCoverage = _multInf.getMeanCoverage();
	}

	//last edge
	vecRemove(graphPath.back()->nodeLeft->outEdges, graphPath.back());
	graphPath.back()->nodeLeft = rightNode;
	rightNode->outEdges.push_back(graphPath.back());
}

int RepeatResolver::resolveConnections(const std::vector<Connection>& connections)
{
	///////////
	/*
	std::unordered_map<GraphEdge*, std::unordered_map<GraphEdge*, int>> stats;
	for (auto& conn : connections)
	{
		++stats[conn.path.front()][conn.path.back()];
	}
	
	for (auto& leftEdge : stats)
	{
		Logger::get().debug() << "For " << leftEdge.first->edgeId.signedId() << " "
			<< leftEdge.first->seqSegments.front().seqId << " "
			<< leftEdge.first->seqSegments.front().end;
		
		for (auto& rightEdge : leftEdge.second)
		{
			Logger::get().debug() << "\t" << rightEdge.first->edgeId.signedId() << " "
				<< rightEdge.first->seqSegments.front().seqId << " "
				<< rightEdge.first->seqSegments.front().start << " " 
				<< rightEdge.second;
		}
		Logger::get().debug() << "";
	}*/
	///////////
	std::unordered_map<GraphEdge*, int> leftCoverage;
	std::unordered_map<GraphEdge*, int> rightCoverage;
	
	//create bipartie graph matrix
	std::unordered_map<GraphEdge*, size_t> leftEdgesId;
	std::unordered_map<size_t, GraphEdge*> leftIdToEdge;
	size_t nextLeftId = 0;
	std::unordered_map<GraphEdge*, size_t> rightEdgesId;
	std::unordered_map<size_t, GraphEdge*> rightIdToEdge;
	size_t nextRightId = 0;

	for (auto& conn : connections)
	{
		GraphEdge* leftEdge = conn.path.front();
		GraphEdge* rightEdge = conn.path.back();
		++leftCoverage[leftEdge];
		++rightCoverage[rightEdge];

		if (!leftEdgesId.count(leftEdge))
		{
			leftEdgesId[leftEdge] = nextLeftId;
			leftIdToEdge[nextLeftId++] = leftEdge;
		}
		if (!rightEdgesId.count(rightEdge))
		{
			rightEdgesId[rightEdge] = nextRightId;
			rightIdToEdge[nextRightId++] = rightEdge;
		}
	}

	size_t numNodes = std::max(leftEdgesId.size(), rightEdgesId.size());
	BipartieTable table;
	table.assign(numNodes, std::vector<double>(numNodes, 0));
	for (auto& conn : connections)
	{
		GraphEdge* leftEdge = conn.path.front();
		GraphEdge* rightEdge = conn.path.back();
		if (leftEdge->edgeId == rightEdge->edgeId ||
			leftEdge->edgeId == rightEdge->edgeId.rc()) continue;

		//solving min cost mathcing
		--table[leftEdgesId[leftEdge]][rightEdgesId[rightEdge]];
	}
	auto edges = bipartieMincost(table);
	typedef std::pair<size_t, size_t> MatchPair;
	std::vector<MatchPair> matchingPairs;
	for (size_t i = 0; i < edges.size(); ++i)
	{
		matchingPairs.emplace_back(i, edges[i]);
	}

	std::unordered_set<FastaRecord::Id> usedEdges;
	std::vector<Connection> uniqueConnections;
	int totalLinks = 0;
	int unresolvedLinks = 0;
	for (auto match : matchingPairs)
	{
		GraphEdge* leftEdge = leftIdToEdge[match.first];
		GraphEdge* rightEdge = rightIdToEdge[match.second];

		int support = -table[match.first][match.second];
		float confidence = 2.0f * support / (leftCoverage[leftEdge] + 
											 rightCoverage[rightEdge]);
		if (!support) continue;
		if (usedEdges.count(leftEdge->edgeId)) continue;
		usedEdges.insert(rightEdge->edgeId.rc());

		Logger::get().debug() << "\tConnection " 
			<< leftEdge->edgeId.signedId()
			<< "\t" << rightEdge->edgeId.signedId()
			<< "\t" << support / 2 << "\t" << confidence;

		if (confidence < Constants::minRepeatResSupport) 
		{
			++unresolvedLinks;
			continue;
		}

		totalLinks += 2;
		for (auto& conn : connections)
		{
			//TODO: choose representetive read more carefully
			if (conn.path.front() == leftEdge && 
				conn.path.back() == rightEdge)
			{
				uniqueConnections.push_back(conn);
				break;
			}
		}
	}

	for (auto& conn : uniqueConnections)
	{
		GraphPath complPath = _graph.complementPath(conn.path);
		SequenceSegment complSegment = conn.readSequence.complement();

		FastaRecord::Id edgeId = _graph.newEdgeId();
		this->separatePath(conn.path, conn.readSequence, edgeId);
		this->separatePath(complPath, complSegment, edgeId.rc());
	}

	Logger::get().debug() << "Resolved: " << totalLinks / 2 << " links: "
						  << connections.size() / 2;
	Logger::get().debug() << "Unresolved: " << unresolvedLinks / 2;

	return totalLinks / 2;
}

void RepeatResolver::findRepeats()
{
	Logger::get().debug() << "Finding repeats";

	//all edges are good at the beginning
	for (auto& edge : _graph.iterEdges())
	{
		edge->repetitive = false;
	}

	GraphProcessor proc(_graph, _asmSeqs, _readSeqs);
	auto unbranchingPaths = proc.getUnbranchingPaths();
	std::unordered_map<FastaRecord::Id, UnbranchingPath*> idToPath;
	for (auto& path : unbranchingPaths) idToPath[path.id] = &path;
	auto complPath = [&idToPath](UnbranchingPath* path)
	{
		if (idToPath.count(path->id.rc()))
		{
			return idToPath[path->id.rc()];
		}
		return path;	//self-complement
	};
	auto markRepetitive = [](UnbranchingPath* path)
	{
		for (auto& edge : path->path) edge->repetitive = true;
	};

	//mark edges with high coverage as repetitive
	for (auto& path : unbranchingPaths)
	{
		if (!path.id.strand()) continue;

		if (path.meanCoverage > _multInf.getUniqueCovThreshold() * 2)
		{
			markRepetitive(&path);
			markRepetitive(complPath(&path));
			Logger::get().debug() << "Cov: " 
				<< path.edgesStr() << "\t" << path.length << "\t" 
				<< path.meanCoverage;
		}

		//plus tanem repeats
		if (path.path.size() == 1 && path.path.front()->isLooped())
		{
			std::unordered_set<FastaRecord::Id> seen;
			for (auto& seg : path.path.front()->seqSegments)
			{
				if (seen.count(seg.seqId))
				{
					markRepetitive(&path);
					markRepetitive(complPath(&path));
					Logger::get().debug() << "Tan: " 
						<< path.edgesStr() << "\t" << path.length << "\t" 
						<< path.meanCoverage;
					break;
				}
				seen.insert(seg.seqId);
			}
		}
	}

	//bubbles with alternative alleles
	int diploidBubbles = 0;
	for (auto& edge : _graph.iterEdges())
	{
		if (edge->isLooped()) continue;
		std::vector<GraphEdge*> parallelEdges;
		for (auto& parEdge : edge->nodeLeft->outEdges)
		{
			if (parEdge->nodeRight == edge->nodeRight) 
			{
				parallelEdges.push_back(parEdge);
			}
		}
		if (parallelEdges.size() != 2) continue;
		if (parallelEdges[0]->edgeId == parallelEdges[1]->edgeId.rc()) continue;
		if (parallelEdges[0]->length() > Constants::trustedEdgeLength || 
			parallelEdges[1]->length() > Constants::trustedEdgeLength) continue;

		float covSum = parallelEdges[0]->meanCoverage + 
					   parallelEdges[1]->meanCoverage;
		if (covSum / _multInf.getMeanCoverage() > 1.5) continue;

		parallelEdges[0]->repetitive = true;
		parallelEdges[1]->repetitive = true;
		++diploidBubbles;
	}
	Logger::get().debug() << "Masked " << diploidBubbles / 4 << " diploid bubbles";

	//Now, using read alignments
	//extract read alignments
	std::unordered_map<GraphEdge*, 
					   std::unordered_map<GraphEdge*, int>> outConnections;
	for (auto& readPath : _aligner.getAlignments())
	{
		if (readPath.size() < 2) continue;
		int overhang = std::max(readPath.front().overlap.curBegin,
								readPath.back().overlap.curLen - 
									readPath.back().overlap.curEnd);
		if (overhang > Constants::maximumOverhang) continue;

		for (size_t i = 0; i < readPath.size() - 1; ++i)
		{
			if (readPath[i].edge == readPath[i + 1].edge &&
				readPath[i].edge->isLooped()) continue;
			if (readPath[i].edge->edgeId == 
				readPath[i + 1].edge->edgeId.rc()) continue;

			GraphEdge* complLeft = _graph.complementEdge(readPath[i].edge);
			GraphEdge* complRight = _graph.complementEdge(readPath[i + 1].edge);
			++outConnections[readPath[i].edge][readPath[i + 1].edge];
			++outConnections[complRight][complLeft];
		}
	}
	//computes right multiplicity
	auto rightMultiplicity = [this, &outConnections] (GraphEdge* edge)
	{
		int maxSupport = 0;
		for (auto& outConn : outConnections[edge])
		{
			if (maxSupport < outConn.second)
			{
				maxSupport =  outConn.second;
			}
		}

		int repeatMult = 0;
		int uniqueMult = 0;
		int minSupport = maxSupport / Constants::outPathsRatio;
		for (auto& outConn : outConnections[edge]) 
		{
			if (outConn.second > minSupport)
			{
				outConn.first->repetitive ? ++repeatMult : ++uniqueMult;
			}
		}
		return std::min(repeatMult, 1) + uniqueMult;
	};

	//order might be important, process short edges first
	std::vector<UnbranchingPath*> sortedPaths;
	for (auto& path : unbranchingPaths) sortedPaths.push_back(&path);
	std::sort(sortedPaths.begin(), sortedPaths.end(),
			  [](const UnbranchingPath* p1, const UnbranchingPath* p2) 
			  {return p1->length < p2->length;});
	for (auto& path : sortedPaths)
	{
		if (!path->id.strand()) continue;
		if (path->path.front()->repetitive) continue;

		int rightMult = rightMultiplicity(path->path.back());
		int leftMult = rightMultiplicity(complPath(path)->path.back());
		int mult = std::max(leftMult, rightMult);
		if (mult > 1) 
		{
			markRepetitive(path);
			markRepetitive(complPath(path));

			Logger::get().debug() << "Mult: " 
				<< path->edgesStr() << "\t" << path->length << "\t" 
				<< path->meanCoverage << "\t" << mult << " ("
				<< leftMult << "," << rightMult << ")";
			
			for (auto& outEdgeCount : outConnections[path->path.back()])
			{
				std::string star = outEdgeCount.first->repetitive ? "R" : " ";
				std::string loop = outEdgeCount.first->isLooped() ? "L" : " ";
				Logger::get().debug() << "+\t" << star << " " << loop << " " 
					<< outEdgeCount.first->edgeId.signedId() << "\t" << outEdgeCount.second;
			}
			for (auto& outEdgeCount : outConnections[complPath(path)->path.back()])
			{
				std::string star = outEdgeCount.first->repetitive ? "R" : " ";
				std::string loop = outEdgeCount.first->isLooped() ? "L" : " ";
				Logger::get().debug() << "-\t" << star << " " << loop << " " 
					<< outEdgeCount.first->edgeId.signedId() << "\t" << outEdgeCount.second;
			}
		}
	}
}

void RepeatResolver::resolveRepeats()
{
	this->removeUnsupportedEdges();
	_aligner.updateAlignments();

	while (true)
	{
		auto connections = this->getConnections();
		int resolvedConnections = this->resolveConnections(connections);
		this->clearResolvedRepeats();
		_aligner.updateAlignments();
		if (!resolvedConnections) break;
		this->findRepeats();
	}
}


std::vector<RepeatResolver::Connection> 
	RepeatResolver::getConnections()
{
	
	auto safeEdge = [this](GraphEdge* edge)
	{
		return !edge->isRepetitive();
	};

	int totalSafe = 0;
	for (GraphEdge* edge : _graph.iterEdges())
	{
		if (edge->edgeId.strand() && safeEdge(edge)) ++totalSafe;
	}
	Logger::get().debug() << "Total unique edges: " << totalSafe;

	std::vector<Connection> readConnections;
	for (auto& readPath : _aligner.getAlignments())
	{
		GraphPath currentPath;
		int32_t readStart = 0;
		for (auto& aln : readPath)
		{
			if (currentPath.empty()) 
			{
				if (!safeEdge(aln.edge)) continue;
				readStart = aln.overlap.curEnd + aln.overlap.extLen - 
							aln.overlap.extEnd;
			}

			currentPath.push_back(aln.edge);
			if (safeEdge(aln.edge) && currentPath.size() > 1)
			{
				if (!currentPath.back()->nodeLeft->isBifurcation() &&
					!currentPath.front()->nodeRight->isBifurcation()) continue;

				GraphPath complPath = _graph.complementPath(currentPath);

				int32_t readEnd = aln.overlap.curBegin - aln.overlap.extBegin;
				readEnd = std::max(readStart + 100, readEnd);
				SequenceSegment segment(aln.overlap.curId, aln.overlap.curLen, 
										readStart, readEnd);
				segment.readSequence = true;
				SequenceSegment complSegment = segment.complement();

				readConnections.push_back({currentPath, segment});
				readConnections.push_back({complPath, complSegment});

				currentPath.clear();
				currentPath.push_back(aln.edge);
				readStart = aln.overlap.curEnd + aln.overlap.extLen - 
							aln.overlap.extEnd;
			}
		}
	}

	return readConnections;
}

void RepeatResolver::removeUnsupportedEdges()
{
	int coverageThreshold = _multInf.getMeanCoverage() / Constants::readCovRate;
	Logger::get().debug() << "Read coverage cutoff: " << coverageThreshold;

	std::unordered_set<GraphEdge*> edgesRemove;
	for (auto& edge : _graph.iterEdges())
	{
		GraphEdge* complEdge = _graph.complementEdge(edge);
		if (edge->meanCoverage <= coverageThreshold)
		{
			edgesRemove.insert(edge);
			edgesRemove.insert(complEdge);
		}
	}
	for (auto& edge : edgesRemove) _graph.removeEdge(edge);
	Logger::get().debug() << "Removed " << edgesRemove.size() 
		<< " unsupported edges";
}

void RepeatResolver::clearResolvedRepeats()
{
	const int MIN_LOOP = Parameters::get().minimumOverlap;
	auto nextEdge = [](GraphNode* node)
	{
		for (auto edge : node->outEdges)
		{
			if (!edge->isLooped()) return edge;
		}
		return (GraphEdge*)nullptr;
	};

	auto shouldRemove = [](GraphEdge* edge)
	{
		//return edge->isRepetitive() && edge->resolved;
		return edge->resolved;
	};

	std::unordered_set<GraphNode*> toRemove;

	for (auto& node : _graph.iterNodes())
	{
		//separated nodes
		if (node->neighbors().size() == 0)
		{
			bool resolved = true;
			for (auto& edge : node->outEdges) 
			{
				if (!shouldRemove(edge) &&
					edge->length() > MIN_LOOP) resolved = false;
			}

			if (resolved) toRemove.insert(node);
		}

		//other nodes
		if (!node->isEnd()) continue;

		GraphEdge* direction = nextEdge(node);
		if (!direction) continue;

		GraphPath traversed;
		traversed.push_back(direction);
		GraphNode* curNode = direction->nodeRight;
		while (curNode->isResolved())
		{
			traversed.push_back(nextEdge(curNode));
			curNode = traversed.back()->nodeRight;
		}
		if (traversed.empty()) continue;

		bool removeLast = curNode->isEnd();
		bool resolvedRepeat = true;
		for (auto& edge : traversed) 
		{
			if (!shouldRemove(edge)) resolvedRepeat = false;
		}

		GraphPath complPath = _graph.complementPath(traversed);
		if (resolvedRepeat)
		{
			//first-last
			toRemove.insert(traversed.front()->nodeLeft);
			if (removeLast) toRemove.insert(complPath.front()->nodeLeft);

			//middle nodes
			for (size_t i = 0; i < traversed.size() - 1; ++i)
			{
				toRemove.insert(traversed[i]->nodeRight);
				toRemove.insert(complPath[i]->nodeRight);
			}

			//last-first
			if (removeLast) toRemove.insert(traversed.back()->nodeRight);
			toRemove.insert(complPath.back()->nodeRight);
		}
	}

	for (auto node : toRemove) _graph.removeNode(node);
}
