project('chrome_remote', 'cpp',
	default_options : ['cpp_std=c++17'],
	version : '1.2.0')

compiler = meson.get_compiler('cpp')

deps = [
	dependency('fmt'),
	dependency('boost_beast'),
	dependency('boost_process'),
	dependency('boost_thread'),
	dependency('boost_asio'),
	dependency('boost_iostreams'),
	dependency('json'),
	dependency('cppcodec'),
	dependency('continuable'),
	compiler.find_library('m', required : false)
]

inc_dirs = include_directories(['src','include'])

if get_option('use_modules') and compiler.has_argument('-fmodules')
	modules_args = [
		'-fmodules',
		'-fmodules-cache-path='+meson.build_root(),
		'-fmodules-ignore-macro=IS_TESTING'
	]
	add_project_arguments(modules_args, language : 'cpp')
	add_project_link_arguments(modules_args, language : 'cpp')
	if get_option('buildtype') == 'debug'
		gmodule_arg = '-gmodules'
		add_project_arguments(gmodule_arg, language : 'cpp')
		add_project_link_arguments(gmodule_arg, language : 'cpp')
	endif
endif

add_project_arguments([
		'-DBOOST_ASIO_HAS_STD_STRING_VIEW',
		'-DBOOST_ASIO_HAS_MOVE'
	], language : 'cpp')

chrome_remote = library(meson.project_name(), [
		'src/chrome_remote.cpp',
		'src/browser.cpp'
	] , dependencies : deps,
	include_directories : inc_dirs,
	install : true)

install_headers(
	'include/chrome_remote/chrome_remote.h',
	'include/chrome_remote/browser.h',
	'include/chrome_remote/asio_continuable.h',
	subdir : 'chrome_remote')

if get_option('build_demo')
	executable(meson.project_name()+'_demo', [
		'main.cpp'
	],	include_directories : inc_dirs,
		dependencies : deps,
		link_with : chrome_remote)
endif
