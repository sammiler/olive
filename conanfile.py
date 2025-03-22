import os
from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files  import patch
from conan.tools.cmake import CMakeToolchain, CMakeDeps
from conan.tools.env import VirtualBuildEnv
from conan.tools.env import VirtualRunEnv
class MyProjectConan(ConanFile):
    name = "myproject"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"

    def layout(self):
        generators = "CMakeDeps", "CMakeToolchain"
        cmake_layout(self,generators,".","build")  # 使用标准的 cmake_layout 函数
    def requirements(self):
        self.requires("ffmpeg/6.1")
        self.requires("openimageio/2.5.18.0")
        self.requires("imath/3.1.9")
        self.requires("opencolorio/2.3.1")
        self.requires("portaudio/19.8.0")
        self.requires("openexr/3.2.3")
        self.requires("libdeflate/1.19")
        # self.requires("minizip-ng/4.0.3")  # 使用合适的版本 
        self.requires("qt/5.15.16")

    def configure(self):
        self.output.info(f"configure : {self.source_folder}")
        self.options["*"].shared = True
        self.options["qt"].gui = True
        self.options["qt"].openssl = True
        self.options["qt"].widgets = True
        self.options["qt"].with_pq = True
        self.options["qt"].with_md4c = True
        self.options["qt"].with_odbc = True
        self.options["qt"].with_zstd = True
        self.options["qt"].with_pcre2 = True
        self.options["qt"].with_libpng = True
        self.options["qt"].with_sqlite3 = True
        self.options["qt"].with_freetype = True
        self.options["qt"].with_doubleconversion = True
        self.options["qt"].qttools = True       # 启用 Qt Tools
        self.options["qt"].qtdeclarative = True # 启用 QML 支持
        self.options["qt"].qtmultimedia = True
        self.options["qt"].qtquickcontrols2 = True
        self.options["qt"].qtxmlpatterns = True
        self.options["minizip-ng"].shared = False  # 强制静态库


    def generate(self):
        tc = CMakeToolchain(self)
        tc.generator = "Ninja"
        tc.cache_variables["CMAKE_CXX_STANDARD"] = "17"
        tc.cache_variables["CMAKE_C_COMPILER"] = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/cl.exe"
        tc.cache_variables["CMAKE_CXX_COMPILER"] = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/cl.exe"
        tc.generate()

        deps = CMakeDeps(self)
        deps.generate()

        # 使用 VirtualRunEnv 设置运行时环境
        env = VirtualRunEnv(self)
        env_vars = env.environment()

        # 你的所有路径
        custom_paths = [
            "C:/Program Files/CMake/bin",
            "C:/Tools",
            "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64",
            "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64",
            "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/lib/x64",
        ]
        a =   env_vars.vars(self)
        self.output.info("1111")
        self.output.info(a.get("PATH"))
        # 遍历并追加所有路径到 PATH
        for path in custom_paths:
            env_vars.prepend_path("PATH", path)  # 使用 append_path 追加到 PATH

        # 生成环境脚本
        env.generate()