#!/bin/zsh

version=${1:-0.0.0}
source_dir=${2:-./src}
build_dir=${3:-./build}

time=$(date "+%m-%d-%Y %H:%M:%S")
system=$(uname -s)
start_time=$(date +%s)

deps=(map memory stdexcept string type_traits)
file=physp_decl
proj=core

log()
{
  local msg=$1

  echo "* generate_headers.zsh: ${msg}"
}

clear()
{
  echo -n > $build_dir/include/physp/$file.hpp
}

input()
{
  local msg=$1
  local has_newline=${2:-1}

  if (( has_newline == 1 )); then
    echo $msg >> $build_dir/include/physp/$file.hpp
  else
    echo -n $msg >> $build_dir/include/physp/$file.hpp
  fi
}

space()
{
  input ""
}

header()
{
  input "// physp::${proj}::${file}_hpp generated on ${time} with system ${system}"
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
  done < ${source_dir}/${proj}/include/${name}.hpp
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
  input "#define pp_make_version(major, minor, patch) (uint32_t)( \\"
  input "\t( (major) << 16 )\t| \\"
  input "\t( (minor) << 8 )\t| \\"
  input "\t(patch)\t\t\t\t\t\t\\"
  input ")"
  input "#define pp_vulkan_version VK_MAKE_API_VERSION(VK_API_VERSION_1_3, 1, 3, 290)"
  input "#define pp_engine_version pp_make_version(0, 0, 0)"
  input "#define pp_settings_manager pp::SettingsManager::instance()"
}

forward_declarations()
{
  input "class Context;"
  input "class IInterface;"
  input "template<typename T> class Interface;"
  input "class IWindow;"
  input "class KeyCallback;"
  input "class Settings;"
  input "class General;"
  input "class SettingsManager;"

  space

  input "namespace windows"
  input "{"

  space

  input "class GLFW;"

  space

  input "} // namespace pp::windows"
}

log "Generating Physics+ version ${version} headers on system ${system}"

if [[ ! -d "${build_dir}/include/physp" ]]; then
  mkdir ${build_dir}/include/physp
  log "made directory ${build_dir}/include/physp"
fi

log "generating ${file}.hpp"

#############################
# physp_decl.hpp generation #
#############################
clear
header

space

input "#ifndef physp_decl_hpp"
input "#define physp_decl_hpp"

space

includes

space

defines

space

input "namespace pp"
input "{"

space

forward_declarations

space

read_file context_decl "Context"

space

read_file interface_decl "IInterface"

space

read_file interface_decl "Interface" 1

space

read_file iwindow_decl "IWindow"

space

read_file iwindow_decl "KeyCallback"

space

read_file settings_decl "Settings"

space

read_file settings_decl "General : public Settings"

space

read_file settings_decl "Window : public Settings"

space

read_file settings_decl "SettingsManager"

space

proj=windows

input "namespace windows"
input "{"

space

read_file glfw "GLFW : public IWindow"

space

input "} // namespace pp::windows"
input "} // namespace pp"

space

input "#endif // physp_decl_hpp" 0

########################
# physp.hpp generation #
########################

file=physp
proj=core

log "Generating ${file}.hpp"

clear
header

space

input "#ifndef physp_hpp"
input "#define physp_hpp"

space

input "#include \"./physp_decl.hpp\""

space

input "namespace pp"
input "{"

space

read_numbered context 9 17

space

read_numbered interface 9 31

space

read_numbered settings 9 84

space

input "} // namespace pp"

space

input "#endif // ${file}_hpp" 0

end_time=$(date +%s)

log "done in $(echo "$end_time - $start_time" | bc)s."