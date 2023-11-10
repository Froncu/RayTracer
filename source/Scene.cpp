#include "Scene.h"

#include "Utilities.hpp"
#include "Materials.hpp"

Scene::Scene(const std::string& sceneName, const Camera& camera, const Renderer::Settings& initialRendererSettings) :
	m_SceneName{ sceneName },

	m_Camera{ camera },
	m_vpMaterials{},
	m_vLights{},

	m_vSpheres{},
	m_vPlanes{},
	m_vTriangleMeshes{},

	m_InitialRendererSettings{ initialRendererSettings }
{
	m_vpMaterials.reserve(32);
	m_vLights.reserve(32);

	m_vSpheres.reserve(32);
	m_vPlanes.reserve(32);
	m_vTriangleMeshes.reserve(32);
}

Scene::~Scene()
{
	for (Material* pMaterial : m_vpMaterials)
		delete pMaterial;
}

void Scene::Update(const Timer& timer)
{
	m_Camera.Update(timer);
}

void Scene::GetClosestHit(const Ray& ray, HitRecord& closestHit) const
{
	for (const Sphere& sphere : m_vSpheres)
		HitTestSphere(sphere, ray, closestHit);

	for (const Plane& plane : m_vPlanes)
		HitTestPlane(plane, ray, closestHit);

	for (const TriangleMesh& triangleMesh : m_vTriangleMeshes)
		HitTestTriangleMesh(triangleMesh, ray, closestHit);
}

bool Scene::DoesHit(const Ray& ray) const
{
	for (const Sphere& sphere : m_vSpheres)
		if (HitTestSphere(sphere, ray))
			return true;

	for (const Plane& plane : m_vPlanes)
		if (HitTestPlane(plane, ray))
			return true;

	for (const TriangleMesh& triangleMesh : m_vTriangleMeshes)
		if (HitTestTriangleMesh(triangleMesh, ray))
			return true;

	return false;
}

unsigned char Scene::AddMaterial(Material* pMaterial)
{
	m_vpMaterials.push_back(pMaterial);
	return static_cast<unsigned char>(m_vpMaterials.size() - 1);
}

Light* const Scene::AddLight(const Light& light)
{
	m_vLights.emplace_back(light);
	return &m_vLights.back();
}

Sphere* const Scene::AddSphere(const Sphere& sphere)
{
	m_vSpheres.emplace_back(sphere);
	return &m_vSpheres.back();
}

Plane* const Scene::AddPlane(const Plane& plane)
{
	m_vPlanes.emplace_back(plane);
	return &m_vPlanes.back();
}

TriangleMesh* const Scene::AddTriangleMesh(const TriangleMesh& triangleMesh)
{
	m_vTriangleMeshes.emplace_back(triangleMesh);
	return &m_vTriangleMeshes.back();
}

SceneWeek1::SceneWeek1() :
	Scene("Week 1")
{
	const unsigned char matId_Solid_Red = AddMaterial(new SolidColorMaterial{ RED });
	const unsigned char matId_Solid_Blue = AddMaterial(new SolidColorMaterial{ BLUE });

	const unsigned char matId_Solid_Yellow = AddMaterial(new SolidColorMaterial{ YELLOW });
	const unsigned char matId_Solid_Green = AddMaterial(new SolidColorMaterial{ GREEN });
	const unsigned char matId_Solid_Magenta = AddMaterial(new SolidColorMaterial{ MAGENTA });

	AddPlane(Plane(Vector3(-75.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), matId_Solid_Green));
	AddPlane(Plane(Vector3(75.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), matId_Solid_Green));
	AddPlane(Plane(Vector3(0.0f, -75.0f, 0.0f), Vector3(0.0f, 1.0f, 0.f), matId_Solid_Yellow));
	AddPlane(Plane(Vector3(0.0f, 75.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), matId_Solid_Yellow));
	AddPlane(Plane(Vector3(0.0f, 0.0f, 125.0f), Vector3(0.0f, 0.0f, -1.0f), matId_Solid_Magenta));

	AddSphere(Sphere(Vector3(-25.0f, 0.0f, 100.0f), 50.0f, matId_Solid_Red));
	AddSphere(Sphere(Vector3(25.0f, 0.0f, 100.0f), 50.0f, matId_Solid_Blue));
}

