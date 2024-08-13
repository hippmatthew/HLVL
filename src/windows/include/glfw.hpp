#ifndef physp_windows_glfw_hpp
#define physp_windows_glfw_hpp

#include "src/core/include/iwindow.hpp"

#ifndef physp_glfw_include
#define physp_glfw_include

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#endif // physp_glfw_include

namespace pp::windows
{

class GLFW : public IWindow
{
  public:
    GLFW();
    ~GLFW() override;

    static std::vector<const char *> instance_extensions();

    bool should_close() const override;
    void poll_events() const override;

    void create_surface(const vk::raii::Instance&) override;

  private:
    GLFWwindow *  gl_window = nullptr;
};

} // namespace pp::windows

#endif // physp_windows_glfw_hpp