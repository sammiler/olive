import json
import os
import platform
import subprocess
from pathlib import Path
import shlex

# 获取当前环境变量（副本）
global_env = os.environ.copy()

# 如果你自己打算添加变量，比如：
# current_env['MY_VAR'] = '123'

# 打印或保存
# for k, v in global_env.items():
#     print(f"{k}={v}")
def load_presets_data(project_dir_str):

    presets_file = Path(project_dir_str) / "CMakePresets.json"
    try:
        with open(presets_file, 'r', encoding='utf-8') as f:
            return json.load(f)
    except Exception as e:
        return None


def get_current_os_name():
    system = platform.system()
    if system == "Darwin": return "Darwin"
    return system

def is_preset_condition_met(preset, current_os_name, debug_prefix=""):
    preset_name = preset.get('name', "Unnamed")
    # print(f"{debug_prefix}[Cond Check] '{preset_name}' for OS '{current_os_name}'")

    if 'condition' not in preset:
        # print(f"{debug_prefix}  '{preset_name}': No condition field, condition MET.")
        return True

    condition = preset['condition']
    if isinstance(condition, bool):
        # print(f"{debug_prefix}  '{preset_name}': Boolean condition ({condition}), MET: {condition}")
        return condition

    if isinstance(condition, dict):
        cond_type = condition.get('type')
        cond_lhs = condition.get('lhs')
        cond_rhs = condition.get('rhs')

        if cond_type == 'equals' and cond_lhs == '${hostSystemName}':
            met = (cond_rhs == current_os_name)
            # print(f"{debug_prefix}  '{preset_name}': hostSystemName ('{current_os_name}') == '{cond_rhs}'? MET: {met}")
            return met
        if cond_type == 'notEquals' and cond_lhs == '${hostSystemName}':
            met = (cond_rhs != current_os_name)
            # print(f"{debug_prefix}  '{preset_name}': hostSystemName ('{current_os_name}') != '{cond_rhs}'? MET: {met}")
            return met
        if cond_type == 'always':
            # print(f"{debug_prefix}  '{preset_name}': Type 'always', MET: True")
            return True
        if cond_type == 'never':
            # print(f"{debug_prefix}  '{preset_name}': Type 'never', MET: False")
            return False

        # print(f"{debug_prefix}  '{preset_name}': Unhandled condition type/lhs '{cond_type}/{cond_lhs}', assuming NOT MET.")
        return False # Unhandled complex conditions

    # print(f"{debug_prefix}  '{preset_name}': Malformed condition field, assuming NOT MET.")
    return False # Malformed condition

def get_all_presets_by_name(presets_data):
    all_presets = {}
    for preset_type_key in ['configurePresets', 'buildPresets', 'testPresets', 'packagePresets', 'workflowPresets']:
        for preset in presets_data.get(preset_type_key, []):
            if 'name' in preset:
                all_presets[preset['name']] = preset
    return all_presets

def is_preset_visible_and_valid(preset, current_os_name, debug_prefix=""):
    preset_name = preset.get('name', "UnnamedPreset")

    if preset.get('hidden', False):
        # print(f"{debug_prefix}  '{preset_name}' is HIDDEN. Result: False")
        return False

    if not is_preset_condition_met(preset, current_os_name, debug_prefix=f"{debug_prefix}  Via '{preset_name}': "):
        # print(f"{debug_prefix}  '{preset_name}' condition NOT MET. Result: False")
        return False

    # print(f"{debug_prefix}  '{preset_name}' is VISIBLE and VALID. Result: True")
    return True

def get_all_valid_configure_preset_names_for_os(presets_data, current_os_name):
    valid_names = set()
    for preset in presets_data.get('configurePresets', []):
        name = preset.get('name', 'N/A')
        # For this list, a configure preset is "valid" if its own condition is met.
        # It can be hidden and still be a valid base for inheritance.
        if is_preset_condition_met(preset, current_os_name, debug_prefix="  CfgValid Check: "):
            valid_names.add(name)
    return valid_names

