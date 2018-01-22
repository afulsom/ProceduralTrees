#include "Engine/PCG/FoliageGenerator.hpp"
#include "Engine/Math/Vector3.hpp"
#include <vector>
#include "Engine/Math/MathUtils.hpp"
#include "Engine/PCG/LeafGenerator.hpp"


enum TreeNodeType
{
	TREENODE_SPLINE,
	TREENODE_LINEAR,
	TREENODE_ROTATION,
	NUM_TREENODETYPES
};

struct TreeNode
{
	TreeNode(Vector3 pos, Vector3 ypr, float width, float length, TreeNodeType type, TreeNode* par)
		: position(pos)
		, orientationYPR(ypr)
		, parent(par)
		, type(type)
		, width(width)
		, length(length)
	{

	}

	Vector3 position;
	Vector3 orientationYPR;
	float width;
	float length;
	std::vector<TreeNode*> children;
	TreeNodeType type;
	TreeNode* parent;
};

void AddSectionOfVertsToFoliageFromPositionsTangentsWidths(Vector3 &startForwardVector, Vector3 &endForwardVector, unsigned int numSlices, Foliage*& foliage, Vector3 startPosition, float startWidth, float startLength, Vector3 endPosition, float endWidth, float endLength)
{

	Vector3 startPerpendicularVector = Vector3::Y_AXIS;
	if (startForwardVector.IsMostlyEqualTo(Vector3::Y_AXIS))
	{
		startPerpendicularVector = Vector3::X_AXIS;
	}

	Vector3 startRightVector = CrossProduct(startForwardVector, startPerpendicularVector);
	Vector3 startUpVector = CrossProduct(startForwardVector, startRightVector);

	startForwardVector.Normalize();
	startRightVector.Normalize();
	startUpVector.Normalize();

	Vector3 endPerpendicularVector = Vector3::Y_AXIS;
	if (endForwardVector.IsMostlyEqualTo(Vector3::Y_AXIS))
	{
		endPerpendicularVector = Vector3::X_AXIS;
	}

	Vector3 endRightVector = CrossProduct(endForwardVector, endPerpendicularVector);
	Vector3 endUpVector = CrossProduct(endForwardVector, endRightVector);

	endForwardVector.Normalize();
	endUpVector.Normalize();
	endRightVector.Normalize();

	float deltaTheta = 360.f / numSlices;
	float theta = 0.f;
	for (unsigned int sliceIndex = 0; sliceIndex < numSlices; sliceIndex++)
	{
		Vector3 bottomLeftOffsetVector = (CosDegrees(theta) * startRightVector) + (SinDegrees(theta) * startUpVector);
		Vector3 bottomRightOffsetVector = (CosDegrees(theta + deltaTheta) * startRightVector) + (SinDegrees(theta + deltaTheta) * startUpVector);
		Vector3 topLeftOffsetVector = (CosDegrees(theta) * endRightVector) + (SinDegrees(theta) * endUpVector);
		Vector3 topRightOffsetVector = (CosDegrees(theta + deltaTheta) * endRightVector) + (SinDegrees(theta + deltaTheta) * endUpVector);

		float tangentTheta = theta - 90.f;
		Vector3 bottomLeftOffsetTangent = (CosDegrees(tangentTheta) * startRightVector) + (SinDegrees(tangentTheta) * startUpVector);
		Vector3 bottomRightOffsetTangent = (CosDegrees(tangentTheta + deltaTheta) * startRightVector) + (SinDegrees(tangentTheta + deltaTheta) * startUpVector);
		Vector3 topLeftOffsetTangent = (CosDegrees(tangentTheta) * endRightVector) + (SinDegrees(tangentTheta) * endUpVector);
		Vector3 topRightOffsetTangent = (CosDegrees(tangentTheta + deltaTheta) * endRightVector) + (SinDegrees(tangentTheta + deltaTheta) * endUpVector);

		float startU = RangeMapFloat(theta, 0.f, 360.f, 0.f, 1.f);
		float endU = RangeMapFloat(theta + deltaTheta, 0.f, 360.f, 0.f, 1.f);
		float startV = RangeMapFloat(startLength, 0.f, 30.f, 0.f, 1.f);
		float endV = RangeMapFloat(endLength, 0.f, 30.f, 0.f, 1.f);

		Vector2 bottomLeftUV(startU, startV);
		Vector2 bottomRightUV(endU, startV);
		Vector2 topLeftUV(startU, endV);
		Vector2 topRightUV(endU, endV);

		//Left tri
		foliage->m_treeMesh.SetNormal(bottomRightOffsetVector);
		foliage->m_treeMesh.SetTangent(bottomRightOffsetTangent);
		foliage->m_treeMesh.SetUV(bottomRightUV);
// 		foliage->m_mesh.SetTint(Rgba::RED);
		foliage->m_treeMesh.AddVertex(startPosition + (bottomRightOffsetVector * startWidth));

		foliage->m_treeMesh.SetNormal(bottomLeftOffsetVector);
		foliage->m_treeMesh.SetTangent(bottomLeftOffsetTangent);
		foliage->m_treeMesh.SetUV(bottomLeftUV);
// 		foliage->m_mesh.SetTint(Rgba::RED);
		foliage->m_treeMesh.AddVertex(startPosition + (bottomLeftOffsetVector * startWidth));

		foliage->m_treeMesh.SetNormal(topLeftOffsetVector);
		foliage->m_treeMesh.SetTangent(topLeftOffsetTangent);
		foliage->m_treeMesh.SetUV(topLeftUV);
// 		foliage->m_mesh.SetTint(Rgba::RED);
		foliage->m_treeMesh.AddVertex(endPosition + (topLeftOffsetVector * endWidth));

		//Right tri
		foliage->m_treeMesh.SetNormal(bottomRightOffsetVector);
		foliage->m_treeMesh.SetTangent(bottomRightOffsetTangent);
		foliage->m_treeMesh.SetUV(bottomRightUV);
// 		foliage->m_mesh.SetTint(Rgba::BLUE);
		foliage->m_treeMesh.AddVertex(startPosition + (bottomRightOffsetVector * startWidth));

		foliage->m_treeMesh.SetNormal(topLeftOffsetVector);
		foliage->m_treeMesh.SetTangent(topLeftOffsetTangent);
		foliage->m_treeMesh.SetUV(topLeftUV);
// 		foliage->m_mesh.SetTint(Rgba::BLUE);
		foliage->m_treeMesh.AddVertex(endPosition + (topLeftOffsetVector * endWidth));

		foliage->m_treeMesh.SetNormal(topRightOffsetVector);
		foliage->m_treeMesh.SetTangent(topRightOffsetTangent);
		foliage->m_treeMesh.SetUV(topRightUV);
// 		foliage->m_mesh.SetTint(Rgba::BLUE);
		foliage->m_treeMesh.AddVertex(endPosition + (topRightOffsetVector * endWidth));

		theta += deltaTheta;
	}
}

