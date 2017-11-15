from conans import ConanFile, CMake

class ChromeRemoteConan(ConanFile):
    name = 'chrome_remote'
    version = '1.0.0'
    license = 'MIT'
    url = 'http://www.example.com'
    options = {'shared':[True,False]}
    default_options = 'shared=True', \
		'Beast:commit=9dc9ca13b9c08c1597d05bcf6c19be357e426041'
    settings = 'os', 'compiler', 'build_type', 'arch'
    build_policy = 'missing'
    description = 'Chromium DevTools remote for C++.'
    exports_sources = 'CMakeLists.txt', 'src/*', 'include/*'
    generators = 'cmake'
    requires = \
        'fmt/latest@signal9/stable', \
        'Beast/latest@signal9/stable', \
        'json/latest@signal9/stable', \
        'cppcodec/latest@signal9/stable', \
		'continuable/latest@signal9/stable'

    def build(self):
        cmake = CMake(self)
        cmake.configure(source_dir=self.source_folder)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ['chrome_remote']
