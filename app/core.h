#ifndef CORE_H
#define CORE_H

#include <olive/core/core.h>  // 引入 Olive 核心库的基础定义 (例如 olive::core::rational, Timecode, SampleFormat)
#include <QFileInfoList>      // 引入 QFileInfoList，用于处理文件和目录列表
#include <QList>              // 引入 QList 容器
#include <QObject>            // 引入 QObject 基类
#include <QStringList>        // 引入 QStringList
#include <QTimer>             // 引入 QTimer，用于定时任务，如自动恢复
#include <QTranslator>        // 引入 QTranslator，用于国际化和语言切换
#include <QUuid>              // 引入 QUuid，用于唯一标识符

#include "node/project.h"                    // 引入项目类的定义
#include "node/project/footage/footage.h"    // 引入素材类的定义
#include "node/project/sequence/sequence.h"  // 引入序列类的定义
#include "task/task.h"                       // 引入任务基类的定义 (虽然在此文件中未直接使用，但可能是上下文依赖)
#include "tool/tool.h"                       // 引入工具枚举和相关定义
#include "undo/undostack.h"                  // 引入撤销栈类的定义
#include "widget/projectexplorer/projectviewmodel.h"  // 引入项目浏览器模型的定义 (虽然在此文件中未直接使用，但可能是上下文依赖)

// 前向声明 MainWindow, Folder, ViewerOutput, Node, MultiUndoCommand
// 如果它们的完整定义已在上述头文件中，则可能非必需。
namespace olive {
class MainWindow;
// class Folder; // 已在 node/project/footage/footage.h 中通过 project.h 间接包含
// class ViewerOutput; // 通常是 Node 的派生类
// class Node; // 已在 node/project.h 中包含
// class MultiUndoCommand; // 已在 undo/undostack.h 中通过 undocommand.h 包含
}  // namespace olive

namespace olive {

// 如果 MainWindow 未完整定义，则需要前向声明
// class MainWindow;

/**
 * @brief Olive 应用程序的主要中央实例。
 *
 * Core 类在 GUI 和 CLI（命令行界面）模式下均可运行，并根据运行模式处理相应的初始化。
 * 它还包含供整个 Olive 使用的各种全局函数/变量。
 *
 * "public slots" 通常是用户触发的动作，可以连接到 UI 元素（例如创建文件夹、打开导入对话框等）。
 */
class Core : public QObject {
 Q_OBJECT  // 声明此类使用 Qt 的元对象系统（信号和槽）
     public :
     /**
      * @brief 存储 Core 对象初始化参数的类。
      */
     class CoreParams {
   public:
    /**
     * @brief CoreParams 构造函数。
     *
     * 初始化参数为默认值。
     */
    CoreParams();

    /**
     * @brief 定义应用程序的运行模式。
     */
    enum RunMode {
      kRunNormal,        ///< 正常 GUI 运行模式。
      kHeadlessExport,   ///< 无头导出模式（命令行导出，无UI）。
      kHeadlessPreCache  ///< 无头预缓存模式（命令行执行缓存任务，无UI）。
    };

    /**
     * @brief 获取是否以全屏模式启动。
     * @return 如果以全屏模式启动则返回 true。
     * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
     */
    [[nodiscard]] bool fullscreen() const { return run_fullscreen_; }

    /**
     * @brief 设置是否以全屏模式启动。
     * @param e true 表示全屏启动。
     */
    void set_fullscreen(bool e) { run_fullscreen_ = e; }

    /**
     * @brief 获取当前的运行模式。
     * @return 返回 RunMode 枚举值。
     * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
     */
    [[nodiscard]] RunMode run_mode() const { return mode_; }

    /**
     * @brief 设置运行模式。
     * @param m 要设置的 RunMode。
     */
    void set_run_mode(RunMode m) { mode_ = m; }

    /**
     * @brief 获取启动时要加载的项目文件路径。
     * @return 返回项目文件路径的 QString。
     * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
     */
    [[nodiscard]] QString startup_project() const { return startup_project_; }

    /**
     * @brief 设置启动时要加载的项目文件路径。
     * @param p 项目文件的路径。
     */
    void set_startup_project(const QString& p) { startup_project_ = p; }

    /**
     * @brief 获取启动时使用的语言环境字符串。
     * @return 返回语言环境字符串 (例如 "en_US", "zh_CN")。
     * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
     */
    [[nodiscard]] const QString& startup_language() const { return startup_language_; }

