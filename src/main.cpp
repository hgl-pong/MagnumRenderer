#include "MagnumRender/MagnumRenderCommon.h"
template <typename T>
struct RenderDeleter
{
	void operator()(T* ptr) const
	{
		if (ptr)
		{
			ptr->Release();
			ptr = nullptr;
		}
	}
};

template <typename T>
using RenderPtr = std::shared_ptr<T>;

template <typename T>
RenderPtr<T> make_render_ptr(T* ptr)
{
	return RenderPtr<T>(ptr, RenderDeleter<T>());
}

class TestingApplication 
{
public:
	explicit TestingApplication(int argc, char** argv);

public:
	void Release()
	{
		delete this;
	}
	int Run()
	{
		while (true)
		{
			m_Renderer->Tick();
		}
		return 1;
	}

private:
	void _KeyPressEvent(Magnum::Platform::Sdl2Application::KeyEvent& event);
	void _KeyReleaseEvent(Magnum::Platform::Sdl2Application::KeyEvent& event);
	void _MousePressEvent(Magnum::Platform::Sdl2Application::MouseEvent& event);
	void _MouseMoveEvent(Magnum::Platform::Sdl2Application::MouseMoveEvent& event);
	void _MouseScrollEvent(Magnum::Platform::Sdl2Application::MouseScrollEvent& event);

private:
	RenderPtr<MagnumRender::Renderer> m_Renderer;
};

TestingApplication::TestingApplication(int argc, char** argv)
{
	m_Renderer = make_render_ptr(MagnumRender::CreateRenderer(argc, argv));
	m_Renderer->SetApplicationName("Testing Application");
	m_Renderer->SetUp(std::bind(&TestingApplication::_MousePressEvent, this, std::placeholders::_1),
		std::bind(&TestingApplication::_MousePressEvent, this, std::placeholders::_1),
		std::bind(&TestingApplication::_MouseMoveEvent, this, std::placeholders::_1),
		std::bind(&TestingApplication::_MouseScrollEvent, this, std::placeholders::_1),
		std::bind(&TestingApplication::_KeyPressEvent, this, std::placeholders::_1),
		std::bind(&TestingApplication::_KeyReleaseEvent, this, std::placeholders::_1));
}

void TestingApplication::_KeyPressEvent(Magnum::Platform::Sdl2Application::KeyEvent& event)
{
}

void TestingApplication::_KeyReleaseEvent(Magnum::Platform::Sdl2Application::KeyEvent& event)
{
}

void TestingApplication::_MousePressEvent(Magnum::Platform::Sdl2Application::MouseEvent& event)
{
}

void TestingApplication::_MouseMoveEvent(Magnum::Platform::Sdl2Application::MouseMoveEvent& event)
{
}

void TestingApplication::_MouseScrollEvent(Magnum::Platform::Sdl2Application::MouseScrollEvent& event)
{
}

int main(int argc, char** argv)
{
	RenderPtr<TestingApplication> app = make_render_ptr(new TestingApplication(argc, argv));
	return app->Run();
}