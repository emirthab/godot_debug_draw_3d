#include "geometry_generators.h"
#include "utils.h"

#include <functional>

#pragma region Predefined Geometry Parts

const float GeometryGenerator::CubeDiaganolLengthForSphere = 0.8660253882f; // "%.10f" % (Vector3.ONE * 0.5).length()

const Vector3 GeometryGenerator::CenteredCubeVertices[8]{
	Vector3(-0.5f, -0.5f, -0.5f),
	Vector3(0.5f, -0.5f, -0.5f),
	Vector3(0.5f, -0.5f, 0.5f),
	Vector3(-0.5f, -0.5f, 0.5f),
	Vector3(-0.5f, 0.5f, -0.5f),
	Vector3(0.5f, 0.5f, -0.5f),
	Vector3(0.5f, 0.5f, 0.5f),
	Vector3(-0.5f, 0.5f, 0.5f)
};

const Vector3 GeometryGenerator::CubeVertices[8]{
	Vector3(0, 0, 0),
	Vector3(1, 0, 0),
	Vector3(1, 0, 1),
	Vector3(0, 0, 1),
	Vector3(0, 1, 0),
	Vector3(1, 1, 0),
	Vector3(1, 1, 1),
	Vector3(0, 1, 1)
};

const int GeometryGenerator::CubeIndices[24]{
	0,
	1,
	1,
	2,
	2,
	3,
	3,
	0,

	4,
	5,
	5,
	6,
	6,
	7,
	7,
	4,

	0,
	4,
	1,
	5,
	2,
	6,
	3,
	7,
};

const int GeometryGenerator::CubeWithDiagonalsIndices[36]{
	0, 1,
	1, 2,
	2, 3,
	3, 0,

	4, 5,
	5, 6,
	6, 7,
	7, 4,

	0, 4,
	1, 5,
	2, 6,
	3, 7,

	// Diagonals

	// Top Bottom
	1, 3,
	// 0, 2,
	4, 6,
	// 5, 7,

	// Front Back
	1, 4,
	// 0, 5,
	3, 6,
	// 2, 7,

	// Left Right
	3, 4,
	// 0, 7,
	1, 6,
	// 2, 5,
};

const Vector3 GeometryGenerator::ArrowheadVertices[6]{
	Vector3(0, 0, -1),
	Vector3(0, 0.25f, 0),
	Vector3(0, -0.25f, 0),
	Vector3(0.25f, 0, 0),
	Vector3(-0.25f, 0, 0),
	// Cross to center
	Vector3(0, 0, -0.2f),
};

const int GeometryGenerator::ArrowheadIndices[16]{
	0,
	1,
	0,
	2,
	0,
	3,
	0,
	4,
	// Cross
	// 1, 2,
	// 3, 4,
	// Or Cross to center
	5,
	1,
	5,
	2,
	5,
	3,
	5,
	4,
};

const Vector3 GeometryGenerator::CenteredSquareVertices[4]{
	Vector3(0.5f, 0.5f, 0),
	Vector3(0.5f, -0.5f, 0),
	Vector3(-0.5f, -0.5f, 0),
	Vector3(-0.5f, 0.5f, 0),
};

const int GeometryGenerator::SquareIndices[6]{
	0,
	1,
	2,
	2,
	3,
	0,
};

const Vector3 GeometryGenerator::PositionVertices[6]{
	Vector3(0.5f, 0, 0),
	Vector3(-0.5f, 0, 0),
	Vector3(0, 0.5f, 0),
	Vector3(0, -0.5f, 0),
	Vector3(0, 0, 0.5f),
	Vector3(0, 0, -0.5f),
};

const int GeometryGenerator::PositionIndices[6]{
	0,
	1,
	2,
	3,
	4,
	5,
};

#pragma endregion

PoolVector3Array GeometryGenerator::CreateCameraFrustumLines(Plane frustum[6]) {
	PoolVector3Array res;
	res.resize(C_ARR_SIZE(CubeIndices));

	std::function<Vector3(Plane &, Plane &, Plane &)> intersect_planes = [&](Plane &a, Plane &b, Plane &c) {
		Vector3 intersec_result;
		a.intersect_3(b, c, &intersec_result);
		return intersec_result;
	};

	//  near, far, left, top, right, bottom
	//  0,    1,   2,    3,   4,     5
	Vector3 cube[]{
		intersect_planes(frustum[0], frustum[3], frustum[2]),
		intersect_planes(frustum[0], frustum[3], frustum[4]),
		intersect_planes(frustum[0], frustum[5], frustum[4]),
		intersect_planes(frustum[0], frustum[5], frustum[2]),

		intersect_planes(frustum[1], frustum[3], frustum[2]),
		intersect_planes(frustum[1], frustum[3], frustum[4]),
		intersect_planes(frustum[1], frustum[5], frustum[4]),
		intersect_planes(frustum[1], frustum[5], frustum[2]),
	};

	{
		auto w = res.write();
		for (int i = 0; i < res.size(); i++)
			w[i] = cube[CubeIndices[i]];
	}

	return res;
}

