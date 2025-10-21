d3d12core_src = [
  'debug.c',
  'main.c'
]

if vkd3d_platform == 'windows'
  d3d12core_name_prefix = 'RemoteGPU'
  d3d12core_dependencies = [ vkd3d_dep, lib_dxgi ]
else
  d3d12core_name_prefix = 'libvkd3d-proton-'
  d3d12core_dependencies = [ vkd3d_dep, lib_dl ]
endif

d3d12core_needs_defs = (not vkd3d_is_msvc) and (vkd3d_platform == 'windows')

d3d12core_lib = shared_library('d3d12core', d3d12core_src,
  name_prefix         : d3d12core_name_prefix,
  dependencies        : d3d12core_dependencies,
  include_directories : vkd3d_private_includes,
  install             : false,
  objects             : d3d12core_needs_defs ? 'd3d12core.def' : [],
  vs_module_defs      : 'd3d12core.def')

if vkd3d_platform == 'windows'
  # Install DLL and PDB to C:/juice/build/Release during meson install
  meson.add_install_script(
    'powershell', '-NoProfile', '-Command',
    'New-Item -ItemType Directory -Force -Path C:\\juice\\build\\Release | Out-Null; '
    + 'Copy-Item "' + meson.current_build_dir() + '\\RemoteGPUd3d12core.dll" "C:\\juice\\build\\Release\\" -Force; '
    + 'if (Test-Path "' + meson.current_build_dir() + '\\RemoteGPUd3d12core.pdb") { Copy-Item "' + meson.current_build_dir() + '\\RemoteGPUd3d12core.pdb" "C:\\juice\\build\\Release\\" -Force; }'
  )
endif

d3d12core_dep = declare_dependency(
  link_with           : d3d12core_lib,
  include_directories : vkd3d_public_includes)