Vector3 AccumulateRotationFromChildren(TreeNode* node)
{
	Vector3 rotation = Vector3::ZERO;

	for (TreeNode* child : node->children)
	{
		if (child->type == TREENODE_ROTATION)
		{
			rotation += AccumulateRotationFromChildren(child) + child->orientationYPR;
		}
	}

	return rotation;
}

void AddSectionOfVertsToFoliage(Foliage*& foliage, TreeNode* start, TreeNode* end, unsigned int numSlices)
{
	Vector3 startTangentVector = Vector3::MakeDirectionVectorFromYawPitchDegrees(start->orientationYPR.x, start->orientationYPR.y);
	Vector3 endOrientation = end->orientationYPR;
	Vector3 endTangentVector = Vector3::MakeDirectionVectorFromYawPitchDegrees(endOrientation.x, endOrientation.y);

	Vector3 startPosition = start->position;
	Vector3 endPosition = end->position;

	float startWidth = start->width;
	float endWidth = end->width;
	switch (end->type)
	{
	case TREENODE_ROTATION:
		AddSectionOfVertsToFoliage(foliage, start, end->children[0], numSlices);
		break;
	case TREENODE_LINEAR:
		AddSectionOfVertsToFoliageFromPositionsTangentsWidths(startTangentVector, endTangentVector, numSlices, foliage, startPosition, startWidth, start->length, endPosition, endWidth, end->length);
		break;
	case TREENODE_SPLINE:
	{
		Vector3 splineStartPosition = CalculateHermiteCubicSplinePosition(startPosition, endPosition, startTangentVector, endTangentVector, 0.f);
		Vector3 splineStartTangent = CalculateHermiteCubicSplineTangent(startPosition, endPosition, startTangentVector, endTangentVector, 0.f);

		float deltaT = 0.2f;
		for (float t = deltaT; t < 1.1f; t += deltaT)
		{
			ClampFloat(t, 0.f, 1.f);
			Vector3 splineEndPosition = CalculateHermiteCubicSplinePosition(startPosition, endPosition, startTangentVector, endTangentVector, t);
			Vector3 splineEndTangent = CalculateHermiteCubicSplineTangent(startPosition, endPosition, startTangentVector, endTangentVector, t);

			AddSectionOfVertsToFoliageFromPositionsTangentsWidths(splineStartTangent, splineEndTangent, numSlices, foliage, splineStartPosition, Interpolate(startWidth, endWidth, t - deltaT), Interpolate(start->length, end->length, t - deltaT), splineEndPosition, Interpolate(startWidth, endWidth, t), Interpolate(start->length, end->length, t));

			splineStartPosition = splineEndPosition;
			splineStartTangent = splineEndTangent;
		}
		break;
	}
	}
}