    /**
     * @brief 设置启动时使用的语言环境。
     * @param s 语言环境字符串。
     */
    void set_startup_language(const QString& s) { startup_language_ = s; }

    /**
     * @brief 获取是否在启动时触发崩溃（用于测试崩溃处理）。
     * @return 如果设置为在启动时崩溃则返回 true。
     * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
     */
    [[nodiscard]] bool crash_on_startup() const { return crash_; }

    /**
     * @brief 设置是否在启动时触发崩溃。
     * @param e 如果为 true，则会在启动时故意触发崩溃。
     */
    void set_crash_on_startup(bool e) {
      crash_ = true;
    }  // 注意：这里无论 e 为何值，crash_ 都被设为 true。这可能是一个笔误，或者是有意为之的设计。

   private:
    RunMode mode_;              ///< 应用程序的运行模式。
    QString startup_project_;   ///< 启动时加载的项目路径。
    QString startup_language_;  ///< 启动时使用的语言。
    bool run_fullscreen_;       ///< 是否以全屏模式启动。
    bool crash_;                ///< 是否在启动时故意崩溃（用于测试）。
  };

  /**
   * @brief Core 构造函数。
   * @param params 初始化 Core 对象所需的参数。
   */
  explicit Core(CoreParams params);

  /**
   * @brief 获取 Core 对象的全局单例实例。
   *
   * 使用此方法在代码的任何地方访问 Core 的功能。
   * @return 返回指向 Core 单例对象的指针。
   */
  static Core* instance();

  /**
   * @brief 获取 Core 的初始化参数。
   * @return 返回 CoreParams 对象的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const CoreParams& core_params() const { return core_params_; }

  /**
   * @brief 启动 Olive Core。
   *
   * 这是应用程序的主启动函数。它会解析命令行参数，并根据运行模式构造主窗口（如果进入 GUI 模式）。
   */
  void Start();

  /**
   * @brief 停止 Olive Core。
   *
   * 结束所有线程并释放所有内存，为应用程序退出做准备。
   */
  void Stop();

  /**
   * @brief 获取主窗口实例。
   * @return 返回指向 olive::MainWindow 对象的指针；如果在 CLI 模式下运行，则返回 nullptr。
   */
  MainWindow* main_window();

  /**
   * @brief 获取 UndoStack（撤销栈）对象。
   * @return 返回指向 UndoStack 对象的指针。
   */
  UndoStack* undo_stack();

  /**
   * @brief 导入一个文件列表到指定的父文件夹。
   *
   * FIXME: 注释中提到对此实现方式不满意，需要模型更新机制。
   * @param urls 要导入的文件的 URL 列表 (QStringList)。
   * @param parent 导入文件的目标父文件夹 (Folder*)。
   */
  void ImportFiles(const QStringList& urls, Folder* parent);

  /**
   * @brief 获取当前活动的工具。
   * @return 返回当前激活的 Tool::Item 枚举值的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const Tool::Item& tool() const;

  /**
   * @brief 获取当前“添加工具”选中的可添加对象类型（如果“添加工具”当前激活）。
   * @return 返回当前选中的 Tool::AddableObject 枚举值的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const Tool::AddableObject& GetSelectedAddableObject() const;

  /**
   * @brief 获取当前“转场工具”选中的转场节点类型（如果“转场工具”当前激活）。
   * @return 返回所选转场标识符的 QString 常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const QString& GetSelectedTransition() const;

  /**
   * @brief 获取当前的吸附设置值。
   * @return 返回表示吸附是否启用的布尔值的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const bool& snapping() const;

  /**
   * @brief 返回最近打开/保存的项目列表。
   * @return 返回包含最近项目文件路径的 QStringList 的常量引用。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] const QStringList& GetRecentProjects() const;

  /**
   * @brief 获取当前活动的项目。
   *
   * 使用 UI/面板系统来确定用户最后聚焦的项目，并假定这是用户希望操作的活动项目。
   * @return 返回活动的 Project 文件指针；如果启发式方法未能找到活动项目，则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] Project* GetActiveProject() const;

  /**
   * @brief 获取当前活动项目中选定的文件夹。
   * @return 返回指向选定 Folder 对象的指针；如果没有选定的文件夹或活动项目，则返回 nullptr。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] static Folder* GetSelectedFolderInActiveProject();

  /**
   * @brief 获取当前的时间码显示模式。
   * @return 返回 Timecode::Display 枚举值。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] static Timecode::Display GetTimecodeDisplay();

  /**
   * @brief 设置当前的时间码显示模式。
   * @param d 要设置的 Timecode::Display 枚举值。
   */
  void SetTimecodeDisplay(Timecode::Display d);

