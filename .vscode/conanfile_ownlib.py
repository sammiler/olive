from conan import ConanFile
from conan.tools.files import copy
import os

class LibPQConan(ConanFile):
    name = "libpq"
    version = "15.4"  # 可调整为你的版本
    settings = "os", "compiler", "build_type", "arch"
    exports_sources = "bin/*", "include/*", "lib/*"  # 更精确地导出子目录内容

    def package(self):
        # 打包所有头文件，包括子目录
        copy(self, "*.h", src=os.path.join(self.source_folder, "include"), 
             dst=os.path.join(self.package_folder, "include"), keep_path=True)
        
        # 根据 build_type 打包库和动态库
        if self.settings.build_type == "Debug":
            copy(self, "libpq.lib", src=os.path.join(self.source_folder, "lib"), 
                 dst=os.path.join(self.package_folder, "lib/debug"))
            copy(self, "libpq.dll", src=os.path.join(self.source_folder, "bin"), 
                 dst=os.path.join(self.package_folder, "bin/debug"))
        elif self.settings.build_type == "Release":
            copy(self, "libpq.lib", src=os.path.join(self.source_folder, "lib"), 
                 dst=os.path.join(self.package_folder, "lib/release"))
            copy(self, "libpq.dll", src=os.path.join(self.source_folder, "bin"), 
                 dst=os.path.join(self.package_folder, "bin/release"))

    def package_info(self):
        # 设置头文件目录
        self.cpp_info.includedirs = ["include"]
        
        # 根据 build_type 设置库和二进制目录
        if self.settings.build_type == "Debug":
            self.cpp_info.libdirs = ["lib/debug"]
            self.cpp_info.bindirs = ["bin/debug"]
            self.cpp_info.libs = ["libpq"]
        elif self.settings.build_type == "Release":
            self.cpp_info.libdirs = ["lib/release"]
            self.cpp_info.bindirs = ["bin/release"]
            self.cpp_info.libs = ["libpq"]