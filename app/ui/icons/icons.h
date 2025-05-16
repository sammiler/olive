#ifndef ICONS_H
#define ICONS_H

#include <QIcon>    // 引入 QIcon 类，用于处理和显示图标
#include <QString>  // 引入 QString 类，用于字符串操作 (虽然在此文件中未直接使用，但 Create 和 LoadAll 函数参数需要)

#include "common/define.h" // 引入项目内通用的定义文件

namespace olive::icon { // 为图标相关的功能定义命名空间 olive::icon

// Playback Icons / 播放控制相关图标
extern QIcon GoToStart; ///< 跳转到开头图标
extern QIcon PrevFrame; ///< 上一帧图标
extern QIcon Play;      ///< 播放图标
extern QIcon Pause;     ///< 暂停图标
extern QIcon NextFrame; ///< 下一帧图标
extern QIcon GoToEnd;   ///< 跳转到末尾图标

// Project Management Toolbar Icons / 项目管理工具栏图标
extern QIcon New;      ///< 新建项目图标
extern QIcon Open;     ///< 打开项目图标
extern QIcon Save;     ///< 保存项目图标
extern QIcon Undo;     ///< 撤销操作图标
extern QIcon Redo;     ///< 重做操作图标
extern QIcon TreeView; ///< 树状视图模式图标
extern QIcon ListView; ///< 列表视图模式图标
extern QIcon IconView; ///< 图标视图模式图标

// Tool Icons / 编辑工具图标
extern QIcon ToolPointer;     ///< 指针工具图标
extern QIcon ToolEdit;        ///< 编辑工具图标
extern QIcon ToolRipple;      ///< 涟漪编辑工具图标
extern QIcon ToolRolling;     ///< 卷动编辑工具图标
extern QIcon ToolRazor;       ///< 剃刀工具图标
extern QIcon ToolSlip;        ///< 滑移工具图标
extern QIcon ToolSlide;       ///< 滑动工具图标
extern QIcon ToolHand;        ///< 手型工具图标 (用于平移视图)
extern QIcon ToolTransition;  ///< 转场工具图标
extern QIcon ToolTrackSelect; ///< 轨道选择工具图标

// Project Icons / 项目元素相关图标
extern QIcon Folder;   ///< 文件夹图标
extern QIcon Sequence; ///< 序列图标
extern QIcon Video;    ///< 视频素材图标
extern QIcon Audio;    ///< 音频素材图标
extern QIcon Image;    ///< 图像素材图标

// Node Icons / 节点编辑器相关图标
extern QIcon MiniMap; ///< 节点编辑器小地图图标

// Triangle Arrows / 三角形箭头图标 (常用于展开/折叠)
extern QIcon TriUp;    ///< 向上三角形箭头图标
extern QIcon TriLeft;  ///< 向左三角形箭头图标
extern QIcon TriDown;  ///< 向下三角形箭头图标
extern QIcon TriRight; ///< 向右三角形箭头图标

// Text / 文本编辑相关图标
extern QIcon TextBold;          ///< 文本加粗图标
extern QIcon TextItalic;        ///< 文本斜体图标
extern QIcon TextUnderline;     ///< 文本下划线图标
extern QIcon TextStrikethrough; ///< 文本删除线图标
extern QIcon TextSmallCaps;     ///< 文本小型大写字母图标
extern QIcon TextAlignLeft;     ///< 文本左对齐图标
extern QIcon TextAlignRight;    ///< 文本右对齐图标
extern QIcon TextAlignCenter;   ///< 文本居中对齐图标
extern QIcon TextAlignJustify;  ///< 文本两端对齐图标
extern QIcon TextAlignTop;      ///< 文本顶端对齐图标
extern QIcon TextAlignBottom;   ///< 文本底端对齐图标
extern QIcon TextAlignMiddle;   ///< 文本垂直居中对齐图标

// Miscellaneous Icons / 其他杂项图标
extern QIcon Snapping;    ///< 吸附功能开关图标
extern QIcon ZoomIn;      ///< 放大图标
extern QIcon ZoomOut;     ///< 缩小图标
extern QIcon Record;      ///< 录制图标
extern QIcon Add;         ///< 添加图标
extern QIcon Error;       ///< 错误/警告图标
extern QIcon DirUp;       ///< 向上导航（父文件夹）图标
extern QIcon Clock;       ///< 时钟/时间相关图标
extern QIcon Diamond;     ///< 菱形图标 (可能用于关键帧等)
extern QIcon Plus;        ///< 加号图标
extern QIcon Minus;       ///< 减号图标
extern QIcon AddEffect;   ///< 添加特效图标
extern QIcon EyeOpened;   ///< 可见状态（睁眼）图标
extern QIcon EyeClosed;   ///< 不可见状态（闭眼）图标
extern QIcon LockOpened;  ///< 未锁定状态图标
extern QIcon LockClosed;  ///< 锁定状态图标
extern QIcon Pencil;      ///< 编辑/铅笔图标
extern QIcon Subtitles;   ///< 字幕相关图标
extern QIcon ColorPicker; ///< 颜色拾取器图标

/**
 * @brief 从文件加载并创建一个图标对象。
 *
 * 此函数使用 `name` 参数从文件加载图标资源，以创建一个可在整个应用程序中使用的 QIcon 对象。
 *
 * Olive 的图标以一种非常特定的格式存储。它们都源自 SVG 文件，但以多种尺寸的 PNG 格式存储。
 * 请参阅 `app/ui/icons/genicons.sh` 脚本，该脚本不仅生成多种尺寸的 PNG，
 * 还生成用于将图标编译到可执行文件中的 QRC 文件。
 *
 * 此函数与 `genicons.sh` 脚本紧密相关，它会加载 `genicons.sh` 生成的所有不同尺寸的图像
 * (使用相同的文件名格式和 QRC 资源目录) 到一个 QIcon 对象中。如果您更改此函数或
 * `genicons.sh`，您很可能也需要更改另一个。
 *
 * 除了在 LoadAll() 函数（该函数将图标全局存储在内存中，这样每次需要图标时就不必重新加载）
 * 之外，几乎没有理由调用此函数。
 *
 * @param theme 主题名称（在 URL 中用作加载 PNG 的文件夹名称）。
 * @param name 图标的名称（对应于原始 SVG 文件的文件名，不含路径或扩展名）。
 * @return 返回一个 QIcon 对象，其中包含从资源加载的各种尺寸的图标。
 */
QIcon Create(const QString &theme, const QString &name);

/**
 * @brief 有条不紊地将所有 Olive 图标加载到可在整个应用程序中访问的全局变量中。
 *
 * 建议在此处加载所有 UI 图标，以便它们在启动时准备就绪，无需在每次使用时重新加载。
 * @param theme 要加载的图标主题的名称。
 */
void LoadAll(const QString &theme);

}  // namespace olive::icon

#endif  // ICONS_H
