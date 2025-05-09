import json
import os
import pathlib
import sys

# --- 可配置常量 ---
VERSION = 6
CMAKE_MIN_MAJOR = 3
CMAKE_MIN_MINOR = 25 # 虽然 launcher 是 3.19+，但保持较高版本要求通常没问题
CMAKE_MIN_PATCH = 0

DEFAULT_BINARY_DIR_SUFFIX = "build"
DEFAULT_RUNTIME_OUTPUT_DIR_SUFFIX = "build/bin"
DEFAULT_BUILD_JOBS = 8
ENABLE_CLANG_FORMAT_CACHE_VAR = "ON"

INTERNAL_NAME_FOR_CLANG_FORMAT_TARGET = "clang-format"
INTERNAL_NAME_FOR_CLANG_FORMAT_CHECK_TARGET = "clang-format-check"

CMAKE_VAR_LINKER = "CMAKE_LINKER"
CMAKE_VAR_RC_COMPILER = "CMAKE_RC_COMPILER"
CMAKE_VAR_MT_COMPILER = "CMAKE_MT"

FALLBACK_WINDOWS_LINKER_VALUE = "lld-link"
FALLBACK_WINDOWS_RC_COMPILER_VALUE = "rc"
FALLBACK_WINDOWS_MT_COMPILER_VALUE = "mt"

DEFAULT_TEST_TIMEOUT = 30
VALGRIND_TEST_TIMEOUT_MULTIPLIER = 5 # Valgrind 可能需要更长时间

