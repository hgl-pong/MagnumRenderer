#pragma once
#include <Magnum/Magnum.h>
#include <Magnum/Image.h>
#include <Magnum/GL/Buffer.h>
#include <Magnum/GL/DefaultFramebuffer.h>
#include <Magnum/GL/Mesh.h>
#include <Magnum/GL/PixelFormat.h>
#include <Magnum/GL/Renderer.h>
#include <Magnum/Math/Color.h>
#include <Magnum/Math/FunctionsBatch.h>
#include <Magnum/MeshTools/Compile.h>
#include <Magnum/Platform/Sdl2Application.h>
#include <Magnum/Primitives/Grid.h>
#include <Magnum/SceneGraph/Camera.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Magnum/SceneGraph/MatrixTransformation3D.h>
#include <Magnum/SceneGraph/Object.h>
#include <Magnum/SceneGraph/Scene.h>
#include <Magnum/Shaders/Flat.h>
#include <Magnum/Shaders/Phong.h>
#include <Magnum/Shaders/VertexColor.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/Math/Matrix4.h>
#include <Magnum/Math/Quaternion.h>
#include <Magnum/Math/Matrix3.h>
#include <Magnum/Primitives/Cylinder.h>
#include <Magnum/Primitives/Cube.h>
#include <Magnum/Primitives/UVSphere.h>
#include <Magnum/Primitives/Plane.h>
#include <Magnum/Trade/MeshData.h>
#include <Magnum/SceneGraph/Drawable.h>
#include <Math/GraphicUtils/Camara.h>
#include <Math/GraphicUtils/MeshData.h>
#include "MagnumRender/MagnumConvertUtils.h"
namespace MagnumRender
{
	static MathLib::HVector3 mLightPosition(7.0f, 5.0f, 2.5f);
	static Magnum::Color4 mLightColor(0.5f, 0.5f, 0.5f, 1.0f);
	using Object3D = Magnum::SceneGraph::Object<Magnum::SceneGraph::MatrixTransformation3D>;
	using Scene3D = Magnum::SceneGraph::Scene<Magnum::SceneGraph::MatrixTransformation3D>;
	using namespace Magnum::Math::Literals;

	class ShaderTable
	{
	public:
		ShaderTable()
		{
		}

		void Init()
		{
			if (m_bIsInitialized)
				return;
			m_FlatShader = Magnum::Shaders::Flat3D{};
			m_PhongShader = Magnum::Shaders::Phong{};
			m_VertexColorShader = Magnum::Shaders::VertexColor3D{};
			m_bIsInitialized = true;
		}

		Magnum::Shaders::Flat3D &GetFlatShader()
		{
			Init();
			return m_FlatShader;
		}

		Magnum::Shaders::Phong &GetPhongShader()
		{
			Init();
			return m_PhongShader;
		}

		Magnum::Shaders::VertexColor3D &GetVertexColorShader()
		{
			Init();
			return m_VertexColorShader;
		}

	private:
		bool m_bIsInitialized = false;
		Magnum::Shaders::Flat3D m_FlatShader{Magnum::NoCreate};
		Magnum::Shaders::Phong m_PhongShader{Magnum::NoCreate};
		Magnum::Shaders::VertexColor3D m_VertexColorShader{Magnum::NoCreate};
	};

	static ShaderTable m_ShaderTable;

	class RenderUnitBase
	{
	public:
		explicit RenderUnitBase(const MathLib::GraphicUtils::MeshData &meshData)
		{
			m_Mesh = Magnum::MeshTools::compile(CreateMesh(meshData));
		}

