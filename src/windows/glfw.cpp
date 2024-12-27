#include "../core/include/iwindow.hpp"
#include "../core/include/settings.hpp"
#include "include/glfw.hpp"

#include <limits>
#include <stdexcept>
#include <iostream>

namespace hlvl::windows
{

#ifdef TESTS
  GLFWwindow * GLFW::p_window = nullptr;
  bool GLFW::resized = false;
#endif // TESTS

GLFW::GLFW()
{
  if (!hlvl_window_settings.initialized)
  {
    glfwInit();
    hlvl_window_settings.initialized = true;
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  gl_window = glfwCreateWindow(
    hlvl_window_settings.size[0],
    hlvl_window_settings.size[1],
    hlvl_window_settings.title.c_str(),
    nullptr, nullptr
  );

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);

  hlvl_window_settings.scale = { sx, sy };

  glfwSetWindowUserPointer(gl_window, this);
  glfwSetFramebufferSizeCallback(gl_window, GLFW::resize_callback);
  glfwSetKeyCallback(gl_window, GLFW::key_callback);

  #ifdef TESTS
    p_window = gl_window;
  #endif // TESTS
}

GLFW::~GLFW()
{
  glfwDestroyWindow(gl_window);
  glfwTerminate();
}

std::vector<const char *> GLFW::instance_extensions()
{
  if (!hlvl_window_settings.initialized)
  {
    glfwInit();
    hlvl_window_settings.initialized = true;
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

  if (keyState != -1)
    call_keybind(keyState);
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
    throw std::runtime_error("hlvl::windows::GLFW: failed to get image index");

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

void GLFW::resize_callback(GLFWwindow * w, int width, int height)
{
  auto * window = reinterpret_cast<GLFW *>(glfwGetWindowUserPointer(w));
  window->modifiedFramebuffer = true;
  resized = true;
}

void GLFW::key_callback(GLFWwindow * w, int key, int, int action, int)
{
  auto window = reinterpret_cast<GLFW *>(glfwGetWindowUserPointer(w));

  if (action == GLFW_PRESS)
  {
    if (window->keyState == -1)
      window->keyState = 0;

    window->keyState |= get_key(key);
    ++window->keyCount;

    if (window->keyCount > 1)
      window->keyState |= (1 << 31);
  }

  if (action == GLFW_RELEASE)
  {
    window->keyState &= ~get_key(key);
    --window->keyCount;

    if (window->keyCount == 0)
      window->keyState = -1;
  }
}

Key GLFW::get_key(int key)
{
  switch(key)
  {
    case GLFW_KEY_A:
      return a;
    case GLFW_KEY_B:
      return b;
    case GLFW_KEY_C:
      return c;
    case GLFW_KEY_D:
      return d;
    case GLFW_KEY_E:
      return e;
    case GLFW_KEY_F:
      return f;
    case GLFW_KEY_G:
      return g;
    case GLFW_KEY_H:
      return h;
    case GLFW_KEY_I:
      return i;
    case GLFW_KEY_J:
      return j;
    case GLFW_KEY_K:
      return k;
    case GLFW_KEY_L:
      return l;
    case GLFW_KEY_M:
      return m;
    case GLFW_KEY_N:
      return n;
    case GLFW_KEY_O:
      return o;
    case GLFW_KEY_P:
      return p;
    case GLFW_KEY_Q:
      return q;
    case GLFW_KEY_R:
      return r;
    case GLFW_KEY_S:
      return s;
    case GLFW_KEY_T:
      return t;
    case GLFW_KEY_U:
      return u;
    case GLFW_KEY_V:
      return v;
    case GLFW_KEY_W:
      return w;
    case GLFW_KEY_X:
      return x;
    case GLFW_KEY_Y:
      return y;
    case GLFW_KEY_Z:
      return z;
    case GLFW_KEY_0:
      return zero;
    case GLFW_KEY_1:
      return one;
    case GLFW_KEY_2:
      return two;
    case GLFW_KEY_3:
      return three;
    case GLFW_KEY_4:
      return four;
    case GLFW_KEY_5:
      return five;
    case GLFW_KEY_6:
      return six;
    case GLFW_KEY_7:
      return seven;
    case GLFW_KEY_8:
      return eight;
    case GLFW_KEY_9:
      return nine;
    case GLFW_KEY_SPACE:
      return space;
    case GLFW_KEY_APOSTROPHE:
      return apostrophe;
    case GLFW_KEY_COMMA:
      return comma;
    case GLFW_KEY_MINUS:
      return minus;
    case GLFW_KEY_PERIOD:
      return period;
    case GLFW_KEY_SLASH:
      return slash;
    case GLFW_KEY_BACKSLASH:
      return backslash;
    case GLFW_KEY_SEMICOLON:
      return semicolon;
    case GLFW_KEY_EQUAL:
      return equal;
    case GLFW_KEY_LEFT_BRACKET:
      return left_bracket;
    case GLFW_KEY_RIGHT_BRACKET:
      return right_bracket;
    case GLFW_KEY_GRAVE_ACCENT:
      return grave;
    case GLFW_KEY_ESCAPE:
      return escape;
    case GLFW_KEY_ENTER:
      return enter;
    case GLFW_KEY_TAB:
      return tab;
    case GLFW_KEY_BACKSPACE:
      return backspace;
    case GLFW_KEY_UP:
      return up;
    case GLFW_KEY_DOWN:
      return down;
    case GLFW_KEY_LEFT:
      return left;
    case GLFW_KEY_RIGHT:
      return right;
    case GLFW_KEY_CAPS_LOCK:
      return caps_lock;
    case GLFW_KEY_LEFT_SHIFT:
      return left_shift;
    case GLFW_KEY_LEFT_CONTROL:
      return left_control;
    case GLFW_KEY_LEFT_ALT:
      return left_alt;
    case GLFW_KEY_LEFT_SUPER:
      return left_super;
    case GLFW_KEY_RIGHT_SHIFT:
      return right_shift;
    case GLFW_KEY_RIGHT_ALT:
      return right_alt;
    case GLFW_KEY_RIGHT_SUPER:
      return right_super;
    default:
      return unknown;
  }
}

void GLFW::createSwapchain()
{
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
    .imageFormat            = hlvl_window_settings.format,
    .imageColorSpace        = hlvl_window_settings.color_space,
    .imageExtent            = hlvl_window_settings.extent(),
    .imageArrayLayers       = 1,
    .imageUsage             = vk::ImageUsageFlagBits::eColorAttachment,
    .imageSharingMode       = vk::SharingMode::eExclusive,
    .queueFamilyIndexCount  = 0,
    .pQueueFamilyIndices    = nullptr,
    .preTransform           = capabilities.currentTransform,
    .compositeAlpha         = vk::CompositeAlphaFlagBitsKHR::eOpaque,
    .presentMode            = hlvl_window_settings.present_mode,
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

  hlvl_window_settings.size = { static_cast<unsigned int>(width), static_cast<unsigned int>(height) };

  float sx, sy;
  glfwGetWindowContentScale(gl_window, &sx, &sy);
  hlvl_window_settings.scale = { sx, sy };

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
      .format     = hlvl_window_settings.format,
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

} // namespace hlvl::windows