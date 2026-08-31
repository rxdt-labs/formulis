import os
import re

from conan import ConanFile
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake
from conan.tools.files import copy, load


class FormulisConan(ConanFile):
    name = "formulis"
    description = (
        "Create event listeners with formulae. "
        "Listen for when formulae change values."
    )
    license = "Apache-2.0"
    url = "https://github.com/thyrgle/formulis"
    homepage = "https://thyrgle.github.io/formulis/"
    topics = ("header-only", "events", "listeners", "reactive")

    package_type = "header-library"

    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps", "VirtualRunEnv"

    exports = "LICENSE"
    exports_sources = (
        "include/*",
        "CMakeLists.txt",
        "cmake/*",
        "test/CMakeLists.txt",
        "test/source/*",
    )

    no_copy_source = True

    def set_version(self):
        if not self.version:
            cmake_lists = load(
                self, os.path.join(self.recipe_folder, "CMakeLists.txt")
            )
            match = re.search(r"VERSION\s+(\d+\.\d+\.\d+)", cmake_lists)
            self.version = match.group(1) if match else "0.0.0"

    def layout(self):
        self.folders.generators = "conan"
        self.cpp.source.includedirs = ["include"]

    def build_requirements(self):
        self.test_requires("catch2/3.7.1")

    def validate(self):
        check_min_cppstd(self, 17)

    def build(self):
        cmake = CMake(self)
        cmake.configure(variables={"formulis_DEVELOPER_MODE": "ON"})
        cmake.build()
        if not self.conf.get("tools.build:skip_test", default=False):
            cmake.test()

    def package_id(self):
        self.info.clear()

    def package(self):
        copy(
            self,
            "LICENSE",
            self.recipe_folder,
            os.path.join(self.package_folder, "licenses"),
        )
        copy(
            self,
            "*.hpp",
            os.path.join(self.source_folder, "include"),
            os.path.join(self.package_folder, "include"),
        )

    def package_info(self):
        self.cpp_info.set_property("cmake_file_name", "formulis")
        self.cpp_info.set_property("cmake_target_name", "formulis::formulis")

        self.cpp_info.bindirs = []
        self.cpp_info.libdirs = []
