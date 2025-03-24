import os
import re
import sys
import subprocess
import platform
import argparse
import threading

class BaseTask:
    """基类，包含公共方法和属性"""
    def __init__(self):
        self.system = platform.system()
        self.shell_path = "C:/Program Files/Git/bin/bash.exe" if self.system == "Windows" else "/bin/bash"
        if self.system == "Windows":
            self.compiler_path = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/cl.exe"
            self.linker_path = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808/bin/Hostx64/x64/link.exe"
            self.rc_compiler = "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/rc.exe"
            self.mt = "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/mt.exe"
            self.mc_compiler = "C:/Program Files (x86)/Windows Kits/10/bin/10.0.22621.0/x64/mc.exe"
        elif self.system == "Linux":
            self.compiler_path = "/usr/bin/gcc"
            self.linker_path = "/usr/bin/ld"
            self.rc_compiler = "/usr/bin/windres"  # 仅在 MinGW 可用，可选
            self.mt = "/usr/bin/objcopy"  # 类似的工具
            self.mc_compiler = "/usr/bin/msgfmt"  # 近似工具，用于消息编译
        elif self.system == "Darwin":  # macOS
            self.compiler_path = "/usr/bin/clang"
            self.linker_path = "/usr/bin/ld"
            self.rc_compiler = "/usr/bin/rez"  # macOS 资源编译器
            self.mt = "/usr/bin/install_name_tool"  # macOS 处理二进制依赖的工具
            self.mc_compiler = "/usr/bin/msgfmt"  # 近似工具

        if self.system == "Windows" and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Git Bash not found. Please adjust shell_path.")
        elif self.system in ("Linux", "Darwin") and not os.path.exists(self.shell_path):
            raise FileNotFoundError("Bash not found. Please adjust shell_path.")

# ANSI 转义码着色函数
    def color_line(self,line):
        RED = "\033[91m"
        YELLOW = "\033[93m"
        RESET = "\033[0m"
        # 根据 MSVC 输出格式，可根据需要调整正则匹配
        if re.search(r'\berror\b', line, re.IGNORECASE):
            return RED + line + RESET
        elif re.search(r'\bwarning\b', line, re.IGNORECASE):
            return YELLOW + line + RESET
        return line
    

    def stream_output(self,stream, color_func):
        for line in iter(stream.readline, ""):
            sys.stdout.write(color_func(line))
        stream.close()
    def run_command_with_color(self,cmd, env, cwd):
        """执行命令，实时着色输出"""
        process = subprocess.Popen(
            cmd,
            env=env,
            cwd=cwd,
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
            universal_newlines=True,
            encoding='utf-8',    # 强制使用 UTF-8 编码
            errors='replace',  
            bufsize=1,
            shell=True
        )
        t1 = threading.Thread(target=self.stream_output, args=(process.stdout, self.color_line))
        t2 = threading.Thread(target=self.stream_output, args=(process.stderr, self.color_line))
        t1.start()
        t2.start()
        t1.join()
        t2.join()

        return process.wait()
        

    def get_conanrun_env(self, build_type="Debug"):
        """从 conan 运行脚本获取环境变量（跨平台）"""
        env = os.environ.copy()
        target_dir = os.path.abspath(os.path.join(os.getcwd(), f"conan_{build_type}", "build", build_type, "generators"))
        if not os.path.exists(target_dir):
            return env
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

    def setup_windows_env(self, env):
        """设置 Windows 特定的环境变量"""
        msvc_path = "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/VC/Tools/MSVC/14.43.34808"
        windows_sdk_path = "C:/Program Files (x86)/Windows Kits/10"
        windows_sdk_version = "10.0.22621.0"
        additional_path = "C:/Program Files/CMake/bin;C:/Tools"

        if not os.path.exists(msvc_path):
            raise FileNotFoundError(f"MSVC path not found: {msvc_path}")
        if not os.path.exists(windows_sdk_path):
            raise FileNotFoundError(f"Windows SDK path not found: {windows_sdk_path}")

        env["PATH"] = f"{additional_path};{msvc_path}/bin/Hostx64/x64;{env['PATH']}"
        env["INCLUDE"] = f"{msvc_path}/include;{windows_sdk_path}/Include/{windows_sdk_version}/ucrt;{windows_sdk_path}/Include/{windows_sdk_version}/um;{windows_sdk_path}/Include/{windows_sdk_version}/shared;"
        env["LIB"] = f"{msvc_path}/lib/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/ucrt/x64;{windows_sdk_path}/Lib/{windows_sdk_version}/um/x64;"
        env["MSYS_NO_PATHCONV"] = "1"


class TerminalLauncher(BaseTask):
    """打开终端并设置环境变量"""
    def launch(self, profile):
        """根据参数打开终端并设置环境"""
        sys.stdout.reconfigure(encoding='utf-8')
        sys.stderr.reconfigure(encoding='utf-8')
        os.environ['PYTHONIOENCODING'] = 'utf-8'
        if self.system == "Windows":
            self.setup_windows_env(env)
        env = self.get_conanrun_env() if profile == "full" else os.environ.copy()
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        cmd = [self.shell_path, "-i"]
        process = subprocess.Popen(cmd, env=env, shell=True)
        process.wait()

