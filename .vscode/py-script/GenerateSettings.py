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

class SettingsGenerator(BaseGenerator):
    def generate(self):
        # Load settings from vscodeConf.json
        config = self.load_json("vscodeConf.json")["settings"]
        os_config = config["os"].get(self.current_os, {})

        # Build settings.json structure
        settings_json = {
            "files.associations": {
                "*.qrc": "xml",
                "**/APP/ts/**/*.ts": "xml",
                "*.ui": "xml"
            },
            "terminal.integrated.defaultProfile.windows": os_config.get("bash", "Git Bash"),
            "C_Cpp.intelliSenseEngine": "disabled",
            "clangd.arguments": [
                "--compile-commands-dir=${workspaceFolder}/build",
                "--clang-tidy",
                "--all-scopes-completion",
                "--completion-style=detailed",
                "--header-insertion=never"
            ],
            "clangd.fallbackFlags": [
                "-I${workspaceFolder}/ext/core/include",
                f"-I{os_config.get('clangdInclude', '')}"
            ],
            "C_Cpp.default.compileCommands": "${workspaceFolder}/build/compile_commands.json",
            "editor.snippetSuggestions": "inline",
            "editor.tabCompletion": "on",
            "openInExternalApp.openMapper": [
                {
                    "extensionName": "ui",
                    "apps": [
                        {
                            "title": "QtUI",
                            "openCommand": os_config.get("qt_exe", "")
                        }
                    ]
                }
            ]
        }

        # Save the generated JSON
        self.save_json("settings.json", settings_json)

def main():
    project_dir_env = os.environ.get("PROJECT_DIR")
    if not project_dir_env:
        print(f"警告：未设置 PROJECT_DIR 环境变量.")
        sys.exit(1)
    else:
        project_dir = pathlib.Path(project_dir_env)
    generators = [
        SettingsGenerator(project_dir)
    ]
    for generator in generators:
        generator.generate()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        print(f"脚本执行失败: {e}")
        sys.exit(1)