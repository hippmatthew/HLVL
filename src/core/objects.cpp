#include "src/core/include/objects.hpp"
#include "src/core/include/vkbuilders.hpp"

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

Object::Object(Object::ObjectBuilder& objectBuilder) {
  if (objectBuilder.vertices.size() < 3)
    throw std::runtime_error("hlvl: object builder must contain at least 3 vertices");

  if (objectBuilder.indices.size() < 3)
    throw std::runtime_error("hlvl: object builder must contain at least 3 indices");

  if (objectBuilder.material == "")
    throw std::runtime_error("hlvl: object builder must contain a material");

  unsigned int vertexSize = objectBuilder.vertices.size() * sizeof(Vertex);
  unsigned int indexSize = objectBuilder.indices.size() * sizeof(unsigned int);

  BufferBuilder bufferBuilder;
  bufferBuilder
    .new_buffer(vertexSize, vk::BufferUsageFlagBits::eTransferSrc)
    .new_buffer(indexSize, vk::BufferUsageFlagBits::eTransferSrc)
    .allocate(vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

  std::vector<unsigned int> stagingOffsets = bufferBuilder.retrieve_offsets();
  std::vector<vk::raii::Buffer> stagingBuffers = bufferBuilder.retrieve_buffers();
  vk::raii::DeviceMemory stagingMemory = bufferBuilder.retrieve_memory();

  void * data = stagingMemory.mapMemory(0, stagingOffsets[1] + indexSize);

  memcpy(data, objectBuilder.vertices.data(), vertexSize);
  memcpy((char *)data + stagingOffsets[1], objectBuilder.indices.data(), indexSize);

  stagingMemory.unmapMemory();
  data = nullptr;

  bufferBuilder.clear()
    .new_buffer(vertexSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst)
    .new_buffer(indexSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst)
    .allocate(vk::MemoryPropertyFlagBits::eDeviceLocal);

  vk_buffers = bufferBuilder.retrieve_buffers();
  vk_memory = bufferBuilder.retrieve_memory();

  CommandBufferBuilder commandBuilder(vk::CommandPoolCreateFlagBits::eTransient, 2, Transfer);
  vk::raii::CommandBuffers commandBuffers = commandBuilder.retrieve_buffers();

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
    throw std::runtime_error("hlvl: hung waiting for buffer transfer");
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

unsigned int Objects::count() const {
  return objects.size();
}

void Objects::add(Object::ObjectBuilder& objectBuilder) {
  objects.emplace_back(Object(objectBuilder));
}

} // namespace hlvl