def get_dependent_presets(preset_list_key, presets_data, valid_cfg_names, current_os, all_presets_map):
    active_presets = []
    for preset in presets_data.get(preset_list_key, []):
        name = preset.get('name', 'N/A')

        if not is_preset_visible_and_valid(preset, current_os, debug_prefix="    Dep Check: "):
            print(f"    '{name}' self not visible/valid. Skipping.")
            continue

        cfg_name = preset.get('configurePreset')
        if not cfg_name:
            print(f"    '{name}' missing 'configurePreset'. Skipping.")
            continue

        if cfg_name in valid_cfg_names:
            active_presets.append(preset)
    return active_presets

def get_active_workflow_presets(presets_data, valid_cfg_names, current_os, all_presets_map):
    active_workflows = []
    for preset in presets_data.get('workflowPresets', []):
        wf_name = preset.get('name', "N/A")

        if not is_preset_visible_and_valid(preset, current_os, debug_prefix="    WF Self Check: "):
            print(f"    Workflow '{wf_name}' self not visible/valid. Skipping.")
            continue

        steps_ok = True
        for step in preset.get("steps", []):
            step_name = step.get("name")
            step_type = step.get("type")

            step_preset_obj = all_presets_map.get(step_name)
            if not step_preset_obj:
                steps_ok = False; break

            # Check if the step preset itself is visible and its condition is met
            if not is_preset_visible_and_valid(step_preset_obj, current_os, debug_prefix=f"      Step '{step_name}' Check: "):
                steps_ok = False; break

            # Check configurePreset dependency for build/test/package steps
            step_cfg_preset_name = step_preset_obj.get("configurePreset")
            if step_type in ["build", "test", "package"] and (not step_cfg_preset_name or step_cfg_preset_name not in valid_cfg_names):
                print(f"      Step '{step_name}' (type {step_type}) refs invalid/missing configurePreset '{step_cfg_preset_name}'. WF INVALID.")
                steps_ok = False; break

            # For configure steps, the step name itself should be a valid configure preset name
            if step_type == "configure" and step_name not in valid_cfg_names:
                print(f"      Configure Step '{step_name}' is not a valid configure preset name. WF INVALID.")
                steps_ok = False; break

        if steps_ok:
            active_workflows.append(preset)
    return active_workflows

def display_menu_and_get_choice(options_list, prompt_message="请选择一个选项:"):
    # (No changes to this function, assuming it's correct)
    print(f"\n{prompt_message}")
    menu_items = []
    if not options_list: # Handle empty list before trying to access options_list[0]
        print("没有可用选项。")
        return 0, None
    if isinstance(options_list[0], dict) and 'name' in options_list[0]:
        menu_items = [(item.get('displayName', item['name']), item['name']) for item in options_list]
    elif isinstance(options_list[0], str):
        menu_items = [(item, item) for item in options_list]
    else:
        return 0, None
    if not menu_items: # Should be caught by the first check, but as a safeguard
        print("没有可用选项。")
        return 0, None
    for i, (display_name, actual_name) in enumerate(menu_items):
        if display_name != actual_name:
            print(f"  {i + 1}. {display_name} ({actual_name})")
        else:
            print(f"  {i + 1}. {display_name}")
    print(f"  0. 返回/退出")
    while True:
        try:
            choice = int(input("请输入您的选择: "))
            if 0 <= choice <= len(menu_items):
                if choice == 0: return 0, None
                return choice, menu_items[choice - 1][1]
            else: print("无效的选择。请重试。")
        except ValueError: print("无效的输入。请输入数字。")


