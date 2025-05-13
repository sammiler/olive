
if (ENABLE_CLANG_FORMAT)
    # Formatting may change with different versions of clang-format.
    # Test new versions before changing the allowed version here to avoid
    # accidental broad changes to formatting.
    find_program(CLANGFORMAT_EXECUTABLE
            NAMES clang-format-14 clang-format
            DOC "Path to clang-format executable")
    if (CLANGFORMAT_EXECUTABLE)
        message(STATUS "Found clang-format: ${CLANGFORMAT_EXECUTABLE}")

        # Gather all files to format recursively
        file(GLOB_RECURSE FORMAT_FILES
                "${CMAKE_SOURCE_DIR}/app/*.h"
                "${CMAKE_SOURCE_DIR}/app/*.cpp"
                "${CMAKE_SOURCE_DIR}/ext/*.cpp"
                "${CMAKE_SOURCE_DIR}/ext/*.h"
        )

        # Check if files were found
        list(LENGTH FORMAT_FILES FILE_COUNT)
        if (FILE_COUNT EQUAL 0)
            message(WARNING "No files found for clang-format. Check file patterns or directories.")
        else ()
            # Debug: Print number of files found
            message(STATUS "Found ${FILE_COUNT} files for formatting")

            # Define batch size (adjust as needed)
            set(BATCH_SIZE 50)
            # Calculate number of batches
            math(EXPR BATCH_COUNT "(${FILE_COUNT} + ${BATCH_SIZE} - 1) / ${BATCH_SIZE}")
            math(EXPR BATCH_COUNT "${BATCH_COUNT} - 1") # Adjust for 0-based indexing

            # Create clang-format target with batched commands
            set(CLANG_FORMAT_TARGETS)
            foreach (i RANGE 0 ${BATCH_COUNT})
                math(EXPR START "${i} * ${BATCH_SIZE}")
                # Ensure we don't exceed the list length
                math(EXPR REMAINING_FILES "${FILE_COUNT} - ${START}")
                if (REMAINING_FILES GREATER 0)
                    list(SUBLIST FORMAT_FILES ${START} ${BATCH_SIZE} BATCH_FILES)
                    add_custom_command(
                            OUTPUT clang-format-batch-${i}
                            COMMAND ${CLANGFORMAT_EXECUTABLE} -style=file -i ${BATCH_FILES}
                            COMMENT "Formatting batch ${i} of ${BATCH_COUNT}"
                            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    )
                    list(APPEND CLANG_FORMAT_TARGETS clang-format-batch-${i})
                endif ()
            endforeach ()
            add_custom_target(clang-format DEPENDS ${CLANG_FORMAT_TARGETS})

            # Create clang-format-check target with batched commands
            if (ENABLE_CLANG_FORMAT_CHECK)
                set(CLANG_FORMAT_CHECK_TARGETS)
                foreach (i RANGE 0 ${BATCH_COUNT})
                    math(EXPR START "${i} * ${BATCH_SIZE}")
                    # Ensure we don't exceed the list length
                    math(EXPR REMAINING_FILES "${FILE_COUNT} - ${START}")
                    if (REMAINING_FILES GREATER 0)
                        list(SUBLIST FORMAT_FILES ${START} ${BATCH_SIZE} BATCH_FILES)
                        add_custom_command(
                                OUTPUT clang-format-check-batch-${i}
                                COMMAND ${CLANGFORMAT_EXECUTABLE} --dry-run --Werror -style=file -i ${BATCH_FILES}
                                COMMENT "Checking format batch ${i} of ${BATCH_COUNT}"
                                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                        )
                        list(APPEND CLANG_FORMAT_CHECK_TARGETS clang-format-check-batch-${i})
                    endif ()
                endforeach ()
                add_custom_target(clang-format-check DEPENDS ${CLANG_FORMAT_CHECK_TARGETS})
            endif ()
        endif ()
    else ()
        message(FATAL_ERROR "clang-format executable not found, but ENABLE_CLANG_FORMAT is ON. "
                "Please install clang-format or set ENABLE_CLANG_FORMAT to OFF.")
    endif ()