SceneWeek2::SceneWeek2() :
	Scene("Week 2", Camera(Vector3(0.0f, 3.0f, -9.0f)))
{
	const unsigned char
		solidRed{ AddMaterial(new SolidColorMaterial{ RED }) },
		solidBlue{ AddMaterial(new SolidColorMaterial{ BLUE }) },
		solidYellow{ AddMaterial(new SolidColorMaterial{ YELLOW }) },
		solidGreen{ AddMaterial(new SolidColorMaterial{ GREEN }) },
		solidMagenta{ AddMaterial(new SolidColorMaterial{ MAGENTA }) };

	AddPlane(Plane(Vector3(-5.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), solidGreen));
	AddPlane(Plane(Vector3(5.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), solidGreen));
	AddPlane(Plane(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), solidYellow));
	AddPlane(Plane(Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), solidYellow));
	AddPlane(Plane(Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, -1.0f), solidMagenta));

	AddSphere(Sphere(Vector3(-1.75f, 1.0f, 0.0f), 0.75f, solidRed));
	AddSphere(Sphere(Vector3(0.0f, 1.0f, 0.0f), 0.75f, solidBlue));
	AddSphere(Sphere(Vector3(1.75f, 1.0f, 0.0f), 0.75f, solidRed));
	AddSphere(Sphere(Vector3(-1.75f, 3.0f, 0.0f), 0.75f, solidBlue));
	AddSphere(Sphere(Vector3(0.0f, 3.0f, 0.0f), 0.75f, solidRed));
	AddSphere(Sphere(Vector3(1.75f, 3.0f, 0.0f), 0.75f, solidBlue));

	AddLight(Light(Vector3(0.f, 5.f,-5.f), 70.f, WHITE));
}

SceneWeek3::SceneWeek3() :
	Scene("Week 3", Camera(Vector3(0.0f, 3.0f, -9.0f)))
{
	const unsigned char
		cookTorrenceGrayRoughMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, 1.f)) },
		cookTorrenceGrayMediumMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, .6f)) },
		cookTorrenceGraySmoothMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, .1f)) },
		cookTorrenceGrayRoughPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, 1.f)) },
		cookTorrenceGrayMediumPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, .6f)) },
		cookTorrenceGraySmoothPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, .1f)) },

		lambertGrayBlue{ AddMaterial(new LambertMaterial(ColorRGB(0.49f, 0.57f, 0.57f), 1.0f)) };

	AddPlane(Plane(Vector3(0.0f, 0.0f, 10.f), Vector3(0.0f, 0.0f, -1.0f), lambertGrayBlue)); //BACK
	AddPlane(Plane(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), lambertGrayBlue)); //BOTTOM
	AddPlane(Plane(Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), lambertGrayBlue)); //TOP
	AddPlane(Plane(Vector3(5.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), lambertGrayBlue)); //RIGHT
	AddPlane(Plane(Vector3(-5.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), lambertGrayBlue)); //LEFT

	AddSphere(Sphere(Vector3(-1.75f, 1.0f, 0.0f), 0.75f, cookTorrenceGrayRoughMetal));
	AddSphere(Sphere(Vector3(0.0f, 1.0f, 0.0f), 0.75f, cookTorrenceGrayMediumMetal));
	AddSphere(Sphere(Vector3(1.75f, 1.0f, 0.0f), 0.75f, cookTorrenceGraySmoothMetal));
	AddSphere(Sphere(Vector3(-1.75f, 3.0f, 0.0f), 0.75f, cookTorrenceGrayRoughPlastic));
	AddSphere(Sphere(Vector3(0.0f, 3.0f, 0.0f), 0.75f, cookTorrenceGrayMediumPlastic));
	AddSphere(Sphere(Vector3(1.75f, 3.0f, 0.0f), 0.75f, cookTorrenceGraySmoothPlastic));

	AddLight(Light(Vector3(0.0f, 5.0f, 5.0f), 50.0f, ColorRGB(1.0f, 0.61f, 0.45f))); //Backlight
	AddLight(Light(Vector3(-2.5f, 5.0f, -5.0f), 70.0f, ColorRGB(1.0f, 0.8f, 0.45f))); //Front Light Left
	AddLight(Light(Vector3(2.5f, 2.5f, -5.0f), 50.0f, ColorRGB(0.34f, 0.47f, 0.68f)));
}

