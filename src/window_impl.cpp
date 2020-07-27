#include "window_impl.hpp"

#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "elemd/context.hpp"
#include "vulkan/vulkan_shared_info.hpp"

namespace elemd
{

    /* ------------------------ DOWNCAST ------------------------ */

    inline WindowImpl* getImpl(Window* ptr)
    {
        return (WindowImpl*)ptr;
    }
    inline const WindowImpl* getImpl(const Window* ptr)
    {
        return (const WindowImpl*)ptr;
    }

    /* ------------------------ PUBLIC IMPLEMENTATION ------------------------ */

    void on_window_resize(GLFWwindow* window, int width, int height);

    Window* Window::create(WindowConfig config)
    {
        return new WindowImpl(config);
    }

    void Window::set_title(const std::string& title)
    {
        WindowImpl* impl = getImpl(this);
        glfwSetWindowTitle(impl->_glfw_window, title.c_str());
    }

    void Window::set_position(int x, int y)
    {
        WindowImpl* impl = getImpl(this);
        glfwSetWindowPos(impl->_glfw_window, x, y);
    }

    void Window::set_size(int width, int height)
    {
        WindowImpl* impl = getImpl(this);
        glfwSetWindowSize(impl->_glfw_window, width, height);
    }

    void Window::set_vsync(bool vsync)
    {
        WindowImpl* impl = getImpl(this);
        _vsync = vsync;
        _context->resize_context(impl->get_width(), impl->get_height());
    }

    void Window::minimize()
    {
        WindowImpl* impl = getImpl(this);
        glfwIconifyWindow(impl->_glfw_window);
    }

    void Window::maximize()
    {
        WindowImpl* impl = getImpl(this);
        glfwMaximizeWindow(impl->_glfw_window);
    }

    bool Window::get_vsync()
    {
        return _vsync;
    }

    void Window::destroy()
    {
        delete this;
    }

    int Window::get_width()
    {
        WindowImpl* impl = getImpl(this);
        int w, h;
        glfwGetWindowSize(impl->_glfw_window, &w, &h);
        return w;
    }

    int Window::get_height()
    {
        WindowImpl* impl = getImpl(this);
        int w, h;
        glfwGetWindowSize(impl->_glfw_window, &w, &h);
        return h;
    }

    bool Window::is_running()
    {
        WindowImpl* impl = getImpl(this);
        return !glfwWindowShouldClose(impl->_glfw_window);
    }

    double Window::now()
    {
        return glfwGetTime();
    }

    void Window::poll_events()
    {
        glfwPollEvents();
    }

    Context* Window::create_context()
    {
        WindowImpl* impl = getImpl(this);
        _context = Context::create(this);

        glfwSetWindowUserPointer(impl->_glfw_window, _context);
        glfwSetWindowSizeCallback(impl->_glfw_window, on_window_resize);

        return _context;
    }

    Context* Window::get_context()
    {
        return _context;
    }

    /* ------------------------ PRIVATE IMPLEMENTATION ------------------------ */

    WindowImpl::WindowImpl(const WindowConfig& config)
    {
        if (_windowCount == 0)
        {
            if (!glfwInit())
            {
                std::cerr << "Failed to initialize GLFW!" << std::endl;
                exit(EXIT_FAILURE);
            }
        }

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_DECORATED, config.decorated);
        glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, config.transparent);
        glfwWindowHint(GLFW_RESIZABLE, config.resizeable);
        glfwWindowHint(GLFW_VISIBLE, config.visible);

        create_window(config);
    }

    WindowImpl::~WindowImpl()
    {
        glfwDestroyWindow(_glfw_window);
        --_windowCount;

        _context->destroy();

        if (_windowCount == 0)
        {
            VulkanSharedInfo::destroy();
            glfwTerminate();
        }
    }

    GLFWwindow* WindowImpl::getGLFWWindow()
    {
        return _glfw_window;
    }

    void WindowImpl::create_window(const WindowConfig& config)
    {
        if (config.width == 0 || config.height == 0)
        {
            std::cerr << "Width or height cannont be 0!" << std::endl;
            exit(EXIT_FAILURE);
        }

        _glfw_window =
            glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
        if (!_glfw_window)
        {
            std::cerr << "Failed to create a window!" << std::endl;
            glfwTerminate();
            exit(EXIT_FAILURE);
        }
        ++_windowCount;

        _vsync = config.vsync;

        load_icon(config);

        if (config.position_x != -1 && config.position_y != -1)
        {
            set_position(config.position_x, config.position_y);
        }
    }

    void WindowImpl::load_icon(const WindowConfig& config)
    {
        GLFWimage icon[1];
        int numComponents;
        icon[0].pixels = stbi_load((config.icon_file).c_str(), &icon[0].width, &icon[0].height,
                                   &numComponents, 4);

        if (icon[0].pixels == NULL)
        {
            std::cerr << "Error: Unable to load Icon: " << config.icon_file << "\n";

            // Fallback to default
            if (config.icon_file.compare(ELEMD_ICON) != 0)
            {
                icon[0].pixels = stbi_load((ELEMD_ICON).c_str(), &icon[0].width, &icon[0].height,
                                           &numComponents, 4);
            }
        }

        glfwSetWindowIcon(_glfw_window, 1, icon);
        stbi_image_free(icon[0].pixels);
    }

    void on_window_resize(GLFWwindow* window, int width, int height)
    {
        if (width <= 0 || height <= 0)
            return;

        Context* context = (Context*)glfwGetWindowUserPointer(window);
        context->resize_context(width, height);
    }

} // namespace elemd