endif ()

# 首先，定义一个选项来控制是否启用 clang-tidy 的修复导出功能
option(ENABLE_CLANG_TIDY_FIX_EXPORT "Enable clang-tidy --export-fixes target" OFF) # 默认关闭

if (ENABLE_CLANG_TIDY_FIX_EXPORT)
    # 1. 找到 clang-tidy 可执行文件
    find_program(CLANG_TIDY_EXECUTABLE
            NAMES clang-tidy # 或者 clang-tidy-18, clang-tidy-17 等，根据你的环境
            HINTS ENV PATH # 确保能找到 vcpkg 提供的版本 (如果工具链配置了 PATH)
            "${LLVM_TOOLS_BINARY_DIR}" # 如果你通过 find_package(LLVM) 得到了这个目录
            DOC "Path to clang-tidy executable")

    if (CLANG_TIDY_EXECUTABLE)
        message(STATUS "Found clang-tidy for fix export: ${CLANG_TIDY_EXECUTABLE}")

        # 先收集 app 目录的文件
        file(GLOB_RECURSE APP_FILES
                "${CMAKE_SOURCE_DIR}/app/*.cpp"
        )
        set(TIDY_TARGET_FILES ${APP_FILES})

        # 再分别收集 ext/ 下你确实想要检查的子目录
        file(GLOB_RECURSE EXT_CORE_FILES_CPP
                "${CMAKE_SOURCE_DIR}/ext/core/*.cpp" # 假设你只想检查 ext/core
                "${CMAKE_SOURCE_DIR}/ext/KDDockWidgets/src/*.cpp"

        )

        list(APPEND TIDY_TARGET_FILES ${EXT_CORE_FILES_CPP})

        # 如果 ext/ 下还有其他想检查的目录，也类似地分别添加
        # file(GLOB_RECURSE EXT_ANOTHERMODULE_FILES_CPP ... )
        # list(APPEND TIDY_TARGET_FILES ${EXT_ANOTHERMODULE_FILES_CPP} ...)
        list(LENGTH TIDY_TARGET_FILES TIDY_FILE_COUNT)
        if (TIDY_FILE_COUNT EQUAL 0)
            message(WARNING "No files found for clang-tidy fix export. Check file patterns or directories.")
        else ()
            message(STATUS "Found ${TIDY_FILE_COUNT} files for clang-tidy fix export.")

            # 3. 定义 YAML 文件输出目录，并确保它存在
            set(CLANG_TIDY_YAML_OUTPUT_DIR "${CMAKE_BINARY_DIR}/clang_tidy_fixes_yaml")

            # 4. 分批处理逻辑 (与你的 clang-format 逻辑类似)
            set(BATCH_SIZE 50) # 每批处理的文件数，可以调整
            math(EXPR BATCH_COUNT "(${TIDY_FILE_COUNT} + ${BATCH_SIZE} - 1) / ${BATCH_SIZE}") # 总批次数
            math(EXPR MAX_BATCH_INDEX "${BATCH_COUNT} - 1") # 0-based 最大索引

            set(ALL_BATCH_YAML_FILES) # 存储所有生成的 YAML 文件名

            # 确保 compile_commands.json 是存在的，作为依赖
            set(COMPILE_COMMANDS_JSON "${CMAKE_BINARY_DIR}/compile_commands.json")
            if (NOT EXISTS "${COMPILE_COMMANDS_JSON}")
                message(WARNING "compile_commands.json not found at ${COMPILE_COMMANDS_JSON}. Clang-tidy might not work correctly.")
            endif ()

            # 5. 为每一批文件创建自定义命令来生成 YAML 修复文件
            foreach (i RANGE 0 ${MAX_BATCH_INDEX})
                math(EXPR START_INDEX "${i} * ${BATCH_SIZE}")
                list(SUBLIST TIDY_TARGET_FILES ${START_INDEX} ${BATCH_SIZE} BATCH_FILES_CURRENT_LIST)

                if (BATCH_FILES_CURRENT_LIST) # 确保列表不为空
                    set(BATCH_YAML_FILE "${CLANG_TIDY_YAML_OUTPUT_DIR}/batch-${i}-fixes.yaml")
                    add_custom_command(
                            OUTPUT "${BATCH_YAML_FILE}"
                            # COMMANDS 会按顺序执行
                            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CLANG_TIDY_YAML_OUTPUT_DIR}" # 确保输出目录存在
                            COMMAND "${CMAKE_COMMAND}" -E rm -f "${BATCH_YAML_FILE}" # 先删除旧的批处理YAML文件
                            COMMAND "${CLANG_TIDY_EXECUTABLE}"
                            -p "${CMAKE_BINARY_DIR}" # 指向 compile_commands.json 所在目录
                            "--export-fixes=${BATCH_YAML_FILE}"
                            # 你可以在这里添加其他 clang-tidy 参数，比如：
                            # "-quiet" # 减少输出
                            # "-header-filter=^${CMAKE_SOURCE_DIR}/(app|ext)/.*" # 如果不想完全依赖 .clang-tidy 文件中的设置
                            ${BATCH_FILES_CURRENT_LIST} # 当前批次的文件列表
                            DEPENDS "${COMPILE_COMMANDS_JSON}" # 依赖 compile_commands.json
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" # 或者 CMAKE_BINARY_DIR，确保文件路径能被正确解析
                            COMMENT "Exporting clang-tidy fixes for batch ${i} to ${BATCH_YAML_FILE}"
                            VERBATIM # 确保命令中的参数被正确处理
                    )
                    list(APPEND ALL_BATCH_YAML_FILES "${BATCH_YAML_FILE}")
                endif ()
            endforeach ()

            # 6. 创建一个总的 Target，它依赖于所有批处理 YAML 文件的生成
            if (ALL_BATCH_YAML_FILES)
                add_custom_target(clang-tidy-export-all
                        DEPENDS ${ALL_BATCH_YAML_FILES}
                        COMMENT "All clang-tidy fix batches exported. YAML files are in ${CLANG_TIDY_YAML_OUTPUT_DIR}"
                )
                message(STATUS "Custom target 'clang-tidy-export-all' created.")
                message(STATUS "Run: ninja clang-tidy-export-all (or make equivalent)")
                message(STATUS "Then review YAML files in '${CLANG_TIDY_YAML_OUTPUT_DIR}'")

                # 7. (可选) 创建一个 Target 来应用所有生成的 YAML 文件中的修复
                find_program(CLANG_APPLY_REPLACEMENTS_EXE clang-apply-replacements
                        HINTS ENV PATH "${LLVM_TOOLS_BINARY_DIR}"
                        DOC "Path to clang-apply-replacements executable")
                if (CLANG_APPLY_REPLACEMENTS_EXE)
                    add_custom_target(clang-tidy-apply-all
                            COMMAND "${CLANG_APPLY_REPLACEMENTS_EXE}"
                            "${CLANG_TIDY_YAML_OUTPUT_DIR}" # 将包含所有 batch-*.yaml 文件的目录传递给它
                            WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}" # 或者 CMAKE_BINARY_DIR
                            DEPENDS clang-tidy-export-all # 确保先生成了所有 YAML
                            COMMENT "Applying all exported clang-tidy fixes from YAML files in ${CLANG_TIDY_YAML_OUTPUT_DIR}"
                            VERBATIM
                    )
                    message(STATUS "Custom target 'clang-tidy-apply-all' created.")
                    message(STATUS "To apply fixes (after review), run: ninja clang-tidy-apply-all")
                else ()
                    message(WARNING "clang-apply-replacements executable not found. 'clang-tidy-apply-all' target not created. You can run it manually from '${CLANG_TIDY_YAML_OUTPUT_DIR}'.")
                endif ()
            else ()
                message(WARNING "No batch YAML files were configured for clang-tidy export. 'clang-tidy-export-all' target not created meaningfully.")
            endif ()
        endif ()
    else ()
        message(WARNING "clang-tidy executable not found, but ENABLE_CLANG_TIDY_FIX_EXPORT is ON. "
                "Please install clang-tidy or set ENABLE_CLANG_TIDY_FIX_EXPORT to OFF.")
    endif ()