SceneWeek4::SceneWeek4() :
	Scene("Week 4", Camera(Vector3(0.0f, 3.0f, -9.0f)))
{
	const unsigned char
		cookTorrenceGrayRoughMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, 1.f)) },
		cookTorrenceGrayMediumMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, .6f)) },
		cookTorrenceGraySmoothMetal{ AddMaterial(new CookTorrenceMaterial({ .972f, .960f, .915f }, 1.f, .1f)) },
		cookTorrenceGrayRoughPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, 1.f)) },
		cookTorrenceGrayMediumPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, .6f)) },
		cookTorrenceGraySmoothPlastic{ AddMaterial(new CookTorrenceMaterial({ .75f, .75f, .75f }, .0f, .1f)) },

		lambertGrayBlue{ AddMaterial(new LambertMaterial(ColorRGB(0.49f, 0.57f, 0.57f), 1.0f)) },
		lambertWhite{ AddMaterial(new LambertMaterial(WHITE, 1.0f)) };

	AddPlane(Plane(Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, -1.0f), lambertGrayBlue)); //BACK
	AddPlane(Plane(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), lambertGrayBlue)); //BOTTOM
	AddPlane(Plane(Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), lambertGrayBlue)); //TOP
	AddPlane(Plane(Vector3(5.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), lambertGrayBlue)); //RIGHT
	AddPlane(Plane(Vector3(-5.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), lambertGrayBlue)); //LEFT

	AddSphere(Sphere(Vector3(-1.75f, 1.0f, 0.0f), 0.75f, cookTorrenceGrayRoughMetal));
	AddSphere(Sphere(Vector3(0.0f, 1.0f, 0.0f), 0.75f, cookTorrenceGrayMediumMetal));
	AddSphere(Sphere(Vector3(1.75f, 1.0f, 0.0f), 0.75f, cookTorrenceGraySmoothMetal));
	AddSphere(Sphere(Vector3(-1.75f, 3.0f, 0.0f), 0.75f, cookTorrenceGrayRoughPlastic));
	AddSphere(Sphere(Vector3(0.0f, 3.0f, 0.0f), 0.75f, cookTorrenceGrayMediumPlastic));
	AddSphere(Sphere(Vector3(1.75f, 3.0f, 0.0f), 0.75f, cookTorrenceGraySmoothPlastic));

	const Triangle baseTriangle{ Vector3(-0.75f, 1.5f, 0.0f), Vector3(0.75f, 0.0f, 0.0f), Vector3(-0.75f, 0.0f, 0.0f), Vector3() };
	constexpr Vector3 offset{ 1.75f, 0.0f, 0.0f };
	for (int index{}; index < 3; ++index)
	{
		Triangle::CullMode cullMode;

		switch (index)
		{
		case 0:
			cullMode = Triangle::CullMode::backFace;
			break;

		case 1:
			cullMode = Triangle::CullMode::frontFace;
			break;

		default:
			cullMode = Triangle::CullMode::none;
			break;
		}

		TriangleMesh* const pTriangleMesh{ AddTriangleMesh(TriangleMesh(lambertWhite, cullMode)) };
		pTriangleMesh->AppendTriangle(baseTriangle);
		pTriangleMesh->SetTranslator(Vector3(-1.75f, 4.5f, 0.0f) + float(index) * offset);
		pTriangleMesh->UpdateTransforms();
		m_apTriangleMeshes[index] = pTriangleMesh;
	}

	AddLight(Light(Vector3(0.0f, 5.0f, 5.0f), 50.0f, ColorRGB(1.0f, 0.61f, 0.45f))); //Backlight
	AddLight(Light(Vector3(-2.5f, 5.0f, -5.0f), 70.0f, ColorRGB(1.0f, 0.8f, 0.45f))); //Front Light Left
	AddLight(Light(Vector3(2.5f, 2.5f, -5.0f), 50.0f, ColorRGB(0.34f, 0.47f, 0.68f)));
}

void SceneWeek4::Update(const Timer& timer)
{
	Scene::Update(timer);

	const float yawAngle{ (cos(timer.GetTotal()) + 1.0f) / 2.0f * DOUBLE_PI };

	for (TriangleMesh* const pTriangleMesh : m_apTriangleMeshes)
	{
		pTriangleMesh->SetRotorY(yawAngle);
		pTriangleMesh->UpdateTransforms();
	}
}