		virtual void UpdateTransformation()
		{
			Object3D *parent = m_Object.parent();
			if (parent == nullptr)
				return;
			auto t = parent->absoluteTransformationMatrix();
			m_TransformationMatrix = m_Object.absoluteTransformationMatrix();
		};
		virtual void ResetTransformation()
		{
			m_Object.resetTransformation();
			UpdateTransformation();
		};
		virtual void Render(MathLib::GraphicUtils::Camera &camera) {}
		virtual void Show(bool show)
		{
			m_bShow = show;
		}
		virtual void SetMeshData(const MathLib::GraphicUtils::MeshData &meshdata) { m_Mesh = Magnum::MeshTools::compile(CreateMesh(meshdata)); }
		virtual void AddToScene(Object3D &scene)
		{
			m_Object.setParent(&scene);
			UpdateTransformation();
		}
		virtual void RemoveFromScene()
		{
			m_Object.setParent(nullptr);
		}
		virtual void SetTransformation(const MathLib::HMatrix4 &transform)
		{
			m_Object.setTransformation(ToMagnum(transform));
			UpdateTransformation();
		}

		Object3D &GetObject()
		{
			return m_Object;
		}

		void SetTransformation(const MathLib::HVector3 *scaling = nullptr, const MathLib::HVector3 *translation = nullptr, const MathLib::HQuaternion *rotation = nullptr)
		{
			Magnum::Matrix4 currentTransform = m_Object.transformationMatrix();
			Magnum::Matrix4 scalingMatrix = scaling == nullptr ? Magnum::Matrix4::scaling(currentTransform.scaling()) : Magnum::Matrix4::scaling(ToMagnum(*scaling));
			Magnum::Matrix4 rotationMatrix = rotation == nullptr ? Magnum::Matrix4::from(currentTransform.rotation(), {}) : Magnum::Matrix4::from(ToMagnum(*rotation).toMatrix(), {});
			Magnum::Vector3 translationVector = translation == nullptr ? currentTransform.translation() : ToMagnum(*translation);

			m_Object.resetTransformation();
			m_Object.setTransformation(scalingMatrix * rotationMatrix);
			m_Object.translate(translationVector);

			UpdateTransformation();
		}

	protected:
		bool m_bShow = true;
		Magnum::GL::Mesh m_Mesh;
		Object3D m_Object;
		Magnum::Matrix4 m_TransformationMatrix;
	};

	class GizmoRenderUnit : public RenderUnitBase
	{
	public:
		explicit GizmoRenderUnit(const MathLib::GraphicUtils::MeshData& meshData)
			: RenderUnitBase(meshData)
		{
			m_Mesh.setPrimitive(Magnum::MeshPrimitive::Lines);
		}

		void Render(MathLib::GraphicUtils::Camera &camera) override
		{
			if (m_bShow)
			{
				m_ShaderTable.GetFlatShader()
					.setColor(m_Color)
					.setTransformationProjectionMatrix(ToMagnum(camera.GetViewProjectMatrix()) * m_TransformationMatrix)
					.draw(m_Mesh);
			}
		}

		void SetColor(const Magnum::Color4 &color)
		{
			m_Color = color;
		}

	private:
		Magnum::Color4 m_Color = 0x747474_rgbf;
	};

	class SimpleRenderUnit : public RenderUnitBase
	{
	public:
		explicit SimpleRenderUnit(const MathLib::GraphicUtils::MeshData &meshData)
			: RenderUnitBase(meshData)
		{
			MathLib::GraphicUtils::MeshData wireframeMeshData;
			wireframeMeshData.m_Vertices = meshData.m_Vertices;
			wireframeMeshData.m_Indices = MathLib::GraphicUtils::TrianglesToLines(meshData.m_Indices);
			m_WireFrameMesh= Magnum::MeshTools::compile(CreateMesh(wireframeMeshData));
			m_WireFrameMesh.setPrimitive(Magnum::MeshPrimitive::Lines);
			m_Mesh.setPrimitive(Magnum::MeshPrimitive::Triangles);
		}