def main():
    project_dir_env = os.environ.get("PROJECT_DIR")
    project_dir = Path(project_dir_env).resolve() if project_dir_env else Path(".").resolve()
    # print(f"[main] Using project_dir: {project_dir}")

    presets_data = load_presets_data(str(project_dir))
    if not presets_data: return

    current_os = get_current_os_name()

    all_presets_map = get_all_presets_by_name(presets_data)

    valid_cfg_names = get_all_valid_configure_preset_names_for_os(presets_data, current_os)

    active_builds = get_dependent_presets('buildPresets', presets_data, valid_cfg_names, current_os, all_presets_map)
    active_tests = get_dependent_presets('testPresets', presets_data, valid_cfg_names, current_os, all_presets_map)
    active_packages = get_dependent_presets('packagePresets', presets_data, valid_cfg_names, current_os, all_presets_map)
    active_workflows = get_active_workflow_presets(presets_data, valid_cfg_names, current_os, all_presets_map)

    while True:
        main_menu_options = []
        if active_workflows: main_menu_options.append(("执行工作流 (Execute Workflow)", "workflow"))
        if active_builds:
            main_menu_options.append(("执行构建预设 (Execute Build Preset)", "build"))
            if any(bp.get("targets") for bp in active_builds):
                main_menu_options.append(("执行构建目标 (Execute Build Target)", "target"))
        if active_tests: main_menu_options.append(("执行测试预设 (Execute Test Preset)", "test"))
        if active_packages: main_menu_options.append(("执行打包预设 (Execute Package Preset)", "package"))

        if not main_menu_options:
            print("\n对于您当前的操作系统或配置，未找到合适的预设。")
            return

        print("\n请选择 CMake 操作:")
        for i, (display, _) in enumerate(main_menu_options): print(f"  {i + 1}. {display}")
        print(f"  0. 退出 (Exit)")

        try:
            user_main_choice_idx = int(input("请输入您的选择: "))
            if user_main_choice_idx == 0: print("正在退出。"); break
            if 1 <= user_main_choice_idx <= len(main_menu_options):
                selected_action_key = main_menu_options[user_main_choice_idx - 1][1]
            else: print("无效的选择。请重试。"); continue
        except ValueError: print("无效的输入。请输入数字。"); continue

        final_command = None
        chosen_presets_list = []
        prompt_msg = ""

        if selected_action_key == "workflow": chosen_presets_list, prompt_msg = active_workflows, "请选择工作流预设:"
        elif selected_action_key == "build": chosen_presets_list, prompt_msg = active_builds, "请选择构建预设:"
        elif selected_action_key == "target":
            build_presets_with_targets = [bp for bp in active_builds if bp.get("targets")]
            choice_idx, sel_build_name = display_menu_and_get_choice(build_presets_with_targets, "请选择用于目标的构建预设:")
            if choice_idx > 0 and sel_build_name:
                sel_build_obj = next((p for p in build_presets_with_targets if p['name'] == sel_build_name), None)
                if sel_build_obj:
                    targets = sel_build_obj.get("targets", [])
                    target_choice_idx, sel_target_name = display_menu_and_get_choice(targets, f"请为 {sel_build_name} 选择目标:")
                    if target_choice_idx > 0 and sel_target_name:
                        final_command = f"cmake --build --preset {sel_build_name} --target {sel_target_name}"
            if final_command:
                command_parts = shlex.split(final_command)
                subprocess.run(command_parts, check=True,env=global_env)
            else: continue # Go back to main menu
        elif selected_action_key == "test": chosen_presets_list, prompt_msg = active_tests, "请选择测试预设:"
        elif selected_action_key == "package": chosen_presets_list, prompt_msg = active_packages, "请选择打包预设:"

        if chosen_presets_list: # For workflow, build, test, package direct selection
            choice_idx, selected_preset_name = display_menu_and_get_choice(chosen_presets_list, prompt_msg)
            if choice_idx > 0 and selected_preset_name:
                if selected_action_key == "workflow": final_command = f"cmake --workflow --preset {selected_preset_name}"
                elif selected_action_key == "build": final_command = f"cmake --build --preset {selected_preset_name}"
                elif selected_action_key == "test": final_command = f"ctest --preset {selected_preset_name}"
                elif selected_action_key == "package": final_command = f"cpack --preset {selected_preset_name}"

        if final_command:
            command_parts = shlex.split(final_command)
            subprocess.run(command_parts, check=True,env=global_env)

if __name__ == "__main__":


    # To simulate PROJECT_DIR not being set (and thus use embedded JSON):
    if "PROJECT_DIR" in os.environ:
        del os.environ["PROJECT_DIR"]
    main()