void AddVertsForTree(TreeNode* node, Foliage* foliage, unsigned int numSlices)
{
	for (TreeNode* child : node->children)
	{
		AddVertsForTree(child, foliage, numSlices);
		AddSectionOfVertsToFoliage(foliage, node, child, numSlices);
	}
}


FoliageGenerator::FoliageGenerator(unsigned int systemIterations, const TreeParameters& trunkParameters, const TreeParameters& limbParameters, const TreeParameters& branchParameters, const FloatRange& xSizeRange, const FloatRange& ySizeRange, Texture2D* barkAlbedo /*= nullptr*/, Texture2D* barkNormal /*= nullptr*/, Texture2D* barkSpecular /*= nullptr*/, Texture2D* leafTop /*= nullptr*/, Texture2D* leafBottom /*= nullptr*/)
	: m_systemIterations(systemIterations)
	, m_trunkParameters(trunkParameters)
	, m_limbParameters(limbParameters)
	, m_branchParameters(branchParameters)
	, m_leafXSizeRange(xSizeRange)
	, m_leafYSizeRange(ySizeRange)
	, m_barkAlbedo(barkAlbedo)
	, m_barkNormal(barkNormal)
	, m_barkSpecular(barkSpecular)
	, m_leafTop(leafTop)
	, m_leaftBottom(leafBottom)
{

}