  /**
   * @brief 设置自动恢复文件的保存频率（如果项目已更改，请参阅 SetProjectModified()）。
   * @param minutes 自动保存的时间间隔（分钟）。
   */
  void SetAutorecoveryInterval(int minutes);

  /**
   * @brief 将字符串复制到系统剪贴板。
   * @param s 要复制的字符串。
   */
  static void CopyStringToClipboard(const QString& s);

  /**
   * @brief 从系统剪贴板粘贴字符串。
   * @return 返回从剪贴板获取的字符串。
   */
  static QString PasteStringFromClipboard();

  /**
   * @brief 递归计算文件/目录列表中的文件数量。
   * @param filenames 包含文件和目录信息的 QFileInfoList。
   * @return 返回文件总数。
   */
  static int CountFilesInFileList(const QFileInfoList& filenames);

  /**
   * @brief 向用户显示一个对话框，用于重命名一组节点。
   * @param nodes 要重命名的 Node 指针的 QVector。
   * @param parent 可选的父 MultiUndoCommand，用于将此操作组合到更大的撤销/重做序列中。
   * @return 如果重命名操作被用户确认并执行，则返回 true；否则返回 false。
   */
  bool LabelNodes(const QVector<Node*>& nodes, MultiUndoCommand* parent = nullptr);

  /**
   * @brief 为活动项目创建一个名称合适的新序列。
   * @param format 序列名称的格式化字符串 (例如 "Sequence %1")。
   * @param project 目标项目。
   * @return 返回新创建的 Sequence 指针。
   */
  static Sequence* CreateNewSequenceForProject(const QString& format, Project* project);

  /**
   * @brief 为活动项目创建一个使用默认名称格式的新序列。
   * @param project 目标项目。
   * @return 返回新创建的 Sequence 指针。
   */
  static Sequence* CreateNewSequenceForProject(Project* project) {
    return CreateNewSequenceForProject(tr("Sequence %1"), project);  // 使用本地化的默认名称格式
  }

  /**
   * @brief 从最近打开的项目列表中打开一个项目。
   * @param index 项目在最近列表中的索引。
   */
  void OpenProjectFromRecentList(int index);

  /**
   * @brief 关闭当前项目。
   * @param auto_open_new 如果为 true，在关闭当前项目后（如果它是最后一个打开的项目）自动创建一个新项目。
   * @param ignore_modified 如果为 true，则忽略项目未保存的更改，直接关闭。
   * @return 如果项目成功关闭（或没有项目可关闭），则返回 true；如果用户取消关闭（例如，在保存提示时），则返回 false。
   */
  bool CloseProject(bool auto_open_new, bool ignore_modified = false);

  /**
   * @brief 在当前活动的序列上运行一个模态的缓存任务。
   * @param in_out_only 如果为 true，则仅缓存标记的入点到出点范围。
   */
  void CacheActiveSequence(bool in_out_only);

  /**
   * @brief 检查已加载项目中的每个素材对象，确认其源文件是否存在或已更改。
   * @param project 要验证素材的项目。
   * @param project_saved_url 项目保存时的原始 URL，用于解析相对路径。
   * @return 如果所有素材都有效或用户解决了问题，则返回 true；否则返回 false。
   */
  bool ValidateFootageInLoadedProject(Project* project, const QString& project_saved_url);

  /**
   * @brief 更改当前应用程序的语言。
   * @param locale 要设置的语言环境字符串 (例如 "en_US", "zh_CN")。
   * @return 如果语言成功更改，则返回 true；否则返回 false。
   */
  bool SetLanguage(const QString& locale);

  /**
   * @brief 在主窗口的状态栏中显示一条消息。
   *
   * 这是 Core::instance()->main_window()->statusBar()->showMessage() 的简写。
   * @param s 要显示的消息文本。
   * @param timeout 消息显示的超时时间（毫秒）。0 表示一直显示直到被清除或被新消息替换。
   */
  void ShowStatusBarMessage(const QString& s, int timeout = 0);

  /**
   * @brief 清除主窗口状态栏中的当前消息。
   */
  void ClearStatusBarMessage();

