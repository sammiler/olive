#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QStringList>
#include <QVector>
#include <QString> // 为了 QString

#include "common/define.h" // 包含 DISABLE_COPY_MOVE 宏

// 命名空间 olive 通常在外部，但此类没有明确指定命名空间，按原样处理
// namespace olive { (如果此类应在olive命名空间内)

/**
 * @brief 自定义命令行参数解析器。
 *
 * 之所以不使用 Qt 自带的 QCommandLineParser，是因为 QCommandLineParser
 * 在解析任何参数之前就需要一个已创建的 QApplication (或 QCoreApplication) 对象。
 * 然而，我们需要根据用户通过命令行参数的指定来决定创建 QApplication (GUI模式)
 * 还是 QCoreApplication (CLI模式)。因此，需要一个自定义实现，
 * 它可以在没有 Q(Core)Application 存在的情况下解析参数。
 */
class CommandLineParser {
 public:
  /**
   * @brief 析构函数。
   * 负责清理内部动态分配的 Option 和 PositionalArgument 对象。
   */
  ~CommandLineParser();

  /**
   * @brief 禁止 CommandLineParser 对象的拷贝和移动操作。
   */
  DISABLE_COPY_MOVE(CommandLineParser)

  /**
   * @brief 表示一个位置参数的类。
   * 位置参数是那些不以特定标志（如 -o 或 --option）开头的参数，
   * 它们的值取决于它们在命令行参数列表中的位置。
   */
  class PositionalArgument {
   public:
    /**
     * @brief 默认构造函数。
     */
    PositionalArgument() = default;
    /**
     * @brief 虚析构函数，允许派生类正确清理。
     */
    virtual ~PositionalArgument() = default;

    /**
     * @brief 获取此位置参数解析后设置的值。
     * @return const QString& 参数的值。
     */
    [[nodiscard]] const QString& GetSetting() const { return setting_; }

    /**
     * @brief 设置此位置参数的值。
     * 通常由 CommandLineParser 在处理参数时调用。
     * @param s 要设置的参数值。
     */
    void SetSetting(const QString& s) { setting_ = s; }

   private:
    /**
     * @brief 存储解析到的位置参数的值。
     */
    QString setting_;
  };

  /**
   * @brief 表示一个命令行选项的类，继承自 PositionalArgument。
   * 选项通常带有标志（如 -v 或 --verbose），并且可能接受一个参数值。
   * 如果选项不接受参数值，它通常作为一个布尔开关存在。
   */
  class Option : public PositionalArgument {
   public:
    /**
     * @brief 默认构造函数。
     * 初始化时，选项默认为未设置 (is_set_ = false)。
     */
    Option() { is_set_ = false; }

    /**
     * @brief 检查此选项是否在命令行中被设置（出现）。
     * @return bool 如果选项被设置则返回 true，否则返回 false。
     */
    [[nodiscard]] bool IsSet() const { return is_set_; }

    /**
     * @brief 将此选项标记为已在命令行中设置。
     * 通常由 CommandLineParser 在处理参数时调用。
     */
    void Set() { is_set_ = true; }

   private:
    /**
     * @brief 标记此选项是否在命令行中被设置。
     */
    bool is_set_;
  };

  /**
   * @brief 默认构造函数。
   * 初始化一个空的命令行解析器。
   */
  CommandLineParser() = default;

  /**
   * @brief 添加一个命令行选项（例如 -f, --file <文件名>）。
   * @param strings 与此选项关联的字符串列表 (例如 {"f", "file"})。
   * @param description 选项的描述，用于生成帮助信息。
   * @param takes_arg 此选项是否接受一个参数值，默认为 false。
   * @param arg_placeholder 如果选项接受参数，此字符串用作帮助信息中参数的占位符 (例如 "FILE")，默认为空。
   * @param hidden 此选项是否在帮助信息中隐藏，默认为 false。
   * @return const Option* 指向内部创建的 Option 对象的指针，用于后续查询选项状态和值。调用者不拥有此指针。
   */
  const Option* AddOption(const QStringList& strings, const QString& description, bool takes_arg = false,
                          const QString& arg_placeholder = QString(), bool hidden = false);

  /**
   * @brief 添加一个位置参数（例如，输入文件名）。
   * @param name 参数的名称，用于帮助信息。
   * @param description 参数的描述，用于帮助信息。
   * @param required 此位置参数是否必需，默认为 false。
   * @return const PositionalArgument* 指向内部创建的 PositionalArgument 对象的指针，用于后续查询参数值。调用者不拥有此指针。
   */
  const PositionalArgument* AddPositionalArgument(const QString& name, const QString& description,
                                                  bool required = false);

  /**
   * @brief 处理传入的命令行参数。
   * 此方法会根据已添加的选项和位置参数定义来解析 `argv`。
   * @param argv 包含命令行参数的 QVector<QString> (通常来自 main 函数的 argc 和 argv)。
   * 注意：通常第一个参数 (argv[0]) 是程序名称，此解析器可能期望它已被移除或会自行处理。
   */
  void Process(const QVector<QString>& argv);

  /**
   * @brief 打印帮助信息到标准输出。
   * @param filename 程序的文件名 (例如 argv[0])，用于显示在用法摘要中。
   */
  void PrintHelp(const char* filename);

 private:
  /**
   * @brief 内部结构体，用于存储已知的命令行选项及其属性。
   */
  struct KnownOption {
    QStringList args;        ///< @brief 触发此选项的参数字符串列表 (例如 {"-f", "--file"})。
    QString description;     ///< @brief 选项的描述文本。
    Option* option;          ///< @brief 指向关联的 Option 对象的指针，用于存储状态和值。
    bool takes_arg;          ///< @brief 此选项是否接受一个参数值。
    QString arg_placeholder; ///< @brief 如果 takes_arg 为 true，此为参数值的占位符名称。
    bool hidden;             ///< @brief 此选项是否在帮助信息中隐藏。
  };

  /**
   * @brief 内部结构体，用于存储已知的位置参数及其属性。
   */
  struct KnownPositionalArgument {
    QString name;                ///< @brief 位置参数的名称。
    QString description;         ///< @brief 位置参数的描述文本。
    PositionalArgument* option;  ///< @brief 指向关联的 PositionalArgument 对象的指针，用于存储值。
    bool required;               ///< @brief 此位置参数是否是必需的。
  };

  /**
   * @brief 存储所有已定义的命令行选项的 QVector。
   */
  QVector<KnownOption> options_;

  /**
   * @brief 存储所有已定义的位置参数的 QVector。
   */
  QVector<KnownPositionalArgument> positional_args_;
};

// } // namespace olive (如果此类应在olive命名空间内)

#endif  // COMMANDLINEPARSER_H