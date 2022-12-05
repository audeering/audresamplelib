from conans import ConanFile, CMake, tools


class AudresampleLibTestConan(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "cmake"
    requires = "catch2/2.13.7"

    def configure(self):
        tools.check_min_cppstd(self, "11")

    def build(self):
        self.cmake = CMake(self)
        self.cmake.configure()
        self.cmake.build()

    def imports(self):
        # using @bindirs and @libdirs placeholders to also support dependencies in editable mode (see https://docs.conan.io/en/latest/reference/conanfile/methods.html#imports)
        self.copy("*.dll", dst="bin", src="@bindirs")
        self.copy("*.dylib*", dst="bin", src="@libdirs")
        self.copy("*.so*", dst="bin", src="@libdirs")

    def test(self):
        if not tools.cross_building(self):
            self.cmake.test(output_on_failure=True)
