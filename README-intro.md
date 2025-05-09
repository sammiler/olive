# Olive 视频编辑器 (推断的项目名称)

Olive 是一款先进的、可能是开源的非线性视频编辑和合成应用程序。它采用基于节点的工作流程来处理复杂的效果和处理任务，使用 C++ 和 Qt 框架构建，以实现跨平台的图形用户界面。

## 核心理念 (推断)

*   **基于节点的能力：** 提供灵活且强大的节点图，用于视频/音频处理、色彩校正、效果和合成。
*   **跨平台：** 借助于 CMake 和 Qt，设计为可在多种操作系统 (Windows、macOS、Linux) 上运行。
*   **可扩展性：** 模块化设计表明其架构可以通过新的节点、工具和功能进行扩展。
*   **专业特性：** 与 FFmpeg、OpenColorIO (OCIO) 和 OpenImageIO (OIIO) 的集成点表明其专注于专业媒体工作流程。

## 主要特性 (根据结构推断)

*   非线性时间轴编辑
*   基于节点的合成与特效系统
*   先进的色彩管理 (OCIO)
*   广泛的编解码器支持 (FFmpeg, OIIO)
*   音频处理与混音
*   可定制的用户界面，支持停靠小部件 (KDDockWidgets)
*   用于渲染和导出的后台任务管理
*   撤销/重做系统
*   国际化支持
*   崩溃处理与报告

## 代码理解顺序建议

为了更好地理解项目代码，建议按以下逻辑顺序进行阅读和学习，这大致反映了模块间的依赖关系和构建层次：

1.  **构建系统与外部依赖 (`cmake/`, 根 `CMakeLists.txt`, `.project/vcpkg/`, `ext/`)**:
    *   首先了解项目是如何通过 CMake 构建的，以及它依赖哪些外部库（如 `ext/KDDockWidgets`）。`ext/core` 尤其重要，因为它似乎是 Olive 自身的底层核心库。

2.  **Olive 核心库 (`ext/core/`)**:
    *   这个目录包含了 Olive 项目最基础的数据结构和实用工具 (如 `rational.h`, `timerange.h`, `color.h`)，它们被广泛应用于项目的其他部分。

3.  **应用层核心逻辑与数据模型 (`app/common/`, `app/core.cpp/.h`, `app/node/`)**:
    *   `app/common/`: 包含整个应用程序共享的工具类和定义。
    *   `app/core.cpp/.h`: 可能定义了应用程序的全局状态或核心管理类。
    *   `app/node/`: 这是 Olive 处理引擎的核心，定义了节点系统（包括各种节点类型如特效、转场、生成器）和项目数据结构（素材、序列）。理解节点系统是理解 Olive 工作原理的关键。

4.  **媒体处理与渲染 (`app/codec/`, `app/audio/`, `app/render/`, `app/shaders/`)**:
    *   `app/codec/`: 负责媒体文件的解码和编码，集成了 FFmpeg 和 OIIO。
    *   `app/audio/`: 音频处理、播放和波形生成。
    *   `app/render/`: 渲染管线，包括色彩处理 (OCIO)、缓存管理和 OpenGL 渲染。
    *   `app/shaders/`: GLSL 着色器，用于视觉效果。

5.  **应用支持服务 (`app/config/`, `app/task/`, `app/undo/`, `app/crashhandler/`, `app/timeline/`)**:
    *   `app/config/`: 应用配置的加载与保存。
    *   `app/task/`: 后台任务管理 (如导出、渲染)。
    *   `app/undo/`: 撤销/重做机制。
    *   `app/crashhandler/`: 崩溃处理。
    *   `app/timeline/`: 时间轴的具体逻辑和操作。

6.  **UI 基础与组件 (`app/ui/`, `app/widget/`)**:
    *   `app/ui/`: 包含主题、图标、光标等视觉资源和样式管理。
    *   `app/widget/`: 大量自定义的 Qt 小部件，是构建用户界面的基础模块。