endif ()


if (ENABLE_CLANG_TIDY_Apply_EXPORT)
    # 确保我们知道 YAML 文件输出到了哪个目录
    set(CLANG_TIDY_YAML_OUTPUT_DIR "${CMAKE_BINARY_DIR}/clang_tidy_fixes_yaml")

    # 找到 clang-apply-replacements 可执行文件
    find_program(CLANG_APPLY_REPLACEMENTS_EXE clang-apply-replacements
            HINTS ENV PATH "${LLVM_TOOLS_BINARY_DIR}" # 如果你通过 find_package(LLVM) 获得了 LLVM 工具目录
            REQUIRED # 如果这个功能对你很重要，可以设为 REQUIRED，找不到就会报错
            DOC "Path to clang-apply-replacements executable")

    if (CLANG_APPLY_REPLACEMENTS_EXE)
        # 创建自定义 Target 来应用所有导出的修复
        add_custom_target(clang-tidy-apply-all # 我改了个稍微具体点的名字，你可以用任何你喜欢的
                COMMAND "${CLANG_APPLY_REPLACEMENTS_EXE}"
                "${CLANG_TIDY_YAML_OUTPUT_DIR}" # 将包含所有 batch-*.yaml 文件的目录传递给它
                # clang-apply-replacements 会扫描这个目录找 .yaml 文件

                # WORKING_DIRECTORY 很重要，它应该是你的项目源码的根目录，
                # 以便 YAML 文件中记录的相对文件路径（如果有的话）能被正确解析。
                # 不过，通常 clang-tidy 生成的 YAML 中的 FilePath 是绝对路径，
                # 这种情况下 WORKING_DIRECTORY 对于 clang-apply-replacements 可能不那么敏感，
                # 但设置为源码根目录通常是最安全的选择。
                WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"

                # 依赖于导出 Target，确保 YAML 文件总是先被生成或更新
                DEPENDS clang-tidy-export-all # 假设你的导出 Target 名称是这个

                COMMENT "Applying all clang-tidy fixes from YAML files in ${CLANG_TIDY_YAML_OUTPUT_DIR}. "
                "IMPORTANT: Review YAML files before running this target!"
                VERBATIM # 确保命令和参数被正确传递
        )
        message(STATUS "Custom target 'olive-apply-tidy-fixes' created.")
        message(STATUS "Workflow: ")
        message(STATUS "  1. Run: ninja clang-tidy-export-all  (or your export target name)")
        message(STATUS "  2. CRITICAL: Manually review and edit YAML files in '${CLANG_TIDY_YAML_OUTPUT_DIR}' to ensure correctness.")
        message(STATUS "  3. Then run: ninja olive-apply-tidy-fixes  (to apply the reviewed fixes)")

    else ()
        # find_program 如果设置了 REQUIRED 且没找到，这里可能不会执行，CMake会直接报错
        # 如果没设置 REQUIRED 且没找到，这里会执行
        message(WARNING "clang-apply-replacements executable not found. "
                "Target 'olive-apply-tidy-fixes' cannot be created. "
                "You may need to run 'clang-apply-replacements ${CLANG_TIDY_YAML_OUTPUT_DIR}' manually "
                "from your source directory after reviewing YAML files.")
    endif ()

endif ()