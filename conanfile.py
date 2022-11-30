from conans import ConanFile, CMake, tools


class AudresampleLibConan(ConanFile):
    name = "audresamplelib"
    version = "latest"
    license = "MIT"
    url = "https://github.com/audeering/audresamplelib"
    description = "A C-wrapper around soxr for Sample Rate Conversion (SRC)"

    settings = "os", "arch", "compiler", "build_type"
    options = {"shared": [True, False], "fPIC": [True, False], "build_tools": [True, False]}
    default_options = {"shared": False, "fPIC": True, "build_tools": False}

    generators = "cmake"
    exports_sources = "src/*", "include/*", "progsrc/*", "tests/*", "CMakeLists.txt"

    def requirements(self):
        self.requires("soxr/0.1.3")
        self.requires("drwav/0.13.7")

    def configure(self):
        del self.settings.compiler.libcxx
        del self.settings.compiler.cppstd

        if self.settings.compiler == "Visual Studio":
            del self.options.fPIC

    def build(self):
        self.cmake = CMake(self)
        self.cmake.definitions["BUILD_TOOLS"] = self.options.build_tools
        self.cmake.configure()
        self.cmake.build()

    def package(self):
        self.copy("*", dst="bin", src="bin")
        self.copy("*.h", dst="include", src="include")
        self.copy("*.lib", dst="lib", keep_path=False)
        self.copy("*.dll", dst="bin", keep_path=False)
        self.copy("*.dylib*", dst="lib", keep_path=False)
        self.copy("*.so", dst="lib", keep_path=False)
        self.copy("*.a", dst="lib", keep_path=False)

    def package_info(self):
        self.cpp_info.libs = ["audresample"]