7.  **UI 界面布局与交互 (`app/dialog/`, `app/panel/`, `app/tool/`)**:
    *   `app/dialog/`: 各种对话框的实现。
    *   `app/panel/`: 构成主界面的各个面板 (如时间轴面板、节点编辑器面板、监视器面板)。
    *   `app/tool/`: 定义了在 UI 中使用的各种交互工具的基类或接口。

8.  **主程序入口与窗口 (`app/main.cpp`, `app/window/`)**:
    *   `app/main.cpp`: 应用程序的入口点。
    *   `app/window/`: 主窗口的实现，负责整合所有 UI 面板和菜单。

9.  **辅助与周边模块 (`app/cli/`, `app/packaging/`, `app/ts/`, `docker/`, `tests/`)**:
    *   这些模块提供了命令行接口、打包、国际化、CI 环境和测试等功能，可以在理解了核心应用逻辑后再进行研究。

## 项目结构与模块概览

项目组织成几个主要目录：

### 1. 根目录 (`olive/`)

包含全局项目文件：
*   `.clang-format`: 代码格式化规则。
*   `CMakeLists.txt`: 整个项目的主 CMake 构建脚本。
*   `CONTRIBUTING.md`: 贡献者指南。
*   `LICENSE`: 项目许可证。
*   `README.md`: 此文件 (或您正在生成的文件)。
*   `update-copyright.sh`: 用于更新版权声明的脚本。

### 2. `.project/`

开发者特定的实用工具和项目模板：
*   `Tips.txt`: 开发提示。
*   `py-script/`: 用于构建自动化和开发任务的 Python 脚本 (例如 CMake 预设生成、复制 DLL)。
*   `template/`: 项目文件模板。
*   `util/`: 实用工具脚本 (例如 `set_env.bat`)。
*   `vcpkg/`: vcpkg C++ 包管理器的配置文件。

### 3. `app/`

核心应用程序源代码。这是项目中最大且最关键的部分。

*   **`audio/`**: 处理音频播放、处理、管理和波形生成。
*   **`cli/`**: 命令行界面组件，可能用于无头操作，如导出。
*   **`codec/`**: 管理媒体解码和编码。包括 FFmpeg 和 OpenImageIO (OIIO) 的封装。
*   **`common/`**: 共享的实用函数、通用定义、文件操作和整个应用程序中使用的辅助类。
*   **`config/`**: 应用程序配置的加载和保存。
*   **`core.cpp`, `core.h`**: 可能是中央应用程序类或全局应用程序状态管理器。
*   **`crashhandler/`**: 实现崩溃报告和处理，可能使用 Google Crashpad。
*   **`dialog/`**: 包含各种对话框窗口的 UI 代码 (例如，关于、首选项、导出、颜色选择器、项目属性)。
*   **`main.cpp`**: GUI 应用程序的主入口点。
*   **`node/`**: 处理引擎的核心。定义了基于节点的系统。
    *   包括各种节点类型：音频效果 (声像、音量)、块类型 (片段、间隙、过渡)、颜色操作 (OCIO)、扭曲、视觉效果、滤镜、生成器 (纯色、文本、噪声)、Gizmo (屏幕控制器)、输入、抠像、数学运算等。
    *   `factory.cpp/.h`: 可能负责创建不同节点类型的实例。
    *   `project/`: 在节点系统中定义项目结构 (素材、序列、文件夹、项目文件序列化器)。
*   **`packaging/`**: 用于为 Linux (AppImage/AppRun)、macOS 和 Windows (NSIS 安装程序) 创建可分发应用程序包的脚本和配置。
*   **`panel/`**: 应用程序界面中主面板的 UI 代码 (例如，音频监视器、曲线编辑器、素材查看器、历史记录、节点编辑器、参数、项目浏览器、示波器、时间轴、查看器)。
*   **`render/`**: 处理渲染管线。
    *   包括颜色处理 (OCIO 集成)、缓存 (磁盘、帧、音频)、OpenGL 渲染、渲染任务的作业管理和纹理管理。
    *   `ocioconf/`: OpenColorIO 配置文件和 LUT。
