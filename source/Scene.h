#pragma once

#include "Camera.h"
#include "DataTypes.hpp"

class Material;

class Scene
{
public:
	Scene(const std::string& sceneName, const Camera& camera);
	virtual ~Scene();

	Scene(const Scene&) = delete;
	Scene(Scene&&) noexcept = delete;
	Scene& operator=(const Scene&) = delete;
	Scene& operator=(Scene&&) noexcept = delete;

	virtual void Update(const Timer& timer);
	void GetClosestHit(const Ray& ray, HitRecord& closestHit) const;
	bool DoesHit(const Ray& ray) const;

	inline const Camera& GetCamera() const
	{
		return m_Camera;
	}

	inline Camera& GetCamera()
	{
		return m_Camera;
	}

	inline const std::vector<Material*>& GetMaterials() const
	{
		return m_vpMaterials;
	}

	inline const std::vector<Light>& GetLights() const
	{
		return m_vLights;
	}

	inline const std::vector<Sphere>& GetSpheres() const
	{
		return m_vSpheres;
	}

	inline const std::vector<Plane>& GetPlanes() const 
	{ 
		return m_vPlanes; 
	}

	inline const std::vector<TriangleMesh>& GetTriangleMeshes() const
	{ 
		return m_vTriangleMeshes;
	}

protected:
	unsigned char AddMaterial(Material* pMaterial);
	Light* const AddLight(const Light& light);

	Sphere* const AddSphere(const Sphere& sphere);
	Plane* const AddPlane(const Plane& plane);
	TriangleMesh* const AddTriangleMesh(const TriangleMesh& triangleMesh);

	std::vector<TriangleMesh> m_vTriangleMeshes;

private:
	std::string	m_SceneName;

	Camera m_Camera;
	std::vector<Material*> m_vpMaterials;
	std::vector<Light> m_vLights;

	std::vector<Sphere> m_vSpheres;
	std::vector<Plane> m_vPlanes;
	//std::vector<TriangleMesh> m_vTriangleMeshes;
};

class SceneWeek1 final : public Scene
{
public:
	SceneWeek1();
	virtual ~SceneWeek1() override = default;

	SceneWeek1(const SceneWeek1&) = delete;
	SceneWeek1(SceneWeek1&&) noexcept = delete;
	SceneWeek1& operator=(const SceneWeek1&) = delete;
	SceneWeek1& operator=(SceneWeek1&&) noexcept = delete;
};

class SceneWeek2 final : public Scene
{
public:
	SceneWeek2();
	virtual ~SceneWeek2() override = default;

	SceneWeek2(const SceneWeek2&) = delete;
	SceneWeek2(SceneWeek2&&) noexcept = delete;
	SceneWeek2& operator=(const SceneWeek2&) = delete;
	SceneWeek2& operator=(SceneWeek2&&) noexcept = delete;
};

class SceneWeek3 final : public Scene
{
public:
	SceneWeek3();
	virtual ~SceneWeek3() override = default;

	SceneWeek3(const SceneWeek3&) = delete;
	SceneWeek3(SceneWeek3&&) noexcept = delete;
	SceneWeek3& operator=(const SceneWeek3&) = delete;
	SceneWeek3& operator=(SceneWeek3&&) noexcept = delete;
};

class SceneWeek4 final : public Scene
{
public:
	SceneWeek4();
	virtual ~SceneWeek4() override = default;

	SceneWeek4(const SceneWeek4&) = delete;
	SceneWeek4(SceneWeek4&&) noexcept = delete;
	SceneWeek4& operator=(const SceneWeek4&) = delete;
	SceneWeek4& operator=(SceneWeek4&&) noexcept = delete;

	virtual void Update(const Timer& timer) override;

//private:
	//TriangleMesh* m_apTriangleMeshes[3];
};

class SceneWeek4Bunny final : public Scene
{
public:
	SceneWeek4Bunny();
	virtual ~SceneWeek4Bunny() override = default;

	SceneWeek4Bunny(const SceneWeek4Bunny&) = delete;
	SceneWeek4Bunny(SceneWeek4Bunny&&) noexcept = delete;
	SceneWeek4Bunny& operator=(const SceneWeek4Bunny&) = delete;
	SceneWeek4Bunny& operator=(SceneWeek4Bunny&&) noexcept = delete;

	virtual void Update(const Timer& timer) override;

private:
	TriangleMesh* m_pBunnyTriangleMesh;
};