class CMakeConfigurer(BaseTask):
    """配置 CMake 环境并运行 cmake 命令"""
    def configure(self, build_type, cmake_flags):
        """设置环境变量并运行 cmake"""
        env = self.get_conanrun_env(build_type)
        if self.system == "Windows":
            print("Windows")
            self.setup_windows_env(env)
        elif self.system in ("Linux", "Darwin"):
            env["PATH"] = f"/usr/local/bin:/opt/cmake/bin:{env['PATH']}"

        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        runtime_output_dir = os.path.join(root_dir, "build", "bin").replace("\\", "/")
        library_output_dir = os.path.join(root_dir, "build", "lib").replace("\\", "/")
        archive_output_dir = os.path.join(root_dir, "build", "lib").replace("\\", "/")
        install_prefix = os.path.join(root_dir, "install_dir").replace("\\", "/")
        toolchain_file = os.path.join(root_dir, f"conan_{build_type}", "build", build_type, "generators", "conan_toolchain.cmake")
        cmake_cmd = []
        if  os.path.exists(toolchain_file):
            toolchain_file = toolchain_file.replace("\\", "/")
            cmake_cmd = [
            "cmake",
            "-S", ".",
            "-B", "build",
            "-G", "Ninja",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            "-DCMAKE_POLICY_DEFAULT_CMP0091=NEW",
            f"-DCMAKE_CXX_COMPILER={self.compiler_path}",
            f"-DCMAKE_C_COMPILER={self.compiler_path}",
            f"-DCMAKE_LINKER={self.linker_path}",
            f"-DCMAKE_RC_COMPILER={self.rc_compiler}",
            f"-DCMAKE_MT={self.mt}",
            f"-DCMAKE_MC_COMPILER={self.mc_compiler}",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DCMAKE_TOOLCHAIN_FILE={toolchain_file}",
            cmake_flags,
            f"-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={runtime_output_dir}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={library_output_dir}",
            f"-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY={archive_output_dir}",
            f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
            ]
        else:
            cmake_cmd = [
            "cmake",
            "-S", ".",
            "-B", "build",
            "-G", "Ninja",
            "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON",
            "-DCMAKE_POLICY_DEFAULT_CMP0091=NEW",
            f"-DCMAKE_BUILD_TYPE={build_type}",
            f"-DCMAKE_CXX_COMPILER={self.compiler_path}",
            f"-DCMAKE_C_COMPILER={self.compiler_path}",
            f"-DCMAKE_LINKER={self.linker_path}",
            f"-DCMAKE_RC_COMPILER={self.rc_compiler}",
            f"-DCMAKE_MT={self.mt}",
            f"-DCMAKE_MC_COMPILER={self.mc_compiler}",
            cmake_flags,
            f"-DCMAKE_RUNTIME_OUTPUT_DIRECTORY={runtime_output_dir}",
            f"-DCMAKE_LIBRARY_OUTPUT_DIRECTORY={library_output_dir}",
            f"-DCMAKE_ARCHIVE_OUTPUT_DIRECTORY={archive_output_dir}",
            f"-DCMAKE_INSTALL_PREFIX={install_prefix}"
            ]
        cmake_cmd_str = " ".join(f'"{arg}"' if " " in arg or "=" in arg else arg for arg in cmake_cmd)
        print(f"CMake command: {cmake_cmd_str}")

        cmd = [self.shell_path, "-c", cmake_cmd_str]
        retcode =  self.run_command_with_color(cmd,env,root_dir)
        if retcode != 0:
            print(f"Configured failed with exit code: {retcode}")
        else:
            print("Configure finished successfully.")

class CMakeBuilder(BaseTask):
    """执行 CMake 构建（ninja）"""
    def build(self, build_type):
        """设置环境变量并运行 ninja 构建"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            self.setup_windows_env(env)
        elif self.system in ("Linux", "Darwin"):
            env["PATH"] = f"/usr/local/bin:/opt/cmake/bin:{env['PATH']}"

        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir
        print(f"Ninja command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        retcode = self.run_command_with_color(cmd, env, root_dir)
        if retcode != 0:
            print(f"Build failed with exit code: {retcode}")
        else:
            print("Build finished successfully.")


    
class CMakeInstaller(BaseTask):
    """执行 CMake 安装（ninja install）"""
    def install(self, build_type):
        """设置环境变量并运行 ninja install"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            self.setup_windows_env(env)
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir + " install"
        print(f"Ninja install command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()

class CMakeCleaner(BaseTask):
    """执行 CMake 清理（ninja clean）"""
    def clean(self, build_type):
        """设置环境变量并运行 ninja clean"""
        env = self.get_conanrun_env(build_type)

        if self.system == "Windows":
            self.setup_windows_env(env)
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

        root_dir = os.getcwd()
        build_dir = os.path.join(root_dir, "build").replace("\\", "/")

        ninja_cmd = "ninja -C " + build_dir + " -t clean"
        print(f"Ninja clean command: {ninja_cmd}")

        cmd = [self.shell_path, "-c", ninja_cmd]
        process = subprocess.Popen(cmd, env=env, cwd=root_dir, shell=True)
        process.wait()

class ConanConfigurer(BaseTask):
    """执行 Conan 配置"""
    def configure(self):
        """运行 conan install 生成 Conan 文件"""
        env = os.environ.copy()
        if self.system == "Windows":
            self.setup_windows_env(env)
        env["LC_ALL"] = "en_US.UTF-8"
        env["LANG"] = "en_US.UTF-8"

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
    parser.add_argument("--task", choices=["launch-terminal", "cmake-configure", "cmake-build", "cmake-install", "cmake-clean", "generate_conan"], required=True, help="Task to execute")
    parser.add_argument("--param", help="Parameter for launch-terminal")
    parser.add_argument("--build-type", default="Debug", help="Build type for cmake-configure")
    parser.add_argument(
        "--cmake-flags",
        default='-DCMAKE_CXX_FLAGS="/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2" -DCMAKE_C_FLAGS="/EHsc /W3 /MP6 /Zi /FS /MDd /D_ITERATOR_DEBUG_LEVEL=2"',
        help="CMake flags"
    )

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