Foliage* FoliageGenerator::GenerateFoliage()
{
	std::string system = m_systems[0].Run();

	for (size_t systemIndex = 1; systemIndex < m_systems.size(); systemIndex++)
	{
		m_systems[systemIndex].m_axiom = system;
		system = m_systems[systemIndex].Run();
	}

	Foliage* generatedFoliage = new Foliage();
	generatedFoliage->m_treeMesh.Begin(PRIMITIVE_TRIANGLES, false);

	float currentWidth = m_trunkParameters.m_width.GetRandomFloatInRange();

	TreeNode* head = new TreeNode(Vector3::ZERO, Vector3(90.f, 0.f, 0.f), currentWidth, 0.f, TREENODE_SPLINE, nullptr);
	TreeNode* root = head;
	std::vector<TreeNode*> returnPointStack;

	for (const char character : system)
	{
		switch (character)
		{
		case 'T': 
		{
			Vector3 segmentToAdd = Vector3::MakeDirectionVectorFromYawPitchDegrees(head->orientationYPR.x, head->orientationYPR.y);
			float lengthToAdd = m_trunkParameters.m_length.GetRandomFloatInRange();
			segmentToAdd = segmentToAdd * lengthToAdd;
			currentWidth = m_trunkParameters.m_width.GetRandomFloatInRange();
			TreeNode* newSegment = new TreeNode(head->position + segmentToAdd, head->orientationYPR, currentWidth, head->length + lengthToAdd, TREENODE_LINEAR, head);
			head->children.push_back(newSegment);
			head = newSegment;

			generatedFoliage->m_positions.push_back(head->parent->position);
			generatedFoliage->m_positions.push_back(head->position);

			generatedFoliage->m_colors.push_back(Rgba::RED);
			generatedFoliage->m_colors.push_back(Rgba::RED);

// 			AddSectionOfVertsToFoliage(generatedFoliage, head->parent, head, 18);

			break;
		}
		case 'L':
		{
			Vector3 segmentToAdd = Vector3::MakeDirectionVectorFromYawPitchDegrees(head->orientationYPR.x, head->orientationYPR.y);
			float lengthToAdd = m_limbParameters.m_length.GetRandomFloatInRange();
			segmentToAdd = segmentToAdd * lengthToAdd;
			currentWidth = m_limbParameters.m_width.GetRandomFloatInRange();
			TreeNode* newSegment = new TreeNode(head->position + segmentToAdd, head->orientationYPR, currentWidth, head->length + lengthToAdd, TREENODE_LINEAR, head);
			head->children.push_back(newSegment);
			head = newSegment;

			generatedFoliage->m_positions.push_back(head->parent->position);
			generatedFoliage->m_positions.push_back(head->position);

			generatedFoliage->m_colors.push_back(Rgba::BLUE);
			generatedFoliage->m_colors.push_back(Rgba::BLUE);

			// 			AddSectionOfVertsToFoliage(generatedFoliage, head->parent, head, 18);

			break;
		}
		case 'B':
		{
			Vector3 segmentToAdd = Vector3::MakeDirectionVectorFromYawPitchDegrees(head->orientationYPR.x, head->orientationYPR.y);
			float lengthToAdd = m_branchParameters.m_length.GetRandomFloatInRange();
			segmentToAdd = segmentToAdd * lengthToAdd;
			currentWidth = m_branchParameters.m_width.GetRandomFloatInRange();
			TreeNode* newSegment = new TreeNode(head->position + segmentToAdd, head->orientationYPR, currentWidth, head->length + lengthToAdd, TREENODE_LINEAR, head);
			head->children.push_back(newSegment);
			head = newSegment;

			generatedFoliage->m_positions.push_back(head->parent->position);
			generatedFoliage->m_positions.push_back(head->position);

			generatedFoliage->m_colors.push_back(Rgba::WHITE);
			generatedFoliage->m_colors.push_back(Rgba::WHITE);

			// 			AddSectionOfVertsToFoliage(generatedFoliage, head->parent, head, 18);

			break;
		}
		case 'S': 
		{
			Vector3 segmentToAdd = Vector3::MakeDirectionVectorFromYawPitchDegrees(head->orientationYPR.x, head->orientationYPR.y);
			float lengthToAdd = m_trunkParameters.m_length.GetRandomFloatInRange();
			segmentToAdd = segmentToAdd * lengthToAdd;
			currentWidth = m_trunkParameters.m_width.GetRandomFloatInRange();
			TreeNode* newSegment = new TreeNode(head->position + segmentToAdd, head->orientationYPR, currentWidth, head->length + lengthToAdd, TREENODE_SPLINE, head);
			head->children.push_back(newSegment);
			head = newSegment;

			generatedFoliage->m_positions.push_back(head->parent->position);
			generatedFoliage->m_positions.push_back(head->position);

			generatedFoliage->m_colors.push_back(Rgba::GREEN);
			generatedFoliage->m_colors.push_back(Rgba::GREEN);

			break;
		}
		case 'X':
		{
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR, 0.f, head->length, TREENODE_LINEAR, head);
			head->children.push_back(newSegment);
			head = newSegment;

			break;
		}
		case 'C':
		{
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR, 0.f, head->length, TREENODE_LINEAR, head);
			head->children.push_back(newSegment);
			head = newSegment;

			generatedFoliage->m_leafGenerators.push_back(LeafGenerator(head->position, head->orientationYPR, 7.f, 10, 18, m_leafXSizeRange.m_min, m_leafXSizeRange.m_max, m_leafYSizeRange.m_min, m_leafYSizeRange.m_max));

			break;
		}
		case '-':
		{
			Vector3 rotationToAdd(m_trunkParameters.m_yaw.GetRandomFloatInRange(), m_trunkParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_trunkParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case '+':
		{
			Vector3 rotationToAdd(-m_trunkParameters.m_yaw.GetRandomFloatInRange(), -m_trunkParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_trunkParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case '<':
		{
			Vector3 rotationToAdd(m_limbParameters.m_yaw.GetRandomFloatInRange(), m_limbParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_limbParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case '>':
		{
			Vector3 rotationToAdd(-m_limbParameters.m_yaw.GetRandomFloatInRange(), -m_limbParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_limbParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case '(':
		{
			Vector3 rotationToAdd(m_branchParameters.m_yaw.GetRandomFloatInRange(), m_branchParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_branchParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case ')':
		{
			Vector3 rotationToAdd(-m_branchParameters.m_yaw.GetRandomFloatInRange(), -m_branchParameters.m_yaw.GetRandomFloatInRange(), 0.f);
			TreeNode* newSegment = new TreeNode(head->position, head->orientationYPR + rotationToAdd, m_branchParameters.m_width.GetRandomFloatInRange(), head->length, TREENODE_ROTATION, head);
			head->children.push_back(newSegment);
			head = newSegment;
			break;
		}
		case '[':
		{
			returnPointStack.push_back(head);
			break;
		}
		case ']':
		{
			head = returnPointStack.back();
			returnPointStack.pop_back();
			break;
		}
		default:
			break;
		}
	}

	generatedFoliage->m_treeMesh.Begin(PRIMITIVE_TRIANGLES, false);
	generatedFoliage->m_treeMesh.SetUV(Vector2::ZERO);
	AddVertsForTree(root, generatedFoliage, m_numSlices);
	generatedFoliage->m_treeMesh.End();

	generatedFoliage->AddLeafVerts();

	generatedFoliage->m_barkAlbedo = m_barkAlbedo;
	generatedFoliage->m_barkNormal = m_barkNormal;
	generatedFoliage->m_barkSpecular = m_barkSpecular;
	generatedFoliage->m_leafTop = m_leafTop;
	generatedFoliage->m_leafBottom = m_leaftBottom;
	return generatedFoliage;
}

void FoliageGenerator::AddSystem(LSystem system)
{
	m_systems.push_back(system);
}

