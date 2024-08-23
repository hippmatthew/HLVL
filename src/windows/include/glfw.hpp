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
    static std::vector<const char *> device_extensions();
    static void resize_callback(GLFWwindow *, int, int);

    bool should_close() const override;
    void poll_events() const override;
    const vk::Image& image(unsigned long) const override;
    const vk::raii::ImageView& image_view(unsigned long) const override;
    unsigned long next_image_index(const vk::raii::Semaphore&) override;

    void create_surface(const vk::raii::Instance&) override;
    void load(std::shared_ptr<Device>) override;

  private:
    void createSwapchain();
    void recreateSwapchain();
    void createViews();

  protected:
    GLFWwindow *  gl_window = nullptr;
    std::shared_ptr<Device> p_device = nullptr;
    bool modifiedFramebuffer = false;

    vk::raii::SwapchainKHR vk_swapchain = nullptr;
    std::vector<vk::Image> vk_images;
    std::vector<vk::raii::ImageView> vk_imageViews;
};

} // namespace pp::windows

#endif // physp_windows_glfw_hpp