PoolVector3Array GeometryGenerator::CreateCubeLines(Vector3 position, Quat rotation, Vector3 size, bool centeredBox, bool withDiagonals) {
	Vector3 scaled[8];
	PoolVector3Array res_with_diags;
	res_with_diags.resize(C_ARR_SIZE(CubeWithDiagonalsIndices));
	PoolVector3Array res;
	res.resize(C_ARR_SIZE(CubeIndices));

	bool dont_rot = rotation == Quat::IDENTITY;

	std::function<Vector3(int)> get;
	if (centeredBox) {
		if (dont_rot)
			get = [&](int idx) { return CenteredCubeVertices[idx] * size + position; };
		else
			get = [&](int idx) { return rotation.xform(CenteredCubeVertices[idx] * size) + position; };
	} else {
		if (dont_rot)
			get = [&](int idx) { return CubeVertices[idx] * size + position; };
		else
			get = [&](int idx) { return rotation.xform(CubeVertices[idx] * size) + position; };
	}

	for (int i = 0; i < 8; i++)
		scaled[i] = get(i);

	if (withDiagonals) {
		{
			auto w = res_with_diags.write();
			for (int i = 0; i < C_ARR_SIZE(CubeWithDiagonalsIndices); i++)
				w[i] = scaled[CubeWithDiagonalsIndices[i]];
		}
		return res_with_diags;
	} else {
		{
			auto w = res.write();
			for (int i = 0; i < C_ARR_SIZE(CubeIndices); i++)
				w[i] = scaled[CubeIndices[i]];
		}
		return res;
	}
}

PoolVector3Array GeometryGenerator::CreateSphereLines(int lats, int lons, float radius, Vector3 position) {
	if (lats < 2)
		lats = 2;
	if (lons < 4)
		lons = 4;

	PoolVector3Array res;
	res.resize(lats * lons * 6);

	{
		auto w = res.write();
		int total = 0;
		for (int i = 1; i <= lats; i++) {
			float lat0 = (float)Math_PI * (-0.5f + (float)(i - 1) / lats);
			float z0 = sin(lat0);
			float zr0 = cos(lat0);

			float lat1 = (float)Math_PI * (-0.5f + (float)i / lats);
			float z1 = sin(lat1);
			float zr1 = cos(lat1);

			for (int j = lons; j >= 1; j--) {
				float lng0 = (float)Math_TAU * (j - 1) / lons;
				float x0 = cos(lng0);
				float y0 = sin(lng0);

				float lng1 = (float)Math_TAU * j / lons;
				float x1 = cos(lng1);
				float y1 = sin(lng1);

				Vector3 v[4]{
					Vector3(x1 * zr0, z0, y1 * zr0) * radius + position,
					Vector3(x1 * zr1, z1, y1 * zr1) * radius + position,
					Vector3(x0 * zr1, z1, y0 * zr1) * radius + position,
					Vector3(x0 * zr0, z0, y0 * zr0) * radius + position
				};

				w[total++] = v[0];
				w[total++] = v[1];
				w[total++] = v[2];

				w[total++] = v[2];
				w[total++] = v[3];
				w[total++] = v[0];
			}
		}
	}
	return res;
}

PoolVector3Array GeometryGenerator::CreateCylinderLines(int edges, float radius, float height, Vector3 position, int drawEdgeEachNStep) {
	real_t angle = 360.f / edges;

	PoolVector3Array points;
	// points.reserve(size_t(4 * edges + ((edges / drawEdgeEachNStep) * 2)));

	// TODO will not be used... PoolVector3 does not support reserving anyway :(
	// TODO test and remove
	PRINT_WARNING("reserved " + String::num_int64((size_t)(4 * edges + ((edges / drawEdgeEachNStep) * 2))) + " points for cylinder");

	Vector3 d = Vector3(0, height * 0.5f, 0);
	for (int i = 0; i < edges; i++) {
		float ra = Math::deg2rad(i * angle);
		float rb = Math::deg2rad((i + 1) * angle);
		Vector3 a = Vector3(sin(ra), 0, cos(ra)) * radius + position;
		Vector3 b = Vector3(sin(rb), 0, cos(rb)) * radius + position;

		// Top
		points.push_back(a + d);
		points.push_back(b + d);

		// Bottom
		points.push_back(a - d);
		points.push_back(b - d);

		// Edge
		if (i % drawEdgeEachNStep == 0) {
			points.push_back(a + d);
			points.push_back(a - d);
		}
	}

	PRINT_WARNING("used " + String::num_int64(points.size()) + " points for cylinder");

	return points;
}

PoolVector3Array GeometryGenerator::CreateLinesFromPath(PoolVector3Array path) {
	PoolVector3Array res;
	res.resize((path.size() - 1) * 2);

	{
		auto w = res.write();
		for (int i = 1; i < path.size() - 1; i++) {
			w[(size_t)i * 2] = path[i];
			w[(size_t)i * 2 + 1] = path[i + 1];
		}
	}

	return res;
}