#include "include/device.hpp"
#include "include/settings_decl.hpp"

#include <queue>

#define VK_PORTABILITY_SUBSET_NAME "VK_KHR_portability_subset"

#define HLVL_GRAPHICS_QUEUE_BIT         0x1000000u
#define HLVL_PRESENT_QUEUE_BIT          0x0100000u
#define HLVL_SYNC_COMPUTE_QUEUE_BIT     0x0010000u
#define HLVL_ASYNC_COMPUTE_QUEUE_BIT    0x0001000u
#define HLVL_SYNC_TRANSFER_QUEUE_BIT    0x0000100u
#define HLVL_ASYNC_TRANSFER_QUEUE_BIT   0x0000010u
#define HLVL_SPARSE_QUEUE_BIT           0x0000001u

#define HLVL_ALL_FAMILY         ( HLVL_GRAPHICS_QUEUE_BIT | HLVL_PRESENT_QUEUE_BIT | HLVL_SYNC_COMPUTE_QUEUE_BIT | HLVL_SYNC_TRANSFER_QUEUE_BIT )
#define HLVL_COMPUTE_FAMILY     ( HLVL_ASYNC_COMPUTE_QUEUE_BIT )
#define HLVL_TRANSFER_FAMILY    ( HLVL_ASYNC_TRANSFER_QUEUE_BIT )
#define HLVL_ASYNC_FAMILY       ( HLVL_ASYNC_COMPUTE_QUEUE_BIT | HLVL_ASYNC_TRANSFER_QUEUE_BIT )
#define HLVL_SPARSE_FAMILY      ( HLVL_SPARSE_QUEUE_BIT )

