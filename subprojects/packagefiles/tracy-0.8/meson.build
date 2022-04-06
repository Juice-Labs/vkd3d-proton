project('tracy', ['cpp'], version : '0.8')

add_project_arguments('-DTRACY_EXPORTS', language : 'cpp')
add_project_arguments('-DTRACY_ENABLE', language : 'cpp')
add_project_arguments('-DTRACY_ONLY_LOCALHOST', language : 'cpp')
add_project_arguments('-DTRACY_ON_DEMAND', language : 'cpp')
add_project_arguments('-DTRACY_NO_CALLSTACK', language : 'cpp')
add_project_arguments('-DTRACY_DELAYED_INIT', language : 'cpp')

threads_dep = dependency('threads')

includes = [
    'TracyC.h',
    'Tracy.hpp',
    'TracyD3D11.hpp',
    'TracyD3D12.hpp',
    'TracyLua.hpp',
    'TracyOpenCL.hpp',
    'TracyOpenGL.hpp',
    'TracyVulkan.hpp'
]

client_includes = [
    'client/tracy_concurrentqueue.h',
    'client/tracy_rpmalloc.hpp',
    'client/tracy_SPSCQueue.h',
    'client/TracyArmCpuTable.hpp',
    'client/TracyCallstack.h',
    'client/TracyCallstack.hpp',
    'client/TracyDebug.hpp',
    'client/TracyDxt1.hpp',
    'client/TracyFastVector.hpp',
    'client/TracyLock.hpp',
    'client/TracyProfiler.hpp',
    'client/TracyRingBuffer.hpp',
    'client/TracyScoped.hpp',
    'client/TracyStringHelpers.hpp',
    'client/TracySysTime.hpp',
    'client/TracySysTrace.hpp',
    'client/TracyThread.hpp'
]

common_includes = [
    'common/tracy_lz4.hpp',
    'common/tracy_lz4hc.hpp',
    'common/TracyAlign.hpp',
    'common/TracyAlign.hpp',
    'common/TracyAlloc.hpp',
    'common/TracyApi.h',
    'common/TracyColor.hpp',
    'common/TracyForceInline.hpp',
    'common/TracyMutex.hpp',
    'common/TracyProtocol.hpp',
    'common/TracyQueue.hpp',
    'common/TracySocket.hpp',
    'common/TracyStackFrames.hpp',
    'common/TracySystem.hpp',
    'common/TracyUwp.hpp',
    'common/TracyYield.hpp'
]

tracy_header_files = common_includes + client_includes + includes

tracy_src = [
    'TracyClient.cpp'
]

tracy_public_include_dirs = include_directories('.')

tracy = shared_library('tracy', tracy_src, tracy_header_files,
  dependencies        : [ threads_dep ],
  include_directories : tracy_public_include_dirs,
  override_options    : [ 'cpp_std=c++11' ])

tracy_dep = declare_dependency(
    include_directories : tracy_public_include_dirs)

meson.override_dependency('tracy', tracy_dep)