		void Render(MathLib::GraphicUtils::Camera &camera) override
		{
			if (!m_bShow)
				return;
			const Magnum::Matrix4 &transformationMatrix = ToMagnum(camera.GetViewMatrix()) * m_TransformationMatrix;

			m_ShaderTable.GetPhongShader().setLightPosition(ToMagnum(mLightPosition)).setAmbientColor(m_AmbientColor).setDiffuseColor(m_DiffuseColor).setLightColor(mLightColor).setTransformationMatrix(transformationMatrix).setNormalMatrix(transformationMatrix.normalMatrix()).setProjectionMatrix(ToMagnum(camera.GetProjectMatrix()));
			m_ShaderTable.GetPhongShader().draw(m_Mesh);
			if (m_bShowWireframe)	
			{
				m_ShaderTable.GetFlatShader().setColor(0x000000_rgbf).setTransformationProjectionMatrix(ToMagnum(camera.GetProjectMatrix()) * transformationMatrix).draw(m_WireFrameMesh);
			}
		}

		void SetAmbientColor(const Magnum::Color4 &color)
		{
			m_AmbientColor = color;
		}

		Magnum::Color4 GetAmbientColor() const
		{
			return m_AmbientColor;
		}

		void SetDiffuseColor(const Magnum::Color4 &color)
		{
			m_DiffuseColor = color;
		}

		Magnum::Color4 GetDiffuseColor() const
		{
			return m_DiffuseColor;
		}

		void ShowWireframe(bool show)
		{
			m_bShowWireframe = show;
		}

	private:
		bool m_bShowWireframe = true;
		Magnum::GL::Mesh m_WireFrameMesh;
		Magnum::Color4 m_AmbientColor = {1.f, 1.f, 1.f, 1.f};
		Magnum::Color4 m_DiffuseColor = {1.f, 1.f, 1.f, 1.f};
	};

	class CoordinateAxis : public RenderUnitBase
	{
	private:
		struct Vertex
		{
			Magnum::Vector3 position;
			Magnum::Color3 color;
		};
	public:
		explicit CoordinateAxis(const MathLib::HReal length = 60.f)
			: RenderUnitBase(MathLib::GraphicUtils::GenerateBoxMeshData(MathLib::HVector3(1, 1, 1)))
		{
			std::vector<Vertex> vertices;
			std::vector<unsigned int> indices;

			// X轴 (红色)
			_AddArrow(vertices, indices, { 0.0f, 0.0f, 0.0f }, { length, 0.0f, 0.0f }, 0xff0000_rgbf, 0.1f);
			// Y轴 (绿色)
			_AddArrow(vertices, indices, { 0.0f, 0.0f, 0.0f }, { 0.0f, length, 0.0f }, 0x00ff00_rgbf, 0.1f);
			// Z轴 (蓝色)
			_AddArrow(vertices, indices, { 0.0f, 0.0f, 0.0f }, { 0.0f, 0.0f, length }, 0x0000ff_rgbf, 0.1f);

			m_VertexBuffer.setData(vertices);
			m_IndexBuffer.setData(indices);

			m_Mesh = Magnum::GL::Mesh{};
			m_Mesh.setPrimitive(Magnum::GL::MeshPrimitive::Triangles)
				.setCount(indices.size())
				.addVertexBuffer(std::move(m_VertexBuffer), 0, Magnum::Shaders::VertexColor3D::Position{}, Magnum::Shaders::VertexColor3D::Color3{})
				.setIndexBuffer(std::move(m_IndexBuffer), 0, Magnum::GL::MeshIndexType::UnsignedInt);
		}