class PresetGenerator:
    def __init__(self, project_dir_str: str, template_path_str: str):
        self.project_dir = pathlib.Path(project_dir_str)
        self.template_path = pathlib.Path(template_path_str)

        self.template_data = self._load_template()
        if not self.template_data:
            sys.exit(1)

        self.presets = {}

    def _load_template(self):
        if not self.template_path.is_file():
            print(f"错误：模板文件未在路径 {self.template_path} 找到")
            sys.exit(1)
        try:
            with open(self.template_path, 'r', encoding='utf-8') as f:
                return json.load(f)
        except json.JSONDecodeError as e:
            print(f"错误：解析模板文件 {self.template_path} 中的 JSON 失败: {e}")
            sys.exit(1)
        except Exception as e:
            print(f"错误：加载模板文件 {self.template_path} 失败: {e}")
            sys.exit(1)
        return None

    def add_version_info(self):
        self.presets["version"] = VERSION
        self.presets["cmakeMinimumRequired"] = {
            "major": CMAKE_MIN_MAJOR,
            "minor": CMAKE_MIN_MINOR,
            "patch": CMAKE_MIN_PATCH
        }

    def add_configure_presets(self):
        self.presets["configurePresets"] = []
        for platform_spec in self.template_data.get("platform", []):
            os_name_template = platform_spec.get("os")
            if not os_name_template:
                print(f"警告：因缺少 'os' 字段跳过平台配置: {platform_spec}")
                continue

            os_preset_name_part = os_name_template.lower()
            display_os_name = os_name_template
            if os_name_template == "Darwin":
                os_preset_name_part = "mac"
                display_os_name = "macOS"

            base_preset_name = f"{os_preset_name_part}-base"

            base_cache_vars = {
                "CMAKE_C_COMPILER": platform_spec.get("C_COMPILER"),
                "CMAKE_CXX_COMPILER": platform_spec.get("CXX_COMPILER"),
                "CMAKE_CXX_STANDARD": str(platform_spec.get("CMAKE_CXX_STANDARD", "")),
                "CMAKE_TOOLCHAIN_FILE": platform_spec.get("toolchain"),
                "VCPKG_TARGET_TRIPLET": platform_spec.get("triplet"),
                "ENABLE_CLANG_FORMAT": ENABLE_CLANG_FORMAT_CACHE_VAR
            }

            if os_name_template == "Windows":
                linker_val = platform_spec.get("LINK")
                base_cache_vars[CMAKE_VAR_LINKER] = linker_val if linker_val else FALLBACK_WINDOWS_LINKER_VALUE
                rc_val = platform_spec.get("RC")
                base_cache_vars[CMAKE_VAR_RC_COMPILER] = rc_val if rc_val else FALLBACK_WINDOWS_RC_COMPILER_VALUE
                mt_val = platform_spec.get("MT")
                base_cache_vars[CMAKE_VAR_MT_COMPILER] = mt_val if mt_val else FALLBACK_WINDOWS_MT_COMPILER_VALUE

            env_path_list = []
            path_separator = ""
            if os_name_template == "Windows":
                env_path_list = platform_spec.get("envPath", [])
                path_separator = ";"
            elif os_name_template == "Linux":
                env_path_list = platform_spec.get("linuxEnvPath", [])
                path_separator = ":"
            elif os_name_template == "Darwin":
                env_path_list = platform_spec.get("macEnvPath", [])
                path_separator = ":"

            env_path_str = path_separator.join(filter(None, env_path_list))

            base_preset = {
                "name": base_preset_name,
                "hidden": True,
                "displayName": f"{display_os_name} Base",
                "description": platform_spec.get("description", f"{display_os_name} 的基础配置"),
                "generator": platform_spec.get("generator", "Ninja"),
                "binaryDir": "${sourceDir}/" + DEFAULT_BINARY_DIR_SUFFIX,
                "cacheVariables": {k: v for k, v in base_cache_vars.items() if v is not None and v != ""},
            }
            if env_path_str:
                base_preset["environment"] = {"PATH": env_path_str}
            self.presets["configurePresets"].append(base_preset)

            for build_type in ["debug", "release"]:
                preset_name = f"{os_preset_name_part}-{build_type}"
                display_build_type = build_type.capitalize()
                flags_key = "debug_flag" if build_type == "debug" else "rel_flag"
                flags = platform_spec.get(flags_key, {})
                configure_preset = {
                    "name": preset_name,
                    "displayName": f"{display_os_name} {display_build_type}",
                    "inherits": base_preset_name,
                    "condition": {"type": "equals", "lhs": "${hostSystemName}", "rhs": os_name_template},
                    "cacheVariables": {
                        "CMAKE_BUILD_TYPE": display_build_type,
                        "CMAKE_CXX_FLAGS": flags.get("CMAKE_CXX_FLAGS"),
                        "CMAKE_C_FLAGS": flags.get("CMAKE_C_FLAGS"),
                        "CMAKE_RUNTIME_OUTPUT_DIRECTORY": "${sourceDir}/" + DEFAULT_RUNTIME_OUTPUT_DIR_SUFFIX
                    }
                }
                configure_preset["cacheVariables"] = {k: v for k,v in configure_preset["cacheVariables"].items() if v is not None}
                self.presets["configurePresets"].append(configure_preset)

    def add_build_presets(self):
        self.presets["buildPresets"] = []
        template_defined_workflows = self.template_data.get("workflows", [])

        for platform_spec in self.template_data.get("platform", []):
            os_name_template = platform_spec.get("os")
            if not os_name_template: continue
            os_preset_name_part = os_name_template.lower()
            display_os_name = os_name_template
            if os_name_template == "Darwin":
                os_preset_name_part = "mac"
                display_os_name = "macOS"

            for build_type_suffix in ["debug", "release"]:
                config_preset_ref_name = f"{os_preset_name_part}-{build_type_suffix}"
                display_build_type_name = build_type_suffix.capitalize()
                self.presets["buildPresets"].append({
                    "name": f"build-{config_preset_ref_name}",
                    "configurePreset": config_preset_ref_name,
                    "jobs": DEFAULT_BUILD_JOBS,
                    "displayName": f"构建项目 ({display_os_name} {display_build_type_name})"
                })
                for wf_spec in template_defined_workflows:
                    wf_name_from_template = wf_spec.get("name")
                    wf_type_from_template = wf_spec.get("type")
                    target_to_build_in_preset = None
                    display_verb_for_preset = ""
                    if wf_type_from_template == "build":
                        if wf_name_from_template == INTERNAL_NAME_FOR_CLANG_FORMAT_TARGET:
                            target_to_build_in_preset = INTERNAL_NAME_FOR_CLANG_FORMAT_TARGET
                            display_verb_for_preset = "格式化"
                        elif wf_name_from_template == INTERNAL_NAME_FOR_CLANG_FORMAT_CHECK_TARGET:
                            target_to_build_in_preset = INTERNAL_NAME_FOR_CLANG_FORMAT_CHECK_TARGET
                            display_verb_for_preset = "检查格式"
                        else:
                            continue
                        if target_to_build_in_preset:
                            self.presets["buildPresets"].append({
                                "name": f"build-{config_preset_ref_name}-{target_to_build_in_preset}",
                                "targets": [target_to_build_in_preset],
                                "configurePreset": config_preset_ref_name,
                                "displayName": f"{display_verb_for_preset}代码 ({display_os_name} {display_build_type_name})"
                            })

    # ... (在 PresetGenerator 类中) ...
    def add_test_presets(self):
        self.presets["testPresets"] = []
        template_defined_workflows = self.template_data.get("workflows", [])
        test_workflows_from_template = [wf for wf in template_defined_workflows if wf.get("type") == "test"]
        if not test_workflows_from_template:
            return

        for platform_spec in self.template_data.get("platform", []):
            os_name_template = platform_spec.get("os")
            if not os_name_template: continue
            os_preset_name_part = os_name_template.lower()
            display_os_name = os_name_template
            if os_name_template == "Darwin":
                os_preset_name_part = "mac"
                display_os_name = "macOS"

            debug_config_name = f"{os_preset_name_part}-debug"

            # 1. Base test preset for this platform/debug config
            base_test_preset_name = f"{os_preset_name_part}-debug-tests"
            base_test_preset = {
                "name": base_test_preset_name,
                "displayName": f"运行测试 ({display_os_name} Debug)",
                "configurePreset": debug_config_name,
                "output": {"outputOnFailure": True, "verbosity": "default"},
                "execution": {"jobs": 1, "timeout": DEFAULT_TEST_TIMEOUT}
            }

            # 尝试从全局或平台特定配置中获取测试过滤器 (例如 label)
            # 这部分逻辑可以根据 template.json 的设计来扩展
            # For example, if template has: "platform": [{"os": "Linux", "test_filter_label": "linux_unit"} ...]
            # Or a global: "global_test_args": {"default_label": "all_tests"}
            # For now, keeping it simple. You can add logic here to populate base_test_preset["filter"]

            self.presets["testPresets"].append(base_test_preset)

            # 2. Specialized test presets if template indicates a launcher
            for wf_spec in test_workflows_from_template:
                wf_args = wf_spec.get("args", {})
                if wf_args.get("use_launcher"): # 检查模板是否指示使用启动器
                    launcher_command_list = wf_args.get("launcher_command") # 从模板获取启动器命令列表

                    if not launcher_command_list or not isinstance(launcher_command_list, list) or not launcher_command_list:
                        print(f"警告: 模板中测试工作流 '{wf_spec.get('name')}' 指定了 use_launcher 但 launcher_command 无效或缺失。")
                        continue

                    # 使用模板中定义的后缀来命名这个带启动器的测试预设
                    suffix = wf_args.get("launcher_test_preset_suffix", "-launcher") # e.g., "-valgrind"
                    launcher_test_preset_name = f"{base_test_preset_name}{suffix}" # e.g., linux-debug-tests-valgrind

                    # 避免为相同的启动器类型重复添加测试预设
                    if any(tp["name"] == launcher_test_preset_name for tp in self.presets.get("testPresets", [])):
                        continue # 如果已经存在同名预设，则跳过

                    # 创建带启动器的测试预设
                    #它可以继承自基础测试预设，然后覆盖/添加 execution.launcher
                    launcher_test_preset = {
                        "name": launcher_test_preset_name,
                        "displayName": f"运行测试 ({launcher_command_list[0]}, {display_os_name} Debug)",
                        "configurePreset": debug_config_name,
                        "inherits": [base_test_preset_name], # 继承基础预设的 output, filter 等
                        "execution": {
                            # "jobs": 1, # launcher 可能会覆盖 jobs 行为，或者jobs由CTest处理
                            # "timeout": DEFAULT_TEST_TIMEOUT * VALGRIND_TEST_TIMEOUT_MULTIPLIER, # 覆盖超时
                            "launcher": launcher_command_list # <<<<<<<<<<<<<<< 正确的字段名和值
                        }
                    }

                    # 如果需要覆盖 jobs 或 timeout，在 execution 中明确设置它们
                    # 例如，如果 base_test_preset 的 execution 已经有 jobs/timeout，
                    # 并且你想为 launcher 特定的预设修改它们：
                    current_execution_config = base_test_preset.get("execution", {}).copy() # 复制基础执行配置
                    current_execution_config["launcher"] = launcher_command_list            # 添加或覆盖 launcher
                    current_execution_config["jobs"] = 1                                    # Valgrind 通常需要单作业
                    current_execution_config["timeout"] = DEFAULT_TEST_TIMEOUT * VALGRIND_TEST_TIMEOUT_MULTIPLIER # 增加超时

                    launcher_test_preset["execution"] = current_execution_config # 更新 execution

                    self.presets["testPresets"].append(launcher_test_preset)

    def add_workflow_presets(self):
        self.presets["workflowPresets"] = []
        template_defined_workflows = self.template_data.get("workflows", [])

        has_clang_format_op = any(wf.get("name") == INTERNAL_NAME_FOR_CLANG_FORMAT_TARGET for wf in template_defined_workflows if wf.get("type") == "build")
        has_clang_format_check_op = any(wf.get("name") == INTERNAL_NAME_FOR_CLANG_FORMAT_CHECK_TARGET for wf in template_defined_workflows if wf.get("type") == "build")

        for platform_spec in self.template_data.get("platform", []):
            os_name_template = platform_spec.get("os")
            if not os_name_template: continue
            os_preset_name_part = os_name_template.lower()
            display_os_name = os_name_template
            if os_name_template == "Darwin":
                os_preset_name_part = "mac"
                display_os_name = "macOS"

            for build_type_suffix in ["debug", "release"]:
                config_preset_ref_name = f"{os_preset_name_part}-{build_type_suffix}"
                display_build_type_name = build_type_suffix.capitalize()

                # Base steps for most workflows (configure, format, check, build)
                base_workflow_steps = [{"type": "configure", "name": config_preset_ref_name}]
                base_workflow_display_parts = ["配置"]

                if has_clang_format_op:
                    base_workflow_steps.append({"type": "build", "name": f"build-{config_preset_ref_name}-{INTERNAL_NAME_FOR_CLANG_FORMAT_TARGET}"})
                    base_workflow_display_parts.append("格式化")
                if has_clang_format_check_op:
                    base_workflow_steps.append({"type": "build", "name": f"build-{config_preset_ref_name}-{INTERNAL_NAME_FOR_CLANG_FORMAT_CHECK_TARGET}"})
                    base_workflow_display_parts.append("检查格式")
                base_workflow_steps.append({"type": "build", "name": f"build-{config_preset_ref_name}"})
                base_workflow_display_parts.append("构建")

                # Create the standard "build-only" or "build-and-format" workflow
                self.presets["workflowPresets"].append({
                    "name": f"{os_preset_name_part}-workflow-{build_type_suffix}", # e.g. linux-workflow-debug
                    "displayName": f"工作流: {display_os_name} {display_build_type_name} ({', '.join(base_workflow_display_parts)})",
                    "steps": base_workflow_steps[:]
                })

                # Create additional workflows based on template's "type": "test" entries
                # These are typically added to the "debug" base workflow
                if build_type_suffix == "debug":
                    for wf_spec in template_defined_workflows:
                        if wf_spec.get("type") == "test":
                            wf_args = wf_spec.get("args", {})
                            template_wf_name = wf_spec.get("name", "Test") # Name from template, e.g., "单元测试并用Valgrind检查内存"

                            # Determine which testPreset to use
                            test_preset_to_ref = f"{os_preset_name_part}-debug-tests" # Default base test preset
                            if wf_args.get("use_launcher"):
                                suffix = wf_args.get("launcher_test_preset_suffix", "-launcher")
                                test_preset_to_ref = f"{test_preset_to_ref}{suffix}" # e.g., linux-debug-tests-valgrind

                            # Check if the target testPreset actually exists
                            if not any(tp["name"] == test_preset_to_ref for tp in self.presets.get("testPresets", [])):
                                print(f"警告: 工作流 '{template_wf_name}' (针对 {display_os_name} Debug) 将跳过测试步骤，因为测试预设 '{test_preset_to_ref}' 未生成。")
                                continue

                            # Create a new workflow that includes these base steps + this specific test step
                            # Name the workflow preset based on the template's workflow name
                            workflow_preset_name = f"{os_preset_name_part}-workflow-debug-{template_wf_name.replace(' ', '-').lower()}"

                            current_test_workflow_steps = base_workflow_steps[:] + [
                                {"type": "test", "name": test_preset_to_ref} # No other args here!
                            ]
                            current_test_workflow_display_parts = base_workflow_display_parts[:] + [template_wf_name]

                            self.presets["workflowPresets"].append({
                                "name": workflow_preset_name,
                                "displayName": f"工作流: {display_os_name} Debug ({', '.join(current_test_workflow_display_parts)})",
                                "steps": current_test_workflow_steps
                            })

    def generate(self):
        self.add_version_info()
        self.add_configure_presets()
        self.add_build_presets()
        # IMPORTANT: Test presets must be generated before workflow presets that might reference them
        self.add_test_presets()
        self.add_workflow_presets()
        self._write_presets()

    def _write_presets(self):
        output_path = self.project_dir / "CMakePresets.json"
        output_path.parent.mkdir(parents=True, exist_ok=True)
        try:
            with open(output_path, 'w', encoding='utf-8') as f:
                json.dump(self.presets, f, indent=2, ensure_ascii=False)
            print(f"已在路径 {output_path} 生成 CMakePresets.json")
        except IOError as e:
            print(f"错误：写入 CMakePresets.json 到 {output_path} 失败: {e}")
            sys.exit(1)

def main():
    project_dir_env = os.environ.get("PROJECT_DIR")
    if not project_dir_env:
        current_script_path = pathlib.Path(__file__).resolve()
        project_dir = current_script_path.parent.parent.parent
        print(f"警告：未设置 PROJECT_DIR 环境变量。: {project_dir}")
    else:
        project_dir = pathlib.Path(project_dir_env)

    template_file_name = "template.json"
    template_path = project_dir / ".project" / "template" / template_file_name

    generator = PresetGenerator(str(project_dir), str(template_path))
    generator.generate()

if __name__ == "__main__":
    main()