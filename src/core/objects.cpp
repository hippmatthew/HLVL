#include "src/core/include/objects.hpp"
#include "src/core/include/vkfactory.hpp"
#include "src/obj/include/parser.hpp"

#include <stdexcept>

namespace hlvl {

Objects * Objects::p_objects = nullptr;

Object::ObjectBuilder& Object::ObjectBuilder::add_vertices(std::vector<Vertex> v) {
  vertices = v;
  return *this;
}

Object::ObjectBuilder& Object::ObjectBuilder::add_indices(std::vector<unsigned int> i) {
  indices = i;
  return *this;
}

Object::ObjectBuilder& Object::ObjectBuilder::add_material(std::string tag) {
  material = tag;
  return *this;
}

Object::ObjectBuilder& Object::ObjectBuilder::add_model(std::string path) {
  auto [tmp_vertices, tmp_indices] = obj::ObjParser::parse(path);
  vertices = std::move(tmp_vertices);
  indices = std::move(tmp_indices);
  return *this;
}

Object::Object(Object::ObjectBuilder& objectBuilder) {
  if (objectBuilder.vertices.size() < 3)
    throw std::runtime_error("hlvl: object builder must contain at least 3 vertices");

  if (objectBuilder.indices.size() < 3)
    throw std::runtime_error("hlvl: object builder must contain at least 3 indices");

  if (objectBuilder.material == "")
    throw std::runtime_error("hlvl: object builder must contain a material");

  indexCount = objectBuilder.indices.size();
  materialTag = objectBuilder.material;

  unsigned int vertexSize = objectBuilder.vertices.size() * sizeof(Vertex);
  unsigned int indexSize = objectBuilder.indices.size() * sizeof(unsigned int);

  std::vector<vk::BufferCreateInfo> bufferInfos = {
    vk::BufferCreateInfo{
      .size         = vertexSize,
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    },
    vk::BufferCreateInfo{
      .size         = indexSize,
      .usage        = vk::BufferUsageFlagBits::eTransferSrc,
      .sharingMode  = vk::SharingMode::eExclusive
    }
  };

  auto [stagingMemory, stagingBuffers, stagingOffsets, allocationSize] = VulkanFactory::newAllocation(
    bufferInfos, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent
  );

  void * data = stagingMemory.mapMemory(0, stagingOffsets[1] + indexSize);

  memcpy(data, objectBuilder.vertices.data(), vertexSize);
  memcpy((char *)data + stagingOffsets[1], objectBuilder.indices.data(), indexSize);

  stagingMemory.unmapMemory();
  data = nullptr;

  bufferInfos[0].usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
  bufferInfos[1].usage = vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst;

  auto [tmp_memory, tmp_buffers, _, __] = VulkanFactory::newAllocation(bufferInfos, vk::MemoryPropertyFlagBits::eDeviceLocal);
  vk_memory = std::move(tmp_memory);
  vk_buffers = std::move(tmp_buffers);

  auto [commandPool, commandBuffers] = VulkanFactory::newCommandPool(Transfer, 2, vk::CommandPoolCreateFlagBits::eTransient);
  unsigned int sizes[2] = { vertexSize, indexSize };

  unsigned int index = 0;
  std::vector<vk::CommandBuffer> commands;
  for (auto& command : commandBuffers) {
    command.begin(vk::CommandBufferBeginInfo{});

    vk::BufferCopy copy{
      .srcOffset  = 0,
      .dstOffset  = 0,
      .size       = sizes[index]
    };

    command.copyBuffer(stagingBuffers[index], vk_buffers[index], copy);
    command.end();

    commands.emplace_back(command);

    ++index;
  }

  vk::SubmitInfo transferSubmit{
    .commandBufferCount = static_cast<unsigned int>(commands.size()),
    .pCommandBuffers    = commands.data()
  };

  vk::raii::Fence transferFence = Context::device().createFence(vk::FenceCreateInfo{});

  Context::queue(Transfer).submit(transferSubmit, transferFence);

  if (Context::device().waitForFences(*transferFence, true, 1000000000ul) != vk::Result::eSuccess)
    throw std::runtime_error("hlvl: hung waiting for transfer fence");
}

Object::ObjectBuilder Object::builder() {
  return ObjectBuilder();
}

Objects& Objects::instance() {
  if (p_objects == nullptr)
    p_objects = new Objects();

  return *p_objects;
}

void Objects::destroy() {
  if (p_objects == nullptr) return;

  delete p_objects;
  p_objects = nullptr;
}

std::vector<Object>::const_iterator Objects::begin() const {
  return objects.begin();
}

std::vector<Object>::const_iterator Objects::end() const {
  return objects.end();
}

unsigned int Objects::count() const {
  return objects.size();
}

void Objects::add(Object::ObjectBuilder& objectBuilder) {
  objects.emplace_back(Object(objectBuilder));
}

} // namespace hlvl