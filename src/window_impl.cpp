#include "window_impl.hpp"

#include <iostream>
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


    /* ------------------------ FUNCTION DECLARATION ------------------------ */

    void on_window_resize(GLFWwindow* window, int width, int height);
    void mouse_position_callback(GLFWwindow* window, double x, double y);
    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    void char_callback(GLFWwindow* window, unsigned int key_code);
    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    std::string UnicodeToUTF8(unsigned int codepoint);

    /* ------------------------ PUBLIC IMPLEMENTATION ------------------------ */

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

    void Window::set_scale(float x, float y)
    {
        WindowImpl* impl = getImpl(this);
        impl->_x_scale = x;
        impl->_y_scale = y;
    }

    void Window::set_scale(float scalar)
    {
        WindowImpl* impl = getImpl(this);
        impl->_x_scale = scalar;
        impl->_y_scale = scalar;
    }

    void Window::set_offset(float x, float y)
    {
        WindowImpl* impl = getImpl(this);
        impl->_x_offset = x; // * impl->_dpi_scale;
        impl->_y_offset = y; // * impl->_dpi_scale;
    }

    void Window::add_resize_listener(std::function<void(resize_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_resize_callbacks.push_back(callback);
    }

    void Window::add_mouse_move_listener(std::function<void(mouse_move_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_mouse_move_callbacks.push_back(callback);
    }

    void Window::add_mouse_click_listener(std::function<void(mouse_button_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_mouse_button_callbacks.push_back(callback);
    }
    
    void Window::add_key_listener(std::function<void(key_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_key_callbacks.push_back(callback);
    }

    void Window::add_char_listener(std::function<void(char_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_char_callbacks.push_back(callback);
    }

    void Window::add_scroll_listener(std::function<void(scroll_event)> callback)
    {
        WindowImpl* impl = getImpl(this);
        impl->_scroll_callbacks.push_back(callback);
    }

    void Window::reset_listener()
    {
        WindowImpl* impl = getImpl(this);
        impl->_resize_callbacks.clear();
        impl->_mouse_move_callbacks.clear();
        impl->_mouse_button_callbacks.clear();
        impl->_key_callbacks.clear();
        impl->_scroll_callbacks.clear();
    }

    void Window::set_cursor(CURSOR_TYPE cursor_type)
    {
        WindowImpl* impl = getImpl(this);

        if (impl->_default_cursors.find(cursor_type) != impl->_default_cursors.end())
        {
            impl->_default_cursors[cursor_type] = glfwCreateStandardCursor(cursor_type);
        }

        glfwSetCursor(impl->_glfw_window, impl->_default_cursors[cursor_type]);
    }

    void Window::set_cursor(Cursor* cursor)
    {
        WindowImpl* impl = getImpl(this);
        if (cursor == nullptr) {
            glfwSetCursor(impl->_glfw_window, NULL);
            return;
        }

        if (impl->_cursors.find(cursor) == impl->_cursors.end())
        {         
            GLFWimage image;
            image.width = 16;
            image.height = 16;
            image.pixels = cursor->data;

            impl->_cursors[cursor] = glfwCreateCursor(&image, cursor->hot_x, cursor->hot_y);
        }

        glfwSetCursor(impl->_glfw_window, impl->_cursors[cursor]);
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

    void Window::restore()
    {
        WindowImpl* impl = getImpl(this);
        glfwRestoreWindow(impl->_glfw_window);
    }

    void Window::close()
    {
        WindowImpl* impl = getImpl(this);
        glfwSetWindowShouldClose(impl->_glfw_window, GLFW_TRUE);
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

    elemd::vec2 Window::get_position()
    {
        WindowImpl* impl = getImpl(this);
        int x, y;
        glfwGetWindowPos(impl->_glfw_window, &x, &y);
        return elemd::vec2(x, y);
    }

    bool Window::is_running()
    {
        WindowImpl* impl = getImpl(this);
        return !glfwWindowShouldClose(impl->_glfw_window);
    }

    elemd::vec2 Window::get_scale()
    {
        WindowImpl* impl = getImpl(this);
        return elemd::vec2(impl->_x_scale, impl->_y_scale);
    }

    elemd::vec2 Window::get_offset()
    {
        WindowImpl* impl = getImpl(this);
        return elemd::vec2(impl->_x_offset, impl->_y_offset);
    }

    float Window::get_dpi_scale()
    {
        WindowImpl* impl = getImpl(this);
        return impl->_dpi_scale;
    }
    

    double Window::now()
    {
        return glfwGetTime();
    }

    void Window::poll_events()
    {
        glfwPollEvents();
    }

    void Window::wait_events()
    {
        glfwWaitEvents();
    }

    void Window::wait_events(float time)
    {
        glfwWaitEventsTimeout(time);
    }

    void Window::trigger_events()
    {
        glfwPostEmptyEvent();
    }

    Context* Window::create_context()
    {
        WindowImpl* impl = getImpl(this);
        _context = Context::create(this);

        glfwSetWindowUserPointer(impl->_glfw_window, impl);
        glfwSetWindowSizeCallback(impl->_glfw_window, on_window_resize);
        glfwSetCursorPosCallback(impl->_glfw_window, mouse_position_callback);
        glfwSetMouseButtonCallback(impl->_glfw_window, mouse_button_callback);
        glfwSetKeyCallback(impl->_glfw_window, key_callback);
        glfwSetCharCallback(impl->_glfw_window, char_callback);
        glfwSetScrollCallback(impl->_glfw_window, scroll_callback);

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
        glfwWindowHint(GLFW_SCALE_TO_MONITOR, GLFW_TRUE);


        create_window(config);
    }

    WindowImpl::~WindowImpl()
    {
        glfwDestroyWindow(_glfw_window);
        --_windowCount;

        _context->destroy();

        for (auto& cursor : _default_cursors)
        {
            glfwDestroyCursor(cursor.second);
        }
        for (auto& cursor : _cursors)
        {
            glfwDestroyCursor(cursor.second);
        }

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
            glfwCreateWindow(config.width, config.height, config.title, NULL, NULL);
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
        
        glfwGetWindowContentScale(_glfw_window, &_dpi_scale, &_dpi_scale);  
        
        //_x_scale = config.x_scale;
        //_y_scale = config.y_scale;        
    }

    void WindowImpl::load_icon(const WindowConfig& config)
    {
        if (config.icon_file == nullptr)
            return;

        GLFWimage icon[1];
        int numComponents;
        icon[0].pixels = stbi_load(config.icon_file, &icon[0].width, &icon[0].height,
                                   &numComponents, 4);

        if (icon[0].pixels == NULL)
        {
            std::cerr << "Error: Unable to load Icon: " << config.icon_file << "\n";
            return;
        }

        glfwSetWindowIcon(_glfw_window, 1, icon);
        stbi_image_free(icon[0].pixels);
    }









    /* ------------------------ EVENTS ------------------------ */

    void on_window_resize(GLFWwindow* window, int width, int height)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);
        winImpl->get_context()->resize_context(width, height);

        std::cout << "event: resize "
                  << "width: " << width << " height: " << height << std::endl;

        for (auto& var : winImpl->_resize_callbacks)
        {
            var({width, height});
        }
    }
    
    void mouse_position_callback(GLFWwindow* window, double x, double y)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);

        /*
        if (winImpl->buttonEvent == 1)
        {
            winImpl->offset_cpx = x - winImpl->cp_x;
            winImpl->offset_cpy = y - winImpl->cp_y;
        }
        */

        std::cout << "event: mouse_position " << "x: " << x << " y: " << y << std::endl;

        for (auto& var : winImpl->_mouse_move_callbacks)
        {
            var({x / winImpl->_dpi_scale, y / winImpl->_dpi_scale});
        }
    }

    void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);

        /*
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS)
        {
            winImpl->buttonEvent = 1;
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            winImpl->cp_x = floor(x);
            winImpl->cp_y = floor(y);
        }
        if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE)
        {
            winImpl->buttonEvent = 0;
            winImpl->cp_x = 0;
            winImpl->cp_y = 0;
        }
        */
        std::cout << "event: mouse_button "
                  << "button: " << button << " action: " << action << " mods: " << mods
                  << std::endl;

        double x, y;
        glfwGetCursorPos(window, &x, &y);

        for (auto& var : winImpl->_mouse_button_callbacks)
        {
            var({(mouse_button)button, (input_action)action, (mouse_mod)mods,
                 x / winImpl->_dpi_scale, y / winImpl->_dpi_scale});
        }
    }

    void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);
        const char* key_code = glfwGetKeyName(key, scancode);

        std::string key_name = (key_code == NULL) ? " " : key_code;

        std::cout << "event: key "
                  << "key: " << key << " scancode: " << scancode << " action: " << action
                  << " mods: " << mods << " key name: " << key_name << std::endl;

        for (auto& var : winImpl->_key_callbacks)
        {
            var({(keyboard_key)key, scancode, (input_action)action, (keyboard_mod)mods, key_code});
        }
    }

    void char_callback(GLFWwindow* window, unsigned int key_code)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);

        std::string utf8 = UnicodeToUTF8(key_code);
        std::cout << "event: char "
                  << "char: " << utf8 << std::endl;

        for (auto& var : winImpl->_char_callbacks)
        {
            var({utf8.c_str()});
        }
    }

    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
    {
        WindowImpl* winImpl = (WindowImpl*)glfwGetWindowUserPointer(window);

        std::cout << "event: scroll "
                  << "xoffset: " << xoffset << " yoffset: " << yoffset << std::endl;

        for (auto& var : winImpl->_scroll_callbacks)
        {
            var({xoffset, yoffset});
        }
    }





    std::string UnicodeToUTF8(unsigned int codepoint)
    {
        std::string out;

        if (codepoint <= 0x7f)
            out.append(1, static_cast<char>(codepoint));
        else if (codepoint <= 0x7ff)
        {
            out.append(1, static_cast<char>(0xc0 | ((codepoint >> 6) & 0x1f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else if (codepoint <= 0xffff)
        {
            out.append(1, static_cast<char>(0xe0 | ((codepoint >> 12) & 0x0f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        else
        {
            out.append(1, static_cast<char>(0xf0 | ((codepoint >> 18) & 0x07)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 12) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | ((codepoint >> 6) & 0x3f)));
            out.append(1, static_cast<char>(0x80 | (codepoint & 0x3f)));
        }
        return out;
    }

} // namespace elemd