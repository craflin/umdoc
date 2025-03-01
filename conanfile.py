from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.env import VirtualBuildEnv
from conan.tools.files import load
import re, os

class umdocConan(ConanFile):
    name = "umdoc"
    license = "Apache-2.0"
    author = "Colin Graf"
    url = "https://github.com/craflin/umdoc"
    description = "Markdown to LaTeX to PDF converter"
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "Doc/*", "Examples/*", "Ext/*", "Src/*", "CDeploy", "CHANGELOG.md", "CMakeLists.txt", "CPackConfig.txt", "CPackInstall.txt", "LICENSE", "NOTICE", "README.md"

    def set_version(self):
        content = load(self, os.path.join(self.recipe_folder, "CMakeLists.txt"))
        self.version = re.search("project\\([^ ]* VERSION ([0-9.]*)", content).group(1)

    def build_requirements(self):
        self.tool_requires("cmake/3.30.1")
        self.tool_requires("ninja/1.12.1")

    def generate(self):
        ms = VirtualBuildEnv(self)
        ms.generate()
        tc = CMakeToolchain(self, generator='Ninja')
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def layout(self):
        cmake_layout(self)

    def package_id(self):
        del self.info.settings.compiler
        del self.info.settings.build_type

    def package_info(self):
        self.cpp_info.bindirs = ['.']
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs.append(os.path.join("lib", "cmake", "umdoc"))
