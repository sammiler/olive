import os
import sys
import subprocess
import platform
import argparse

class TerminalLauncher:
    """打开终端并设置环境变量"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def get_conanrun_env(self):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        if self.system == "Windows":
            target_dir = os.path.abspath(os.path.join(os.getcwd(), "conan_debug", "build", "Debug", "generators"))
            cmd = f'cmd.exe /c "cd /d {target_dir} && call conanrun.bat && set"'
        elif self.system in ("Linux", "Darwin"):
            target_dir = os.path.abspath(os.path.join(os.getcwd(), "conan_debug", "build", "Debug", "generators"))
            cmd = f"bash -c 'cd {target_dir} && source conanbuild.sh && env'"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding="utf-8")
        for line in result.stdout.splitlines():
            if '=' in line:
                key, value = line.split('=', 1)
                env[key] = value
        return env

    def launch(self, profile):
        """根据参数打开终端并设置环境"""
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
        os.environ['PYTHONIOENCODING'] = 'utf-8'

        env = self.get_conanrun_env() if profile == "full" else os.environ.copy()
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        cmd = [self.shell_path, "-i"]
        process = subprocess.Popen(cmd, env=env, shell=True)
        process.wait()

class CMakeConfigurer:
    """配置 CMake 环境并运行 cmake 命令"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def get_conanrun_env(self, build_type):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        target_dir = os.path.abspath(os.path.join(os.getcwd(), f"conan_{build_type}", "build", build_type, "generators"))
        if self.system == "Windows":
            cmd = f'cmd.exe /c "cd /d {target_dir} && call conanrun.bat && set"'
        elif self.system in ("Linux", "Darwin"):
            cmd = f"bash -c 'cd {target_dir} && source conanbuild.sh && env'"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding="utf-8")
        if result.returncode != 0:
            raise RuntimeError(f"Failed to run conan script: {result.stderr}")
        for line in result.stdout.splitlines():
            if '=' in line:
                key, value = line.split('=', 1)
                env[key] = value
        return env

    def configure(self, build_type, cmake_flags):
        """设置环境变量并运行 cmake"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            msvc_path = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808"
            windows_sdk_path = "C:/Program Files (x86)/Windows Kits/10"
            windows_sdk_version = "10.0.22621.0"
            vcpkg_path = "C:/vcpkg/installed/x64-windows"
            additional_path = "C:/Program Files/CMake/bin;C:/Tools"

            if not os.path.exists(msvc_path):
                raise FileNotFoundError(f"MSVC path not found: {msvc_path}")
            if not os.path.exists(windows_sdk_path):
                raise FileNotFoundError(f"Windows SDK path not found: {windows_sdk_path}")

            env["PATH"] = f"{additional_path};{msvc_path}/bin/Hostx64/x64;{env['PATH']}"
            env["INCLUDE"] = f"{msvc_path}/include;{windows_sdk_path}/Include/{windows_sdk_version}/ucrt;{windows_sdk_path}/Include/{windows_sdk_version}/um;{windows_sdk_path}/Include/{windows_sdk_version}/shared;{vcpkg_path}/include"
            env["LIB"] = f"{msvc_path}/lib/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/ucrt/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/um/x64;{vcpkg_path}/debug/lib"
            # 添加 MSYS_NO_PATHCONV
            env["MSYS_NO_PATHCONV"] = "1"
        elif self.system in ("Linux", "Darwin"):
            env["PATH"] = f"/usr/local/bin:/opt/cmake/bin:{env['PATH']}"

        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        toolchain_file = os.path.join(root_dir, f"conan_{build_type}", "build", build_type, "generators", "conan_toolchain.cmake")
        if not os.path.exists(toolchain_file):
            raise FileNotFoundError(f"Toolchain file not found: {toolchain_file}")

        toolchain_file = toolchain_file.replace("\\", "/")
        runtime_output_dir = os.path.join(root_dir, "build", "bin").replace("\\", "/")
        library_output_dir = os.path.join(root_dir, "build", "lib").replace("\\", "/")
        archive_output_dir = os.path.join(root_dir, "build", "lib").replace("\\", "/")
        install_prefix = os.path.join(root_dir, "install_dir").replace("\\", "/")

        cmake_cmd = [
            "cmake",
            "-S", ".",
            "-B", "build",
            "-G", "Ninja",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            "-DCMAKE_POLICY_DEFAULT_CMP0091=NEW",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
            cmake_flags,  # 直接传递完整字符串
            f"-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={runtime_output_dir}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={library_output_dir}",
            f"-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY={archive_output_dir}",
            f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
        ]

        cmake_cmd_str = " ".join(f'"{arg}"' if " " in arg or "=" in arg else arg for arg in cmake_cmd)
        print(f"CMake command: {cmake_cmd_str}")

        cmd = [self.shell_path, "-c", cmake_cmd_str]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()


class CMakeBuilder:
    """执行 CMake 构建（ninja）"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def get_conanrun_env(self, build_type):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        target_dir = os.path.abspath(os.path.join(os.getcwd(), f"conan_{build_type}", "build", build_type, "generators"))
        if self.system == "Windows":
            cmd = f'cmd.exe /c "cd /d {target_dir} && call conanrun.bat && set"'
        elif self.system in ("Linux", "Darwin"):
            cmd = f"bash -c 'cd {target_dir} && source conanbuild.sh && env'"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding="utf-8")
        if result.returncode != 0:
            raise RuntimeError(f"Failed to run conan script: {result.stderr}")
        for line in result.stdout.splitlines():
            if '=' in line:
                key, value = line.split('=', 1)
                env[key] = value
        return env

    def build(self, build_type):
        """设置环境变量并运行 ninja 构建"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            msvc_path = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808"
            windows_sdk_path = "C:/Program Files (x86)/Windows Kits/10"
            windows_sdk_version = "10.0.22621.0"
            vcpkg_path = "C:/vcpkg/installed/x64-windows"
            additional_path = "C:/Program Files/CMake/bin;C:/Tools"

            if not os.path.exists(msvc_path):
                raise FileNotFoundError(f"MSVC path not found: {msvc_path}")
            if not os.path.exists(windows_sdk_path):
                raise FileNotFoundError(f"Windows SDK path not found: {windows_sdk_path}")

            env["PATH"] = f"{additional_path};{msvc_path}/bin/Hostx64/x64;{env['PATH']}"
            env["INCLUDE"] = f"{msvc_path}/include;{windows_sdk_path}/Include/{windows_sdk_version}/ucrt;{windows_sdk_path}/Include/{windows_sdk_version}/um;{windows_sdk_path}/Include/{windows_sdk_version}/shared;{vcpkg_path}/include"
            env["LIB"] = f"{msvc_path}/lib/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/ucrt/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/um/x64;{vcpkg_path}/debug/lib"
            env["MSYS_NO_PATHCONV"] = "1"
        elif self.system in ("Linux", "Darwin"):
            env["PATH"] = f"/usr/local/bin:/opt/cmake/bin:{env['PATH']}"

        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir
        print(f"Ninja command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()


class CMakeInstaller:
    """执行 CMake 安装（ninja install）"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def get_conanrun_env(self, build_type):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        target_dir = os.path.abspath(os.path.join(os.getcwd(), f"conan_{build_type}", "build", build_type, "generators"))
        if self.system == "Windows":
            cmd = f'cmd.exe /c "cd /d {target_dir} && call conanrun.bat && set"'
        elif self.system in ("Linux", "Darwin"):
            cmd = f"bash -c 'cd {target_dir} && source conanbuild.sh && env'"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding="utf-8")
        if result.returncode != 0:
            raise RuntimeError(f"Failed to run conan script: {result.stderr}")
        for line in result.stdout.splitlines():
            if '=' in line:
                key, value = line.split('=', 1)
                env[key] = value
        return env

    def install(self, build_type):
        """设置环境变量并运行 ninja install"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            env["MSYS_NO_PATHCONV"] = "1"
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir + " install"
        print(f"Ninja install command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()

class CMakeCleaner:
    """执行 CMake 清理（ninja clean）"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def get_conanrun_env(self, build_type):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        target_dir = os.path.abspath(os.path.join(os.getcwd(), f"conan_{build_type}", "build", build_type, "generators"))
        if self.system == "Windows":
            cmd = f'cmd.exe /c "cd /d {target_dir} && call conanrun.bat && set"'
        elif self.system in ("Linux", "Darwin"):
            cmd = f"bash -c 'cd {target_dir} && source conanbuild.sh && env'"
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True, encoding="utf-8")
        if result.returncode != 0:
            raise RuntimeError(f"Failed to run conan script: {result.stderr}")
        for line in result.stdout.splitlines():
            if '=' in line:
                key, value = line.split('=', 1)
                env[key] = value
        return env

    def clean(self, build_type):
        """设置环境变量并运行 ninja clean"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            env["MSYS_NO_PATHCONV"] = "1"
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir + " -t clean"
        print(f"Ninja clean command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()


class ConanConfigurer:
    """执行 Conan 配置"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:\\Program Files\\Git\\bin\\bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

    def configure(self):
        """运行 conan install 生成 Conan 文件"""
        env = os.environ.copy()
        if self.system == "Windows":
            env["MSYS_NO_PATHCONV"] = "1"
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        # 切换到 .vscode/py-script 目录
        root_dir = os.getcwd()
        conan_dir = os.path.join(root_dir, ".vscode", "py-script").replace("\\", "/")
        output_dir = os.path.join(root_dir, "conan_debug").replace("\\", "/")

        conan_cmd = (
            "conan install . "
            f"-of={output_dir} "
            "--build=missing "
            "-s build_type=Debug "
            "-s arch=x86_64 "
            "-s compiler=msvc "
            "-s compiler.version=194"
        )
        print(f"Conan command: {conan_cmd}")

        cmd = [self.shell_path, "-c", conan_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=conan_dir, shell=True)
        process.wait()