		void Render(MathLib::GraphicUtils::Camera &camera) override
		{
			if (m_bShow) {
				Magnum::Matrix4 viewProjectionMatrix = ToMagnum(camera.GetViewProjectMatrix());
				Magnum::Matrix4 screenTransform = Magnum::Matrix4::translation({ -0.9f, -0.9f, 0.0f }) *
					Magnum::Matrix4::scaling({ 0.1f, 0.1f, 0.1f });

				m_ShaderTable.GetVertexColorShader().setTransformationProjectionMatrix(viewProjectionMatrix * m_TransformationMatrix);
				m_ShaderTable.GetVertexColorShader().draw(m_Mesh);
			}
		}
	private:
		void _AddArrow(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
			const Magnum::Vector3& start, const Magnum::Vector3& end, const Magnum::Color3& color,const MathLib::HReal radius =1.f) {
			// 添加圆柱体部分
			unsigned int startIndex = vertices.size();
			_AddCylinder(vertices, indices, start, end - (end - start).normalized(), color, radius);

			// 计算箭头底部的中心位置
			Magnum::Vector3 direction = (end - start).normalized();
			Magnum::Vector3 arrowBase = end - direction;

			// 添加圆锥体部分
			_AddCone(vertices, indices, arrowBase, end, color, radius*5);
		}

		void _AddCylinder(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
			const Magnum::Vector3& start, const Magnum::Vector3& end, const Magnum::Color3& color, float radius) {
			const int numSegments = 10;
			unsigned int baseIndex = vertices.size();

			Magnum::Vector3 direction = (end - start).normalized();
			Magnum::Vector3 up = direction.z() != 0.0f ? Magnum::Vector3{ 1.0f, 0.0f, 0.0f } : Magnum::Vector3{ 0.0f, 0.0f, 1.0f };
			Magnum::Vector3 right = Magnum::Math::cross(direction, up).normalized();
			up = Magnum::Math::cross(right, direction).normalized();

			for (int i = 0; i < numSegments; ++i) {
				float angle = 2.0f * Magnum::Math::Constants<float>::pi() * i / numSegments;
				float x = std::cos(angle) * radius;
				float y = std::sin(angle) * radius;
				vertices.push_back({ start + right * x + up * y, color });
				vertices.push_back({ end + right * x + up * y, color });
			}

			for (int i = 0; i < numSegments; ++i) {
				int next = (i + 1) % numSegments;
				indices.push_back(baseIndex + i * 2);
				indices.push_back(baseIndex + next * 2);
				indices.push_back(baseIndex + i * 2 + 1);

				indices.push_back(baseIndex + next * 2);
				indices.push_back(baseIndex + next * 2 + 1);
				indices.push_back(baseIndex + i * 2 + 1);
			}
		}

		void _AddCone(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices,
			const Magnum::Vector3& base, const Magnum::Vector3& tip, const Magnum::Color3& color, float radius) {
			const int numSegments = 10;
			unsigned int baseIndex = vertices.size();

			Magnum::Vector3 direction = (tip - base).normalized();
			Magnum::Vector3 up = direction.z() != 0.0f ? Magnum::Vector3{ 1.0f, 0.0f, 0.0f } : Magnum::Vector3{ 0.0f, 0.0f, 1.0f };
			Magnum::Vector3 right = Magnum::Math::cross(direction, up).normalized();
			up = Magnum::Math::cross(right, direction).normalized();

			// 生成圆锥底部顶点
			for (int i = 0; i < numSegments; ++i) {
				float angle = 2.0f * Magnum::Math::Constants<float>::pi() * i / numSegments;
				float x = std::cos(angle) * radius;
				float y = std::sin(angle) * radius;
				vertices.push_back({ base + right * x + up * y, color });
			}
			vertices.push_back({ tip, color }); // 箭头尖端
			unsigned int tipIndex = vertices.size() - 1;

			// 生成圆锥侧面索引
			for (int i = 0; i < numSegments; ++i) {
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + (i + 1) % numSegments);
				indices.push_back(tipIndex);
			}

			// 生成圆锥底部索引
			unsigned int centerIndex = vertices.size();
			vertices.push_back({ base, color });
			for (int i = 0; i < numSegments; ++i) {
				indices.push_back(centerIndex);
				indices.push_back(baseIndex + i);
				indices.push_back(baseIndex + (i + 1) % numSegments);
			}
		}
	private:
		Magnum::GL::Buffer m_VertexBuffer;
		Magnum::GL::Buffer m_IndexBuffer;
	};
}