import json
import os
import pathlib
import platform
import sys

#TODO 最好修改为Append模式，并处理空文件的情况
class BaseGenerator:
    def __init__(self, dir):
        self.root_dir = dir
        self.template_dir = self.root_dir / ".vscode" / "template"
        self.output_dir = self.root_dir / ".vscode"
        self.system = platform.system()
        self.os_map = {"Windows": "Windows", "Linux": "Linux", "Darwin": "Mac"}
        self.current_os = self.os_map.get(self.system, "Windows")

    def load_json(self, filename):
        file_path = self.template_dir / filename
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)

    def save_json(self, filename, data):
        output_path = self.output_dir / filename
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4, ensure_ascii=False)
        print(f"已生成: {output_path}")

class LaunchGenerator(BaseGenerator):
    def find_executables(self):
        bin_dir = self.root_dir / "build" / "bin"
        exes = []
        ext = ".exe" if self.system == "Windows" else ""
        for file in bin_dir.rglob(f"*{ext}"):
            if file.is_file() and (self.system != "Windows" or file.suffix == ".exe"):
                exes.append({"name": file.name, "path": str(file).replace("\\", "/")})
        return exes

    def generate(self):
        # Load launch configuration from vscodeConf.json
        config = self.load_json("vscodeConf.json")["launch"]
        os_config = config.get(self.current_os, {})

        # Find executables in build/bin
        exes = self.find_executables()

        # Build configurations for each executable
        configurations = []
        for exe in exes:
            config_entry = {
                "name": f"Debug {exe['name']}",
                "program": exe["path"],
                "type": os_config.get("debug_type", "cppvsdbg"),
                "request": "launch",
                "stopAtEntry": True,
                "cwd": "${workspaceFolder}/build/bin",
                "environment": [
                    {
                        "name": "PATH",
                        "value": "${env:PATH};"
                    }
                ],
                "console": "internalConsole",
                "logging": {
                    "moduleLoad": True,
                    "exceptions": True
                },
                "visualizerFile": os_config.get("qt5_natvis", ""),
                "sourceFileMap": {}
            }
            configurations.append(config_entry)

        # Build final launch.json structure
        launch_json = {
            "version": "0.2.0",
            "configurations": configurations
        }

        # Save the generated JSON
        self.save_json("launch.json", launch_json)

def main():
    project_dir_env = os.environ.get("PROJECT_DIR")
    if not project_dir_env:
        print(f"警告：未设置 PROJECT_DIR 环境变量.")
        sys.exit(1)
    else:
        project_dir = pathlib.Path(project_dir_env)
    generators = [
        LaunchGenerator(project_dir)
    ]
    for generator in generators:
        generator.generate()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"脚本执行失败: {e}")
        sys.exit(1)