  /**
   * @brief 打开一个自动恢复的项目文件。
   * @param filename 恢复文件的路径。
   */
  void OpenRecoveryProject(const QString& filename);

  /**
   * @brief 在查看器中打开指定的输出节点。
   * @param viewer 指向要打开的 ViewerOutput 节点的指针。
   */
  void OpenNodeInViewer(ViewerOutput* viewer);

  /**
   * @brief 为指定的查看器输出节点打开导出对话框。
   * @param viewer 要导出的 ViewerOutput 节点。
   * @param start_still_image 如果为 true，导出对话框将预设为导出静止图像。
   */
  void OpenExportDialogForViewer(ViewerOutput* viewer, bool start_still_image);

  /**
   * @brief 检查“魔法”调试功能是否已启用。
   * @return 如果启用了魔法功能，则返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool IsMagicEnabled() const { return magic_; }

 public slots:
  /**
   * @brief 启动一个打开文件对话框，以从文件加载项目。
   */
  void OpenProject();

  /**
   * @brief 保存当前项目。
   * @return 如果保存成功或没有活动项目可保存，则返回 true；如果用户取消保存或保存失败，则返回 false。
   */
  bool SaveProject();

  /**
   * @brief 对当前项目执行“另存为”操作。
   * @return 如果另存为成功，则返回 true；如果用户取消或失败，则返回 false。
   */
  bool SaveProjectAs();

  /**
   * @brief 将当前项目恢复到上次保存的状态。
   */
  void RevertProject();

  /**
   * @brief 设置当前应用程序范围内的活动工具。
   * @param tool 要设置的 Tool::Item 枚举值。
   */
  void SetTool(const Tool::Item& tool);

  /**
   * @brief 设置当前的吸附设置。
   * @param b true 表示启用吸附，false 表示禁用。
   */
  void SetSnapping(const bool& b);

  /**
   * @brief 显示“关于”对话框。
   */
  void DialogAboutShow();

  /**
   * @brief 打开导入素材对话框，并导入用户选择的文件 (调用 ImportFiles())。
   */
  void DialogImportShow();

  /**
   * @brief 显示“首选项”对话框。
   */
  void DialogPreferencesShow();

  /**
   * @brief 显示“项目属性”对话框。
   */
  void DialogProjectPropertiesShow();

  /**
   * @brief 显示“导出”对话框。
   */
  void DialogExportShow();

#ifdef USE_OTIO  // 仅当启用了 OpenTimelineIO 支持时编译
  /**
   * @brief 显示 OTIO (OpenTimelineIO) 导入对话框。
   * @param sequences 一个 QList，用于接收导入的序列。
   * @return 如果导入成功，返回 true。
   * [[nodiscard]] 属性提示编译器，此函数的返回值不应被忽略。
   */
  [[nodiscard]] bool DialogImportOTIOShow(const QList<Sequence*>& sequences) const;
#endif

  /**
   * @brief 在当前活动项目中创建一个新文件夹。
   */
  void CreateNewFolder();

  /**
   * @brief 在当前活动项目中创建一个新序列。
   */
  void CreateNewSequence();

  /**
   * @brief 设置当前“添加工具”应创建的对象类型。
   * @param obj 要设置的 Tool::AddableObject 枚举值。
   */
  void SetSelectedAddableObject(const Tool::AddableObject& obj);

  /**
   * @brief 设置当前“转场工具”应创建的转场类型。
   * @param obj 转场类型的标识符字符串。
   */
  void SetSelectedTransitionObject(const QString& obj);

  /**
   * @brief 清除最近打开/保存的项目列表。
   */
  void ClearOpenRecentList();

  /**
   * @brief 创建一个新的空项目并打开它。
   */
  void CreateNewProject();

  /**
   * @brief 检查是否存在可用的自动恢复文件。
   */
  void CheckForAutoRecoveries();

  /**
   * @brief 浏览（通常是打开一个对话框显示）可用的自动恢复文件。
   */
  void BrowseAutoRecoveries();

  /**
   * @brief 请求在所有查看器中启用或禁用像素采样（颜色拾取）功能。
   * @param e true 表示启用，false 表示禁用。
   */
  void RequestPixelSamplingInViewers(bool e);

  /**
   * @brief 当缓存已满时发出警告。
   */
  void WarnCacheFull();

