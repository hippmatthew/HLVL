#include "src/core/include/settings.hpp"
#include "src/windows/include/glfw.hpp"

#include <limits>
#include <stdexcept>

namespace pp::windows
{

GLFW::GLFW()
{
  auto& s_window = pp_window_settings;

  if (!s_window.initialized)
  {
    glfwInit();
    s_window.initialized = true;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  gl_window = glfwCreateWindow(
    s_window.size[0],
    s_window.size[1],
    s_window.title.c_str(),
    nullptr, nullptr
  );

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);

  s_window.scale = { sx, sy };

  glfwSetWindowUserPointer(gl_window, this);
  glfwSetFramebufferSizeCallback(gl_window, GLFW::resize_callback);
}

GLFW::~GLFW()
{
  glfwDestroyWindow(gl_window);
  glfwTerminate();
}

std::vector<const char *> GLFW::instance_extensions()
{
  auto& s_window = pp_window_settings;
  if (!s_window.initialized)
  {
    glfwInit();
    s_window.initialized = true;
  }

  unsigned int count = 0;
  const char ** extensions = glfwGetRequiredInstanceExtensions(&count);

  return std::vector<const char *>(extensions, extensions + count);
}

std::vector<const char *> GLFW::device_extensions()
{
  return { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
}

bool GLFW::should_close() const
{
  return glfwWindowShouldClose(gl_window);
}

void GLFW::poll_events() const
{
  glfwPollEvents();
}

const vk::Image& GLFW::image(unsigned long index) const
{
  return vk_images[index];
}

const vk::raii::ImageView& GLFW::image_view(unsigned long index) const
{
  return vk_imageViews[index];
}

unsigned long GLFW::next_image_index(const vk::raii::Semaphore& semaphore)
{
  auto [ result, index ] = vk_swapchain.acquireNextImage(std::numeric_limits<unsigned long>::max(), *semaphore, nullptr);

  if (result == vk::Result::eSuboptimalKHR || result == vk::Result::eErrorOutOfDateKHR || modifiedFramebuffer)
    recreateSwapchain();
  else if (result != vk::Result::eSuccess)
    throw std::runtime_error("pp::windows::GLFW: failed to get image index");

  return index;
}

void GLFW::create_surface(const vk::raii::Instance& vk_instance)
{
  VkSurfaceKHR surface = nullptr;
  glfwCreateWindowSurface(*vk_instance, gl_window, nullptr, &surface);

  vk_surface = vk::raii::SurfaceKHR(vk_instance, surface);
}

void GLFW::load(std::shared_ptr<Device> device)
{
  p_device = device;

  createSwapchain();
  vk_images = vk_swapchain.getImages();
  createViews();
}

void GLFW::resize_callback(GLFWwindow * window, int width, int height)
{
  auto gui = reinterpret_cast<GLFW *>(glfwGetWindowUserPointer(window));

  glfwSetWindowSize(gui->gl_window, width, height);
  gui->modifiedFramebuffer = true;
}

void GLFW::createSwapchain()
{
  auto& s_window = pp_window_settings;

  check_format(p_device->physical());
  check_present_mode(p_device->physical());
  check_extent(p_device->physical());

  auto capabilities = p_device->physical().getSurfaceCapabilitiesKHR(*vk_surface);

  unsigned int imageCount = capabilities.minImageCount + 1;
  if (0 < capabilities.maxImageCount && capabilities.maxImageCount < imageCount)
    imageCount = capabilities.maxImageCount;

  vk::SwapchainCreateInfoKHR ci_swapchain{
    .surface                = *vk_surface,
    .minImageCount          = imageCount,
    .imageFormat            = s_window.format,
    .imageColorSpace        = s_window.color_space,
    .imageExtent            = s_window.extent(),
    .imageArrayLayers       = 1,
    .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode       = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount  = 0,
    .pQueueFamilyIndices    = nullptr,
    .preTransform           = capabilities.currentTransform,
    .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode            = s_window.present_mode,
    .clipped                = vk::True,
    .oldSwapchain           = nullptr
  };

  vk_swapchain = p_device->logical().createSwapchainKHR(ci_swapchain);
}

void GLFW::recreateSwapchain()
{
  int width = 0; int height = 0;
  glfwGetWindowSize(gl_window, &width, &height);

  while (width == 0 && height == 0)
  {
    glfwGetWindowSize(gl_window, &width, &height);
    glfwWaitEvents();
  }

  p_device->logical().waitIdle();

  pp_window_settings.size = { static_cast<unsigned int>(width), static_cast<unsigned int>(height) };

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);
  pp_window_settings.scale = { sx, sy };

  vk_swapchain.clear();
  vk_images.clear();
  vk_imageViews.clear();

  createSwapchain();
  vk_images = vk_swapchain.getImages();
  createViews();

  modifiedFramebuffer = false;
}

void GLFW::createViews()
{
  for (const auto& vk_image : vk_images)
  {
    vk::ImageViewCreateInfo ci_view{
      .image      = vk_image,
      .viewType   = vk::ImageViewType::e2D,
      .format     = pp_window_settings.format,
      .components = {
        .r  = vk::ComponentSwizzle::eIdentity,
        .g  = vk::ComponentSwizzle::eIdentity,
        .b  = vk::ComponentSwizzle::eIdentity,
        .a  = vk::ComponentSwizzle::eIdentity
      },
      .subresourceRange = {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
        .baseMipLevel   = 0,
        .levelCount     = 1,
        .baseArrayLayer = 0,
        .layerCount     = 1
      }
    };

    vk_imageViews.emplace_back(p_device->logical().createImageView(ci_view));
  }
}

} // namespace pp::windows