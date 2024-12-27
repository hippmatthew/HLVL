#!/bin/zsh

version=${1:-0.0.0}
source_dir=${2:-./src}
build_dir=${3:-./build}

time=$(date "+%m-%d-%Y %H:%M:%S")
system=$(uname -s)
start_time=$(date +%s)

deps=(future map memory mutex queue set stdexcept string type_traits vector)
file=hlvl_decl
proj=core

log()
{
  local msg=$1

  echo "* generate_headers.zsh: ${msg}"
}

clear()
{
  echo -n > $build_dir/include/hlvl/$file.hpp
}

input()
{
  local msg=$1
  local has_newline=${2:-1}

  if (( has_newline == 1 )); then
    echo $msg >> $build_dir/include/hlvl/$file.hpp
  else
    echo -n $msg >> $build_dir/include/hlvl/$file.hpp
  fi
}

space()
{
  input ""
}

header()
{
  input "// hlvl::${proj}::${file}_hpp generated on ${time} with system ${system}"
}

read_file()
{
  local name=$1
  local class=${2:-${(C)name}}
  local templated=${3:-0}

  local read=0;

  while IFS= read -r line
  do
    if [[ $read == 1 && "${line}" == "};" ]]; then
      input $line
      break
    fi

    if [[  $read != 1 && ("${line}" == "class ${class}" || ($templated == 1 && "${line}" == "template <typename T>")) ]]
    then
      read=1
    fi

    if (( read == 1 )); then
      input $line
      continue
    fi
  done < $source_dir/$proj/include/$name.hpp
}

read_numbered()
{
  local name=$1
  local start=$2
  local end=$3

  local line_num=1

  while IFS= read -r line; do
    if (( line_num >= start )); then
      input $line
    fi

    line_num=$((line_num + 1))

    if (( line_num > end )); then
      break
    fi
  done < $source_dir/$proj/include/$name.hpp
}

includes()
{
  input "#define VULKAN_HPP_NO_CONSTRUCTORS"
  input "#include <vulkan/vulkan_raii.hpp>"
  space

  input "#define GLFW_INCLUDE_NONE"
  input "#include <GLFW/glfw3.h>"
  space

  for dep in "${deps[@]}"; do
    input "#include <${dep}>"
  done
}

defines()
{
  input "#define hlvl_make_version(major, minor, patch) (uint32_t)( \\"
  input "\t( (major) << 16 )\t| \\"
  input "\t( (minor) << 8 )\t| \\"
  input "\t(patch)\t\t\t\t\t\t\\"
  input ")"
  input "#define hlvl_vulkan_version VK_MAKE_API_VERSION(VK_API_VERSION_1_3, 1, 3, 290)"
  input "#define hlvl_engine_version hlvl_make_version(0, 0, 0)"
  space

  input "#define hlvl_settings_manager hlvl::SettingsManager::instance()"
  input "#define hlvl_general_settings hlvl_settings_manager.settings<hlvl::GeneralSettings>()"
  input "#define hlvl_window_settings  hlvl_settings_manager.settings<hlvl::WindowSettings>()"
}

forward_declarations()
{
  input "class Allocation;"
  input "class Allocator;"
  input "class Buffer;"
  input "class ComponentManager;"
  input "class Context;"
  input "class Device;"
  input "class ECSController;"
  input "class EntityManager;"
  input "class IComponentArray;"
  input "template<typename T> class ComponentArray;"
  input "class IInterface;"
  input "template<typename T> class Interface;"
  input "class IResource;"
  input "template<typename T> class Resource;"
  input "class ISystem;"
  input "class IWindow;"
  input "class Settings;"
  input "class GeneralSettings;"
  input "class WindowSettings;"
  input "class SettingsManager;"
  input "class Signature;"
  input "class StagingBuffer;"
  input "class SystemManager;"
  space

  input "namespace windows"
  input "{"
  space

  input "class GLFW;"
  space

  input "} // namespace hlvl::windows"
}

mainloop_macros()
{
  input "#define hlvl_loop_start \\"
  input "\tmainloop: \\"
  input "\t\thlvl_general_settings.p_context->poll_events();"
  space

  input "#define hlvl_loop_end(condition) \\"
  input "\tif (!(hlvl_general_settings.p_context->should_close() || condition)) \\"
  input "\t\tgoto mainloop;"
}

log "Generating HLVL version ${version} headers on system ${system}"

if [[ ! -d "${build_dir}/include/hlvl" ]]; then
  mkdir -p $build_dir/include/hlvl
  log "made directory ${build_dir}/include/hlvl"
fi

#############################
# hlvl_decl.hpp generation #
#############################
log "generating ${file}.hpp"

clear
header
space

input "#pragma once"
space

includes
space

defines
space

input "namespace hlvl"
input "{"
space

forward_declarations
space

input "using Entity = unsigned long;"
space

# enum FamilyType
read_numbered device 10 17
space

# enum Key
read_numbered iwindow 16 83
space

# enum Locality
read_numbered allocation 11 15
space

# enum QueueType
read_numbered device 19 28
space

# enum ResourceType
read_numbered resource_decl 8 14
space

# struct ResourcePool
read_numbered allocation 17 21
space

read_file allocation
space

read_file stagingbuffer "StagingBuffer"
space

read_file allocator
space

read_file allocation "Buffer"
space

read_file entitymanager "EntityManager"
space

read_file components_decl "ComponentManager"
space

read_file systems_decl "SystemManager"
space

read_file ecscontroller_decl "ECSController"
space

read_file context_decl "Context"
space

read_file device
space

read_file components_decl "IComponentArray"
space

read_file components_decl "ComponentArray" 1
space

read_file interface_decl "IInterface"
space

read_file interface_decl "Interface" 1
space

read_file resource_decl "IResource"
space

read_file resource_decl "Resource" 1
space

read_file systems_decl "ISystem"
space

read_file iwindow "IWindow"
space

read_file settings_decl "Settings"
space

read_file settings_decl "GeneralSettings : public Settings"
space

read_file settings_decl "WindowSettings : public Settings"
space

read_file settings_decl "SettingsManager"
space

read_file signature

proj=windows

input "namespace windows"
input "{"

space

read_file glfw "GLFW : public IWindow"

space

input "} // namespace hlvl::windows"
input "} // namespace hlvl" 0

########################
# hlvl.hpp generation #
########################
file=hlvl
proj=core

log "Generating ${file}.hpp"

clear
header
space

input "#pragma once"
space

input "#include \"hlvl_decl.hpp\""
space

mainloop_macros
space

input "namespace hlvl"
input "{"
space

read_numbered components 10 171
space

read_numbered context 8 17
space

read_numbered ecscontroller 10 106
space

read_numbered interface 8 66
space

read_numbered resource 8 92
space

read_numbered settings 11 94
space

read_numbered systems 12 104
space

input "} // namespace hlvl" 0

end_time=$(date +%s)
log "done in $(echo "$end_time - $start_time" | bc)s."