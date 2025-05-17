import json
import os
import pathlib
import platform
import sys

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

class CCppPropertiesGenerator(BaseGenerator):
    def generate(self):
        # 从 vscodeConf.json 加载配置
        config = self.load_json("vscodeConf.json")["c_cpp_properties"]
        # 获取当前操作系统的配置
        os_config = config.get(self.current_os, {})

        # 直接构建 c_cpp_properties.json 的数据结构
        c_cpp_properties = {
            "configurations": [
                {
                    "name": os_config.get("os", "Win32"),
                    "includePath": [],
                    "defines": ["_DEBUG"],
                    "intelliSenseMode": os_config.get("mode", "windows-msvc-x64"),
                    "compilerPath": os_config.get("compiler_path", ""),
                    "cppStandard": "c++17",
                    "cStandard": "c11",
                    "configurationProvider": "ms-vscode.cmake-tools",
                    "mergeConfigurations": False
                }
            ],
            "version": 4
        }

        # 保存生成的 JSON 文件
        self.save_json("c_cpp_properties.json", c_cpp_properties)

def main():
    project_dir_env = os.environ.get("PROJECT_DIR")
    if not project_dir_env:
        print(f"警告：未设置 PROJECT_DIR 环境变量.")
        sys.exit(1)
    else:
        project_dir = pathlib.Path(project_dir_env)
    
    generators = [
        CCppPropertiesGenerator(project_dir)
    ]
    for generator in generators:
        generator.generate()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"脚本执行失败: {e}")
        sys.exit(1)