  /**
   * @brief 设置“魔法”调试功能的启用状态。
   * @param e true 表示启用。
   */
  void SetMagic(bool e) { magic_ = e; }

 signals:
  /**
   * @brief 当活动工具从任何地方更改时发出的信号。
   * @param tool 新的 Tool::Item。
   */
  void ToolChanged(const Tool::Item& tool);

  /**
   * @brief 当通过 SetSelectedAddableObject 更改可添加对象时发出的信号。
   * @param o 新的 Tool::AddableObject。
   */
  void AddableObjectChanged(Tool::AddableObject o);

  /**
   * @brief 当吸附设置更改时发出的信号。
   * @param b 新的吸附状态。
   */
  void SnappingChanged(const bool& b);

  /**
   * @brief 当默认的时间码显示模式更改时发出的信号。
   * @param d 新的 Timecode::Display 模式。
   */
  void TimecodeDisplayChanged(Timecode::Display d);

  /**
   * @brief 当最近打开项目列表发生更改时发出的信号。
   */
  void OpenRecentListChanged();

  /**
   * @brief 在所有查看器上启用鼠标颜色采样功能的信号。
   *
   * 此功能可能较慢，因此仅在需要时开启。
   * @param e true 表示启用，false 表示禁用。
   */
  void ColorPickerEnabled(bool e);

  /**
   * @brief 当启用了颜色拾取的查看器发出一个颜色时触发的信号。
   * @param reference 参考颜色空间中的颜色值。
   * @param display 显示颜色空间中的颜色值。
   */
  void ColorPickerColorEmitted(const Color& reference, const Color& display);

 private:
  /**
   * @brief 获取可用于 QFileDialog 打开和保存兼容项目的文件过滤器字符串。
   * @param include_any_filter 如果为 true，则包含一个“任何文件 (*)”的过滤器选项。
   * @return 返回文件过滤器字符串。
   */
  static QString GetProjectFilter(bool include_any_filter);

  /**
   * @brief 返回存储最近打开/保存项目列表的文件路径。
   * @return 返回文件路径的 QString。
   */
  static QString GetRecentProjectsFilePath();

  /**
   * @brief 仅在启动时调用以设置区域设置（语言环境）。
   */
  void SetStartupLocale();

  /**
   * @brief 将文件名添加到最近打开项目列表的顶部（如果已存在则移至顶部）。
   * @param s 要添加或移动的文件名。
   */
  void PushRecentlyOpenedProject(const QString& s);

  /**
   * @brief 为 Qt 的信号/槽系统声明自定义类型/类。
   *
   * Qt 的信号/槽系统要求类型被声明。为了仅在启动时执行此操作，我们将它们都包含在此函数中。
   */
  static void DeclareTypesForQt();

  /**
   * @brief 启动 Olive 的 GUI 部分。
   *
   * 启动 Olive GUI 所需的服务和对象。保证在不运行此函数的情况下创建的应用程序实例
   * 是完全有效的，只是没有UI（例如，用于 CLI 模式）。
   * @param full_screen 如果为 true，则以全屏模式启动 GUI。
   */
  void StartGUI(bool full_screen);

  /**
   * @brief 将项目保存到文件的内部函数。
   * @param override_filename 可选参数，如果提供，则项目将保存到此文件名而不是其当前文件名。
   */
  void SaveProjectInternal(const QString& override_filename = QString());

  /**
   * @brief 获取当前最活动的序列以供导出。
   * @return 返回指向要导出的 ViewerOutput 节点的指针。
   */
  ViewerOutput* GetSequenceToExport();

  /**
   * @brief 获取自动恢复索引文件的名称。
   * @return 返回索引文件名。
   */
  static QString GetAutoRecoveryIndexFilename();

  /**
   * @brief 保存未恢复的项目列表（可能用于下次启动时提示用户）。
   */
  void SaveUnrecoveredList();

  /**
   * @brief 恢复项目的内部实现。
   * @param by_opening_existing 如果为 true，表示恢复是通过打开一个已存在的（可能是备份）文件。
   * @return 如果恢复成功，返回 true。
   */
  bool RevertProjectInternal(bool by_opening_existing);

  /**
   * @brief 保存最近打开的项目列表到持久存储。
   */
  void SaveRecentProjectsList();

  /**
   * @brief 将项目添加到“打开的项目”列表中。
   * @param p 要添加的 Project 指针。
   * @param add_to_recents 如果为 true，同时将项目添加到最近文件列表。
   */
  void AddOpenProject(olive::Project* p, bool add_to_recents = false);

