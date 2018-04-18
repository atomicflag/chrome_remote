from conans import ConanFile, Meson
from os.path import exists

class ChromeRemoteConan(ConanFile):
	name = 'chrome_remote'
	version = '1.2.0'
	license = 'MIT'
	url = 'https://gitlab.com/signal9/chrome_remote'
	options = {'shared':[True,False]}
	default_options = 'shared=True', '*:shared=True'
	settings = 'os', 'compiler', 'build_type', 'arch'
	build_policy = 'missing'
	description = 'Chromium DevTools remote for C++.'
	exports_sources = \
		'meson.build', \
		'meson_options.txt', \
		'src/*', \
		'include/*'
	generators = 'pkg_config'
	requires = \
		'fmt/4.1.0@signal9/stable', \
		'boost_beast/1.67.0@bincrafters/testing', \
		'boost_thread/1.67.0@bincrafters/testing', \
		'boost_process/1.67.0@bincrafters/testing', \
		'boost_asio/1.67.0@bincrafters/testing', \
		'boost_iostreams/1.67.0@bincrafters/testing', \
		'json/[>=3.1]@signal9/stable', \
		'cppcodec/[>=1.0]@signal9/stable', \
		'continuable/[>=3.0]@signal9/stable'

	def build(self):
		cmake = Meson(self)
		cmake.configure(
			source_folder=self.source_folder,
			args=['--prefix='+self.package_folder]
			)
		cmake.build(targets=['install'])

	def package_info(self):
		self.cpp_info.libs = ['chrome_remote']
		self.cpp_info.libdirs = list(filter(exists,('lib32','lib64','lib')))
