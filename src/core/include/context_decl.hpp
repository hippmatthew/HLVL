#ifndef physp_core_context_decl_hpp
#define physp_core_context_decl_hpp

#include "src/core/include/interface.hpp"
#include "src/core/include/allocator.hpp"

namespace pp
{

class Context
{
  public:
    Context();
    Context(Context&) = delete;
    Context(Context&&) = delete;

    ~Context();

    Context& operator = (Context&) = delete;
    Context& operator = (Context&&) = delete;

    Allocator& allocator() const;
    bool should_close() const;
    void poll_events() const;

    void initialize(void * p_next = nullptr);

    template <typename T>
    void set_interface();

  private:
    void createInstance();

  protected:
    std::shared_ptr<IInterface> p_interface = nullptr;
    std::shared_ptr<Device> p_device = nullptr;
    std::shared_ptr<Allocator> p_allocator = nullptr;

    vk::raii::Instance vk_instance = nullptr;
};

} // namespace pp

#endif // physp_core_context_decl_hpp
