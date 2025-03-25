import json
import platform
import re
from pathlib import Path

class BaseGenerator:
    def __init__(self):
        self.root_dir = Path(__file__).resolve().parents[3]
        self.template_dir = self.root_dir / ".vscode" / "template"
        self.output_dir = self.root_dir / ".vscode"
        self.system = platform.system()
        self.os_map = {"Windows": "Windows", "Linux": "Linux", "Darwin": "Mac"}
        self.current_os = self.os_map.get(self.system, "Windows")

    def load_json(self, filename):
        file_path = self.template_dir / filename
        with open(file_path, 'r', encoding='utf-8') as f:
            return json.load(f)

    def load_template(self, filename):
        file_path = self.template_dir / filename
        with open(file_path, 'r', encoding='utf-8') as f:
            return f.read()

    def save_json(self, filename, data):
        output_path = self.output_dir / filename
        output_path.parent.mkdir(parents=True, exist_ok=True)
        with open(output_path, 'w', encoding='utf-8') as f:
            json.dump(data, f, indent=4, ensure_ascii=False)
        print(f"已生成: {output_path}")

class SettingsGenerator(BaseGenerator):
    def get_conan_path(self):
        conan_file = self.root_dir / "conan_debug" / "build" / "Debug" / "generators" / "conanrunenv-debug-x86_64.bat"
        try:
            with open(conan_file, 'r', encoding='utf-8') as f:
                for line in f:
                    if line.startswith('set "PATH='):
                        path = re.search(r'set "PATH=(.+?);%PATH%"', line).group(1)
                        return path.replace("\\", "/")
        except FileNotFoundError:
            print(f"警告: 未找到 {conan_file}，conan_path 将为空")
            return ""
        return ""

    def generate(self):
        config = self.load_json("template.json")["settings"]
        template = self.load_template("settings.json.in")
        replacements = config["os"].get(self.current_os, {})
        dynamic = config.get("dynamic", [])
        
        if "conan_path" in dynamic:
            replacements["conan_path"] = self.get_conan_path()

        for key, value in replacements.items():
            template = template.replace(f"${{{key}}}", str(value))
        
        self.save_json("settings.json", json.loads(template))

class TasksGenerator(BaseGenerator):
    def generate(self):
        config = self.load_json("template.json")["tasks"]
        template = self.load_template("tasks.json.in")
        replacements = config.get(self.current_os, {})
        
        for key, value in replacements.items():
            template = template.replace(f"${{{key}}}", str(value))
        
        self.save_json("tasks.json", json.loads(template))

class CCppPropertiesGenerator(BaseGenerator):
    def generate(self):
        config = self.load_json("template.json")["c_cpp_properties"]
        template = self.load_template("c_cpp_properties.json.in")
        replacements = config.get(self.current_os, {})
        
        for key, value in replacements.items():
            template = template.replace(f"${{{key}}}", str(value))
        
        self.save_json("c_cpp_properties.json", json.loads(template))

class LaunchGenerator(BaseGenerator):
    def get_conan_path(self):
        conan_file = self.root_dir / "conan_debug" / "build" / "Debug" / "generators" / "conanrunenv-debug-x86_64.bat"
        try:
            with open(conan_file, 'r', encoding='utf-8') as f:
                for line in f:
                    if line.startswith('set "PATH='):
                        path = re.search(r'set "PATH=(.+?);%PATH%"', line).group(1)
                        return path.replace("\\", "/")
        except FileNotFoundError:
            print(f"警告: 未找到 {conan_file}，conan_path 将为空")
            return ""
        return ""

    def find_executables(self):
        bin_dir = self.root_dir / "build" / "bin"
        exes = []
        ext = ".exe" if self.system == "Windows" else ""
        for file in bin_dir.rglob(f"*{ext}"):
            if file.is_file() and (self.system != "Windows" or file.suffix == ".exe"):
                exes.append({"name": file.name, "path": str(file).replace("\\", "/")})
        return exes

    def generate(self):
        config = self.load_json("template.json")["launch"]
        template = self.load_json("launch.json.in")
        replacements = config.get(self.current_os, {})
        dynamic = config.get("dynamic", [])
        
        if "conan_path" in dynamic:
            replacements["conan_path"] = self.get_conan_path()

        exes = self.find_executables()
        configurations = []
        for exe in exes:
            config_str = self.load_template("launch.json.in").replace("[]", "[{}]")
            exe_replacements = replacements.copy()
            exe_replacements["exe_name"] = exe["name"]
            exe_replacements["exe_path"] = exe["path"]
            for key, value in exe_replacements.items():
                config_str = config_str.replace(f"${{{key}}}", str(value))
            config_data = json.loads(config_str)
            configurations.extend(config_data["configurations"])

        template["configurations"] = configurations
        self.save_json("launch.json", template)

def main():
    generators = [
        SettingsGenerator(),
        # TasksGenerator(),
        # CCppPropertiesGenerator(),
        # LaunchGenerator()
    ]
    for generator in generators:
        generator.generate()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"脚本执行失败: {e}")
        exit(1)