namespace hlvl
{

Device::QueueFamily::QueueFamily(unsigned long index, unsigned int types)
: qf_index(index), qf_types(types)
{}

Device::QueueFamilies::QueueFamilies(const vk::raii::PhysicalDevice& vk_physicalDevice, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<unsigned long> f_all, f_compute, f_transfer, f_async, f_sparse;

  unsigned long index = 0;
  for (const auto& family : vk_physicalDevice.getQueueFamilyProperties())
  {
    unsigned int types = 0x0000000u;

    bool surfaceSupport = true;
    if (hlvl_general_settings.draw_window)
      surfaceSupport = vk_physicalDevice.getSurfaceSupportKHR(index, *vk_surface);

    if (family.queueFlags & vk::QueueFlagBits::eGraphics && surfaceSupport)
      types |= HLVL_GRAPHICS_QUEUE_BIT | HLVL_PRESENT_QUEUE_BIT;

    if (family.queueFlags & vk::QueueFlagBits::eCompute)
    {
      unsigned int type = HLVL_SYNC_COMPUTE_QUEUE_BIT;
      if (!(types & HLVL_GRAPHICS_QUEUE_BIT))
        type = HLVL_ASYNC_COMPUTE_QUEUE_BIT;

      types |= type;
    }

    if (family.queueFlags & vk::QueueFlagBits::eTransfer)
    {
      unsigned int type = HLVL_SYNC_TRANSFER_QUEUE_BIT;
      if (!(types & HLVL_GRAPHICS_QUEUE_BIT))
        type = HLVL_ASYNC_TRANSFER_QUEUE_BIT;

      types |= type;
    }

    if (family.queueFlags & vk::QueueFlagBits::eSparseBinding)
      types |= HLVL_SPARSE_QUEUE_BIT;

    switch (to_family(types))
    {
      case All:
        f_all.emplace(index++);
        continue;
      case Compute:
        f_compute.emplace(index++);
        continue;
      case Transfer:
        f_transfer.emplace(index++);
        continue;
      case Async:
        f_async.emplace(index++);
        continue;
      case Sparse:
        f_sparse.emplace(index++);
        continue;
    }
  }

  if (!f_all.empty())
  {
    addFamily(f_all.front(), FamilyType::All);
    f_all.pop();
  }

  if (!f_compute.empty() && !f_transfer.empty())
  {
    addFamily(f_compute.front(), FamilyType::Compute);
    addFamily(f_transfer.front(), FamilyType::Transfer);
  }
  else if (f_async.size() >= 2)
  {
    addFamily(f_async.front(), FamilyType::Compute);
    f_async.pop();
    addFamily(f_async.front(), FamilyType::Transfer);
  }
  else if (f_all.size() >= 2)
  {
    addFamily(f_all.front(), FamilyType::Compute);
    f_all.pop();
    addFamily(f_all.front(), FamilyType::Transfer);
  }
  else if (!f_async.empty())
    addFamily(f_async.front(), FamilyType::Async);
  else if (!f_all.empty())
    addFamily(f_all.front(), FamilyType::Async);

  if (!f_sparse.empty())
    addFamily(f_sparse.front(), FamilyType::Sparse);
}

void Device::QueueFamilies::addFamily(unsigned long index, FamilyType type)
{
  familyMap.emplace(std::make_pair(std::to_string(type), std::make_shared<QueueFamily>(index, to_bits(type))));
  supportedFamilies.emplace_back(type);
}

void Device::QueueFamilies::setQueues(const vk::raii::Device& vk_device)
{
  for (const auto& type : supportedFamilies)
  {
    auto family = familyMap[std::to_string(type)];
    family->qf_queue = vk_device.getQueue(family->qf_index, 0);
  }
}

Device::Device(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface, const void * p_next)
{
  getGPU(vk_instance, vk_surface);
  createDevice(p_next);

  queueFamilies->setQueues(vk_device);
}

const vk::raii::PhysicalDevice& Device::physical() const
{
  return vk_physicalDevice;
}

const vk::raii::Device& Device::logical() const
{
  return vk_device;
}

bool Device::hasFamily(FamilyType type) const
{
  for (const auto& familyType : queueFamilies->supportedFamilies)
  {
    if (familyType == type)
      return true;
  }

  return false;
}

unsigned long Device::familyIndex(FamilyType type) const
{
  return (queueFamilies->familyMap[std::to_string(type)])->qf_index;
}

const vk::raii::Queue& Device::queue(FamilyType type) const
{
  return (queueFamilies->familyMap[std::to_string(type)])->qf_queue;
}

void Device::lockQueue(FamilyType type) const
{
  queueFamilies->familyMap.at(std::to_string(type))->mutex.lock();
}

void Device::unlockQueue(FamilyType type) const
{
  queueFamilies->familyMap.at(std::to_string(type))->mutex.unlock();
}

void Device::getGPU(const vk::raii::Instance& vk_instance, const vk::raii::SurfaceKHR& vk_surface)
{
  std::queue<vk::raii::PhysicalDevice> discreteGPUs, integratedGPUs, virtualGPUs;

  vk::raii::PhysicalDevices GPUs(vk_instance);
  for (const auto& GPU : GPUs)
  {
    auto properties = GPU.getProperties();
    QueueFamilies families(GPU, vk_surface);

    bool hasAllFamily = false;
    for (const auto& family : families.supportedFamilies)
    {
      if (family == FamilyType::All)
      {
        hasAllFamily = true;
        break;
      }
    }
    if (!hasAllFamily) continue;

    bool supportsExtensions = true;
    for (const char * extension : hlvl_general_settings.vk_device_extensions)
    {
      supportsExtensions = false;
      for (const auto& property : GPU.enumerateDeviceExtensionProperties())
      {
        if (std::string(property.extensionName) == extension)
        {
          supportsExtensions = true;
          break;
        }
      }

      if (!supportsExtensions) break;
    }
    if (!supportsExtensions) continue;


    if (hlvl_general_settings.draw_window)
    {
      if (GPU.getSurfaceFormatsKHR(*vk_surface).empty()) continue;
      if (GPU.getSurfacePresentModesKHR(*vk_surface).empty()) continue;
    }

    switch (properties.deviceType)
    {
      case vk::PhysicalDeviceType::eDiscreteGpu:
        discreteGPUs.emplace(GPU);
      case vk::PhysicalDeviceType::eIntegratedGpu:
        integratedGPUs.emplace(GPU);
      case vk::PhysicalDeviceType::eVirtualGpu:
        virtualGPUs.emplace(GPU);
      default:
        continue;
    }
  }

  if (!discreteGPUs.empty())
    vk_physicalDevice = discreteGPUs.front();
  else if (!integratedGPUs.empty())
    vk_physicalDevice = integratedGPUs.front();
  else if (!virtualGPUs.empty())
    vk_physicalDevice = virtualGPUs.front();
  else
    throw std::runtime_error("hlvl::Device: no suitable GPU found");

  queueFamilies = std::make_unique<QueueFamilies>(vk_physicalDevice, vk_surface);

  bool portability = false;
  for (const auto& extension : vk_physicalDevice.enumerateDeviceExtensionProperties())
  {
    if (std::string(extension.extensionName) == VK_PORTABILITY_SUBSET_NAME)
    {
      hlvl_general_settings.add_device_extensions({ VK_PORTABILITY_SUBSET_NAME });
      portability = true;
      break;
    }
  }
  if (portability ^ hlvl_general_settings.portability_enabled)
    hlvl_general_settings.portability_enabled = !hlvl_general_settings.portability_enabled;
}

void Device::createDevice(const void * p_next)
{
  float queuePriority = 1.0f;
  std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;

  for (const auto& type : queueFamilies->supportedFamilies)
  {
    auto family = queueFamilies->familyMap[std::to_string(type)];

    vk::DeviceQueueCreateInfo createInfo{
      .queueFamilyIndex = static_cast<unsigned int>(family->qf_index),
      .queueCount       = 1,
      .pQueuePriorities = &queuePriority
    };

    queueCreateInfos.emplace_back(createInfo);
  }

  vk::DeviceCreateInfo ci_device{
    .pNext                    = p_next,
    .queueCreateInfoCount     = static_cast<unsigned int>(queueCreateInfos.size()),
    .pQueueCreateInfos        = queueCreateInfos.data(),
    .enabledExtensionCount    = static_cast<unsigned int>(hlvl_general_settings.vk_device_extensions.size()),
    .ppEnabledExtensionNames  = hlvl_general_settings.vk_device_extensions.data(),
    .pEnabledFeatures         = &hlvl_general_settings.vk_physical_device_features
  };

  vk_device = vk_physicalDevice.createDevice(ci_device);
}

unsigned int to_bits(FamilyType type)
{
  switch (type)
  {
    case All:
      return HLVL_ALL_FAMILY;
    case Compute:
      return HLVL_COMPUTE_FAMILY;
    case Transfer:
      return HLVL_TRANSFER_FAMILY;
    case Async:
      return HLVL_ASYNC_FAMILY;
    case Sparse:
      return HLVL_SPARSE_FAMILY;
  }

  throw std::runtime_error("hlvl::to_bits: unknown hlvl::FamilyType");
}

FamilyType to_family(unsigned int bits)
{
  switch (bits)
  {
    case HLVL_ALL_FAMILY:
      return FamilyType::All;
    case HLVL_COMPUTE_FAMILY:
      return FamilyType::Compute;
    case HLVL_TRANSFER_FAMILY:
      return FamilyType::Transfer;
    case HLVL_ASYNC_FAMILY:
      return FamilyType::Async;
    case HLVL_SPARSE_FAMILY:
      return FamilyType::Sparse;
  }

  throw std::runtime_error("hlvl::to_family() : uint has no corresponding hlvl::FamilyType");
}

} // namespace hlvl

namespace std
{

string to_string(hlvl::FamilyType type)
{
  switch (type)
  {
    case hlvl::FamilyType::All:
      return "All";
    case hlvl::FamilyType::Compute:
      return "Compute";
    case hlvl::FamilyType::Transfer:
      return "Transfer";
    case hlvl::FamilyType::Async:
      return "Async";
    case hlvl::FamilyType::Sparse:
      return "Sparse";
  }
}

} // namespace std