  /**
   * @brief 从一个（加载）任务的结果中添加一个打开的项目。
   * @param task 完成的项目加载任务。
   * @param add_to_recents 是否添加到最近文件列表。
   * @return 如果成功添加项目，返回 true。
   */
  bool AddOpenProjectFromTask(Task* task, bool add_to_recents);

  /**
   * @brief 设置当前活动的 Project 对象。
   * @param p 要设置为活动的项目。
   */
  void SetActiveProject(Project* p);

  MainWindow* main_window_;  ///< 指向主窗口对象的指针。

  Project* open_project_;  ///< 指向当前打开的唯一项目对象的指针 (Olive 可能设计为单项目实例)。

  Tool::Item tool_;                     ///< 当前激活的编辑工具。
  Tool::AddableObject addable_object_;  ///< 当前“添加工具”选中的可添加对象类型。
  QString selected_transition_;         ///< 当前“转场工具”选中的转场类型ID。
  bool snapping_;                       ///< 当前的吸附启用状态。

  QTimer autorecovery_timer_;    ///< 用于触发自动保存恢复文件的定时器。
  UndoStack undo_stack_;         ///< 应用程序范围内的撤销/重做栈实例。
  QStringList recent_projects_;  ///< 最近打开/保存的项目文件路径列表。

  CoreParams core_params_;  ///< 存储应用程序启动时设置的参数。

  static Core* instance_;  ///< Core 类的静态单例实例指针。

  QTranslator* translator_;  ///< 用于处理应用程序界面翻译的 QTranslator 对象。

  QVector<QUuid> autorecovered_projects_;  ///< 存储已创建自动恢复文件但尚未被用户处理（恢复或丢弃）的项目ID列表。

  bool magic_;  ///< 一个“魔法”调试标志。

  int pixel_sampling_users_;       ///< 当前请求像素采样（颜色拾取）功能的部件数量。
  bool shown_cache_full_warning_;  ///< 标记是否已经显示过缓存已满的警告。

 private slots:
  /**
   * @brief 保存自动恢复文件的槽函数。
   *
   * 由 autorecovery_timer_ 触发。
   */
  void SaveAutorecovery();

  /**
   * @brief 项目保存成功后调用的槽函数。
   * @param task 完成的项目保存任务。
   */
  void ProjectSaveSucceeded(Task* task);

  /**
   * @brief 从任务结果添加打开的项目并将其加入最近列表的槽函数。
   * @param task 完成的项目加载任务。
   * @return 如果成功，返回 true。
   */
  bool AddOpenProjectFromTaskAndAddToRecents(Task* task) { return AddOpenProjectFromTask(task, true); }

  /**
   * @brief 导入文件任务完成时调用的槽函数。
   * @param task 完成的导入任务。
   */
  void ImportTaskComplete(Task* task);

  /**
   * @brief 确认用户是否希望将一组文件作为图像序列导入。
   * @param filename 序列中的一个文件名（通常是第一个）。
   * @return 如果用户确认作为图像序列导入，返回 true。
   */
  bool ConfirmImageSequence(const QString& filename);

  /**
   * @brief 当项目被修改状态发生变化时调用的槽函数。
   * @param e true 表示项目已被修改，false 表示项目未被修改（已保存）。
   */
  void ProjectWasModified(bool e);

  /**
   * @brief 启动无头导出过程的槽函数。
   * @return 如果导出过程成功启动，返回 true。
   */
  bool StartHeadlessExport();

  /**
   * @brief 打开在启动参数中指定的项目的槽函数。
   */
  void OpenStartupProject();

  /**
   * @brief 从任务结果添加一个恢复项目的槽函数。
   * @param task 完成的项目加载任务（用于恢复）。
   */
  void AddRecoveryProjectFromTask(Task* task);

  /**
   * @brief 打开项目的内部实现。
   * @param filename 要打开的项目文件的路径。
   * @param recovery_project 如果为 true，则表示正在打开一个恢复文件。
   */
  void OpenProjectInternal(const QString& filename, bool recovery_project = false);

  /**
   * @brief 导入单个文件的槽函数（可能是拖放等操作的结果）。
   * @param f 要导入的文件的路径。
   */
  void ImportSingleFile(const QString& f);
};

}  // namespace olive

#endif  // CORE_H