*   **`shaders/`**: 用于各种视觉效果、颜色操作和 UI 元素的 GLSL 着色器代码。
*   **`task/`**: 管理后台任务，如媒体整理、导出、缓存和项目加载/保存。
*   **`timeline/`**: 编辑时间轴的逻辑，包括标记、坐标系和特定于时间轴操作的撤销命令。
*   **`tool/`**: 为 UI 中使用的工具定义抽象基类或接口 (例如，时间轴上的指针工具、剃刀工具)。
*   **`ts/`**: 用于国际化的翻译文件 (`.ts`，用于 Qt Linguist)。
*   **`ui/`**: 底层 UI 组件和样式。
    *   `cursors/`: 自定义鼠标光标。
    *   `graphics/`: 闪屏、加载动画等。
    *   `icons/`: 图标管理。
    *   `style/`: 主题 (例如 `olive-dark`、`olive-light`)，包括 CSS、调色板和图标资源 (SVG 和 PNG)。
*   **`undo/`**: 通用撤销/重做命令框架。
*   **`version.cpp`, `version.h`**: 应用程序版本信息。
*   **`widget/`**: 用于构建应用程序用户界面的丰富自定义 Qt 小部件集合 (例如，贝塞尔曲线编辑器、颜色选择器、滑块、节点视图、时间轴小部件、查看器)。
*   **`window/`**: 管理主应用程序窗口，包括其布局、菜单栏和状态栏。

### 4. `cmake/`

自定义 CMake 模块，用于辅助构建过程，例如查找特定库 (FFMPEG、OpenEXR、Crashpad) 或设置 QHeaders。

### 5. `docker/`

用于创建 Docker化构建和持续集成 (CI) 环境的 Dockerfile 配置和脚本。这有助于确保不同系统上的一致构建。

### 6. `ext/`

外部库或子模块。
*   **`KDDockWidgets/`**: 一个第三方库，为 Qt 应用程序提供高级停靠小部件功能。这是一个重要的 UI 组件。
*   **`core/`**: 这似乎是专门为 Olive 设计的基础库，提供非常底层的实用工具和数据结构。
    *   `include/olive/core/`: 此 Olive 核心库的公共头文件。
        *   `render/`: 基本渲染类型 (像素格式、采样缓冲区)。
        *   `util/`: 核心实用工具 (贝塞尔曲线、颜色、数学、有理数、时间码、时间范围、值类型)。
    *   `src/`: 这些核心实用工具的实现。
    *   这个 `ext/core` 库可能提供了贯穿 `app/` 模块使用的基本构建块。

### 7. `tests/`

单元测试和集成测试的源代码，确保各种应用程序组件的稳定性和正确性 (例如，合成、通用实用工具、时间轴)。

## 技术栈 (推断)

*   **编程语言**: C++ (可能是 C++17 或更新版本)
*   **构建系统**: CMake
*   **GUI 框架**: Qt (可能是 Qt 5 或 Qt 6)
*   **停靠框架**: KDDockWidgets
*   **图形 API**: OpenGL (用于渲染和着色器)
*   **媒体处理**:
    *   FFmpeg (用于广泛的音视频编解码器)
    *   OpenImageIO (OIIO) (用于图像序列和静态图像格式)
*   **色彩管理**: OpenColorIO (OCIO)
*   **崩溃报告**: Google Crashpad (推断)
*   **包管理 (开发)**: vcpkg (推断)
*   **CI/构建环境**: Docker

## 快速上手 (设想)

1.  **克隆仓库。**
2.  **安装依赖：**
    *   C++ 编译器 (支持 C++17/20)
    *   CMake
    *   Qt (与项目匹配的 SDK 版本)
    *   FFmpeg, OpenColorIO, OpenImageIO 等 (如果已配置，则使用 vcpkg)
3.  **配置构建：**
    ```bash
    mkdir build && cd build
    cmake .. -DCMAKE_PREFIX_PATH=/path/to/Qt
    ```
4.  **构建项目：**
    ```bash
    cmake --build .
    ```
5.  **运行应用程序。**

(注意：实际构建步骤可能因项目特定的 CMake 选项和依赖管理而异)。

---
此 README 为理解 Olive 项目提供了一个良好的起点。通过建议的阅读顺序，应该能更容易地逐步深入了解其复杂的架构。