SceneWeek4Bunny::SceneWeek4Bunny() :
	Scene("Week 4: Bunny", Camera(Vector3(0.0f, 3.0f, -9.0f))),

	m_pBunnyTriangleMesh{}
{
	const unsigned char 
		lambertGrayBlue{ AddMaterial(new LambertMaterial(ColorRGB(0.49f, 0.57f, 0.57f), 1.0f)) },
		lambertWhite{ AddMaterial(new LambertMaterial(WHITE, 1.0f)) };

	AddPlane(Plane(Vector3(0.0f, 0.0f, 10.0f), Vector3(0.0f, 0.0f, -1.0f), lambertGrayBlue)); //BACK
	AddPlane(Plane(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), lambertGrayBlue)); //BOTTOM
	AddPlane(Plane(Vector3(0.0f, 10.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), lambertGrayBlue)); //TOP
	AddPlane(Plane(Vector3(5.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0), lambertGrayBlue)); //RIGHT
	AddPlane(Plane(Vector3(-5.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), lambertGrayBlue)); //LEFT

	m_pBunnyTriangleMesh = AddTriangleMesh(TriangleMesh("Resources/lowpoly_bunny.obj", lambertWhite));

	m_pBunnyTriangleMesh->SetScalar(2.0f);
	m_pBunnyTriangleMesh->UpdateTransforms();

	AddLight(Light(Vector3(0.0f, 5.0f, 5.0f), 50.0f, ColorRGB(1.0f, 0.61f, 0.45f))); //Backlight
	AddLight(Light(Vector3(-2.5f, 5.0f, -5.0f), 70.0f, ColorRGB(1.0f, 0.8f, 0.45f))); //Front Light Left
	AddLight(Light(Vector3(2.5f, 2.5f, -5.0f), 50.0f, ColorRGB(0.34f, 0.47f, 0.68f)));
}

void SceneWeek4Bunny::Update(const Timer& timer)
{
	Scene::Update(timer);

	const float yawAngle{ (cos(timer.GetTotal()) + 1.0f) / 2.0f * DOUBLE_PI };
	m_pBunnyTriangleMesh->SetRotorY(yawAngle);
	m_pBunnyTriangleMesh->UpdateTransforms();
}

SceneExtra::SceneExtra() :
	Scene("Extra Scene", Camera(Vector3(0.0f, 2.0f, -12.0f), TO_RADIANS * 70.0f), Renderer::Settings(Renderer::Settings::LightingMode::combined, false, true, 5))
{
	const unsigned char
		mirror{ AddMaterial(new CookTorrenceMaterial({ 1.0f,  1.0f,  1.0f }, 0.0f, 0.0f)) },
		smootherCyan{ AddMaterial(new CookTorrenceMaterial({ 0.0f,  1.0f,  1.0f }, 0.0f, 0.2f)) },
		smoothPink{ AddMaterial(new CookTorrenceMaterial({ 1.0f,  0.0f,  1.0f }, 0.0f, 0.3f)) },
		roughYellow{ AddMaterial(new CookTorrenceMaterial({ 1.0f,  1.0f,  0.0f }, 0.0f, 0.5f)) };

	AddSphere(Sphere(Vector3(0.0f, 5.0f, 15.0f), 10.0f, smootherCyan)); //BACK
	AddSphere(Sphere(Vector3(0.0f, -20.0f, 0.0f), 20.0f, smoothPink)); //BOTTOM
	AddSphere(Sphere(Vector3(0.0f, 30.0f, 0.0f), 20.0f, roughYellow)); //TOP
	AddPlane(Plane(Vector3(0.0f, 0.0f, -15.0f), Vector3(0.0f, 0.0f, 1.0f), mirror)); //FRONT
	AddPlane(Plane(Vector3(5.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), smootherCyan)); //RIGHT
	AddPlane(Plane(Vector3(-5.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), smootherCyan)); //LEFT

	TriangleMesh* const pTriangleMesh{ AddTriangleMesh(TriangleMesh("Resources/lowpoly_bunny.obj", mirror)) };
	pTriangleMesh->SetScalar(3.0f);
	pTriangleMesh->SetRotorY(TO_RADIANS * 45.0f);
	pTriangleMesh->UpdateTransforms();
	pTriangleMesh->ToggleDynamic();

	AddLight(Light(Vector3(2.5f, 7.5f, 5.0f), 50.0f, ColorRGB(1.0f, 1.0f, 1.0f)));
	AddLight(Light(Vector3(-2.5f, 5.0f, 0.0f), 50.0f, ColorRGB(1.0f, 1.0f, 1.0f)));
	AddLight(Light(Vector3(1.0f, 3.0f, -7.5f), 100.0f, ColorRGB(1.0f, 1.0f, 1.0f)));
}