def main():
    parser = argparse.ArgumentParser(description="Terminal Manager for Multiple Platforms")
    parser.add_argument("--task", choices=["launch-terminal", "cmake-configure", "cmake-build","cmake-install","cmake-clean","generate_conan"], required=True, help="Task to execute")
    parser.add_argument("--param", help="Parameter for launch-terminal")
    parser.add_argument("--build-type", default="Debug", help="Build type for cmake-configure")
    parser.add_argument("--cmake-flags", default="-DCMAKE_CXX_FLAGS=\"/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2\" -DCMAKE_C_FLAGS=\"/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2\"", help="CMake flags")

    args = parser.parse_args()

    if args.task == "launch-terminal":
        if not args.param:
            raise ValueError("launch-terminal requires --param")
        launcher = TerminalLauncher()
        launcher.launch(args.param)
    elif args.task == "cmake-configure":
        configurer = CMakeConfigurer()
        configurer.configure(args.build_type, args.cmake_flags)
    elif args.task == "cmake-build":
        builder = CMakeBuilder()
        builder.build(args.build_type)
    elif args.task == "cmake-install":
        installer = CMakeInstaller()
        installer.install(args.build_type)
    elif args.task == "cmake-clean":
        cleaner = CMakeCleaner()
        cleaner.clean(args.build_type)
    elif args.task == "generate_conan":
        conan_configurer = ConanConfigurer()
        conan_configurer.configure()

if __name__ == "__main__":
    main()