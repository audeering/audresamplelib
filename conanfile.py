from conan import ConanFile
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout


class AudresampleLibConan(ConanFile):
    name = "audresamplelib"
    version = "latest"
    license = "MIT"
    url = "https://github.com/audeering/audresamplelib"
    description = "A C-wrapper around soxr for Sample Rate Conversion (SRC)"
    topics = ("resample", "soxr")
    generators = "CMakeDeps"

    settings = "os", "arch", "compiler", "build_type"
    options = {
        "shared": [True, False],
        "fPIC": [True, False],
        "build_tools": [True, False],
    }
    default_options = {
        "shared": False,
        "fPIC": True,
        "build_tools": False,
    }

    exports_sources = "src/*", "include/*", "progsrc/*", "tests/*", "CMakeLists.txt", "LICENSE"

    def config_options(self):
        if self.settings.os == "Windows":
            self.options.rm_safe("fPIC")

    def configure(self):
        if self.options.shared:
            self.options.rm_safe("fPIC")
        self.settings.rm_safe("compiler.cppstd")
        self.settings.rm_safe("compiler.libcxx")

    def requirements(self):
        self.requires("soxr/0.1.3", transitive_headers=True)
        self.requires("drwav/0.13.7", transitive_headers=True)

    def layout(self):
        cmake_layout(self, src_folder=".")

    def generate(self):
        tc = CMakeToolchain(self)
        tc.variables["BUILD_TOOLS"] = self.options.build_tools
        tc.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def package(self):
        cmake = CMake(self)
        cmake.install()

    def package_info(self):
        self.cpp_info.libs = ["audresample"]
