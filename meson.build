project(
  'beam',
  'c',
  default_options: ['buildtype=debug', 'optimization=0', 'b_lto=false', 'c_std=c11'],
  subproject_dir: 'ThirdParty'
)

# imports
cmake = import('cmake')

beam_incs = include_directories('Source', 'Include')
beam_srcs = files('Bin/Main.c', 'Source/Http.c')

# Dependencies
misra = subproject('MisraStdC')

misra_lib = misra.get_variable('misra_std')
misra_inc = misra.get_variable('inc_misra')
beam = executable(
  'beam',
  beam_srcs,
  include_directories: [beam_incs, misra_inc],
  install: true,
  dependencies: misra.get_variable('misra_std_dep'),
  link_with: misra_lib
)
