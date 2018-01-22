#include "Engine/PCG/Foliage.hpp"
#include "Engine/Math/MathUtils.hpp"



Foliage::Foliage()
{

}

void Foliage::AddLeafVerts()
{
	m_leafMesh.Begin(PRIMITIVE_TRIANGLES, false);

	for (LeafGenerator generator : m_leafGenerators)
	{
		GenerateLeavesForGenerator(generator);
	}
}

void Foliage::GenerateLeavesForGenerator(const LeafGenerator& generator)
{
	int numLeaves = GetRandomIntInRange(generator.m_leafCount.x, generator.m_leafCount.y);

	for (int leafIndex = 0; leafIndex < numLeaves; leafIndex++)
	{
		float yawJitter = GetRandomFloatInRange(-20.f, 20.f);
		float pitchJitter = GetRandomFloatInRange(-20.f, 20.f);
		float radius = GetRandomFloatInRange(0.f, generator.m_maxRadius);
		float theta = GetRandomFloatInRange(0.f, 360.f);

		Vector3 forward = Vector3::MakeDirectionVectorFromYawPitchDegrees(generator.m_orientation.x + yawJitter, generator.m_orientation.y + pitchJitter);
		// 	Vector3 right = Vector3::MakeDirectionVectorFromYawPitchDegrees(-generator.m_orientation.y, generator.m_orientation.x);
		Vector3 right = CrossProduct(forward, Vector3::Y_AXIS);
		Vector3 up = CrossProduct(forward, right);
		right = CrossProduct(forward, up);
		Vector3 halfForward = forward * 0.5f;
		Vector3 halfRight = right * 0.5f;
		Vector3 halfUp = up * 0.5f;

		Vector3 leafPosition = generator.m_position + (radius * (forward * SinDegrees(theta) + right * CosDegrees(theta))) + (up * generator.m_leafNormalOffset.GetRandomFloatInRange());

		float perpendicularScale = generator.m_xSizeRange.GetRandomFloatInRange();

		Vector3 scaledHalfForward = halfForward * generator.m_ySizeRange.GetRandomFloatInRange();
		Vector3 scaledHalfRight = halfRight * perpendicularScale;
		Vector3 scaledHalfUp = halfUp * perpendicularScale;


		//Horizontal
		Vector3 frontLeft = leafPosition + scaledHalfForward - scaledHalfRight;
		Vector3 frontRight = leafPosition + scaledHalfForward + scaledHalfRight;
		Vector3 backLeft = leafPosition - scaledHalfForward - scaledHalfRight;
		Vector3 backRight = leafPosition - scaledHalfForward + scaledHalfRight;

		m_leafMesh.SetNormal(up * -1.f);
		m_leafMesh.SetTangent(right * -1.f);

		//top
		m_leafMesh.SetTint(Rgba(103, 184, 98, 255));
		m_leafMesh.SetUV(Vector2(0.f, 0.f));
		m_leafMesh.AddVertex(frontLeft);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backLeft);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontRight);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontRight);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backLeft);

		m_leafMesh.SetUV(Vector2(1.f, 1.f));
		m_leafMesh.AddVertex(backRight);


		//bottom
		m_leafMesh.SetNormal(up);
		m_leafMesh.SetTangent(right);

		m_leafMesh.SetUV(Vector2(0.f, 0.f));
		m_leafMesh.AddVertex(frontLeft);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontRight);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backLeft);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontRight);

		m_leafMesh.SetUV(Vector2(1.f, 1.f));
		m_leafMesh.AddVertex(backRight);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backLeft);


		//Vertical
		Vector3 frontTop = leafPosition + scaledHalfForward - scaledHalfUp;
		Vector3 frontBottom = leafPosition + scaledHalfForward + scaledHalfUp;
		Vector3 backTop = leafPosition - scaledHalfForward - scaledHalfUp;
		Vector3 backBottom = leafPosition - scaledHalfForward + scaledHalfUp;

		m_leafMesh.SetNormal(right);
		m_leafMesh.SetTangent(up);

		//top
		m_leafMesh.SetUV(Vector2(0.f, 0.f));
		m_leafMesh.AddVertex(frontTop);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backTop);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontBottom);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontBottom);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backTop);

		m_leafMesh.SetUV(Vector2(1.f, 1.f));
		m_leafMesh.AddVertex(backBottom);

		//bottom
		m_leafMesh.SetNormal(right * -1.f);
		m_leafMesh.SetTangent(up * -1.f);

		m_leafMesh.SetUV(Vector2(0.f, 0.f));
		m_leafMesh.AddVertex(frontTop);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontBottom);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backTop);

		m_leafMesh.SetUV(Vector2(1.f, 0.f));
		m_leafMesh.AddVertex(frontBottom);

		m_leafMesh.SetUV(Vector2(1.f, 1.f));
		m_leafMesh.AddVertex(backBottom);

		m_leafMesh.SetUV(Vector2(0.f, 1.f));
		m_leafMesh.AddVertex(backTop);
	}
}
