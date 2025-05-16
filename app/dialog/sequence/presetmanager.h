#ifndef PRESETMANAGER_H
#define PRESETMANAGER_H

#include <QCoreApplication>  // Qt核心应用类，用于国际化翻译等
#include <QDir>              // 目录操作类
#include <QFile>             // 文件操作类
#include <QInputDialog>      // 输入对话框类
#include <QMessageBox>       // 消息对话框类
#include <QObject>           // Qt对象模型基类
#include <QXmlStreamReader>  // XML流读取类
#include <QXmlStreamWriter>  // XML流写入类
#include <memory>            // C++智能指针库
#include <utility>           // C++实用工具库，例如 std::move

#include "common/define.h"         // 项目通用定义
#include "common/filefunctions.h"  // 文件操作相关函数
#include "common/xmlutils.h"       // XML处理工具函数

namespace olive {

/**
 * @class Preset
 * @brief 预设的基类。
 *
 * 定义了所有具体预设类需要实现的基本接口，
 * 包括获取/设置名称以及加载/保存预设数据。
 */
class Preset {
 public:
  /**
   * @brief 默认构造函数。
   */
  Preset() = default;

  /**
   * @brief 默认虚析构函数。
   * 确保派生类的析构函数能够被正确调用。
   */
  virtual ~Preset() = default;

  /**
   * @brief 获取预设的名称。
   * @return const QString& 预设名称的常量引用。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] const QString& GetName() const { return name_; }

  /**
   * @brief 设置预设的名称。
   * @param s 要设置的预设名称。
   */
  void SetName(const QString& s) { name_ = s; }

  /**
   * @brief 从 XML 流加载预设数据。
   *
   * 纯虚函数，需要在派生类中实现具体的加载逻辑。
   * @param reader 指向 QXmlStreamReader 对象的指针，用于读取 XML 数据。
   */
  virtual void Load(QXmlStreamReader* reader) = 0;

  /**
   * @brief 将预设数据保存到 XML 流。
   *
   * 纯虚函数，需要在派生类中实现具体的保存逻辑。
   * @param writer 指向 QXmlStreamWriter 对象的指针，用于写入 XML 数据。
   */
  virtual void Save(QXmlStreamWriter* writer) const = 0;

 private:
  /**
   * @brief 预设的名称。
   */
  QString name_;
};

/**
 * @typedef PresetPtr
 * @brief Preset 对象的共享指针类型别名。
 *
 * 使用 std::shared_ptr 来管理 Preset 对象的生命周期。
 */
using PresetPtr = std::shared_ptr<Preset>;

/**
 * @class PresetManager
 * @brief 预设管理器模板类。
 *
 * 此类用于管理特定类型 T 的预设集合，T 必须是 Preset 的派生类。
 * 它负责从文件加载预设、保存预设到文件、获取、删除和管理预设数据。
 * @tparam T 预设的具体类型，必须继承自 Preset。
 */
template <typename T>
class PresetManager {
 public:
  /**
   * @brief PresetManager 构造函数。
   *
   * @param parent 父 QWidget 对象指针，主要用于显示对话框。
   * @param preset_name 预设文件的名称（不包含路径），用于区分不同类型的预设。
   */
  PresetManager(QWidget* parent, QString preset_name) : preset_name_(std::move(preset_name)), parent_(parent) {
    // 从文件加载自定义预设数据
    QFile preset_file(GetCustomPresetFilename());  // 获取预设文件的完整路径
    if (preset_file.open(QFile::ReadOnly)) {       // 以只读方式打开文件
      QXmlStreamReader reader(&preset_file);       // 创建 XML 流读取器

      // 循环读取 XML 文档
      while (XMLReadNextStartElement(&reader)) {           // XMLReadNextStartElement 是一个辅助函数，用于跳过非元素节点
        if (reader.name() == QStringLiteral("presets")) {  // 查找根元素 "presets"
          while (XMLReadNextStartElement(&reader)) {
            if (reader.name() == QStringLiteral("preset")) {  // 查找每个 "preset" 元素
              PresetPtr p = std::make_unique<T>();            // 创建一个具体类型 T 的预设对象

              p->Load(&reader);  // 调用预设对象的 Load 方法从 XML 加载数据

              custom_preset_data_.append(p);  // 将加载的预设添加到列表中
            } else {
              reader.skipCurrentElement();  // 跳过不认识的元素
            }
          }
        } else {
          reader.skipCurrentElement();  // 跳过根元素之外的其他元素
        }
      }

      preset_file.close();  // 关闭文件
    }
  }

  /**
   * @brief PresetManager 析构函数。
   *
   * 在对象销毁时，将所有自定义预设数据保存到磁盘文件。
   */
  ~PresetManager() {
    // 将自定义预设保存到磁盘
    QFile preset_file(GetCustomPresetFilename());
    if (preset_file.open(QFile::WriteOnly)) {  // 以只写方式打开文件
      QXmlStreamWriter writer(&preset_file);   // 创建 XML 流写入器
      writer.setAutoFormatting(true);          // 设置自动格式化，使输出的 XML 更易读

      writer.writeStartDocument();  // 写入 XML 文档开始标记

      writer.writeStartElement(QStringLiteral("presets"));  // 写入根元素 "presets"

      // 遍历所有自定义预设并保存
      foreach (PresetPtr p, custom_preset_data_) {
        writer.writeStartElement(QStringLiteral("preset"));  // 写入 "preset" 元素开始标记

        p->Save(&writer);  // 调用预设对象的 Save 方法将数据写入 XML

        writer.writeEndElement();  // 结束 "preset" 元素
      }

      writer.writeEndElement();  // 结束 "presets" 元素

      writer.writeEndDocument();  // 写入 XML 文档结束标记

      preset_file.close();  // 关闭文件
    }
  }

  /**
   * @brief 获取用户输入的预设名称。
   *
   * 弹出一个输入对话框，让用户输入或确认预设的名称。
   * @param start 对话框中预填充的初始名称。
   * @return QString 用户输入的预设名称。如果用户取消对话框，则返回空字符串。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] QString GetPresetName(QString start) const {
    bool ok;  // 用于接收输入对话框的返回状态

    forever {  // 无限循环，直到获得有效名称或用户取消
      // 显示文本输入对话框
      start = QInputDialog::getText(parent_, QCoreApplication::translate("PresetManager", "Save Preset"),  // 对话框标题
                                    QCoreApplication::translate("PresetManager", "Set preset name:"),      // 提示标签
                                    QLineEdit::Normal,                                                     // 输入框模式
                                    start,                                                                 // 默认文本
                                    &ok);  // 获取用户操作结果 (确定/取消)

      if (!ok) {
        // 用户取消了对话框 - 完全退出函数
        return {};  // 返回空 QString
      }

      if (start.isEmpty()) {
        // 未输入预设名称，重新开始循环
        QMessageBox::critical(parent_,
                              QCoreApplication::translate("PresetManager", "Invalid preset name"),  // 错误对话框标题
                              QCoreApplication::translate("PresetManager", "You must enter a preset name"),  // 错误消息
                              QMessageBox::Ok);
      } else {
        break;  // 名称有效，跳出循环
      }
    }

    return start;  // 返回有效的预设名称
  }

  /**
   * @enum SaveStatus
   * @brief 保存预设操作的状态。
   */
  enum SaveStatus {
    kAppended,  ///< 预设已成功添加。
    kReplaced,  ///< 同名预设已被替换。
    kNotSaved   ///< 预设未保存（例如用户取消）。
  };

  /**
   * @brief 保存一个预设。
   *
   * 此函数会提示用户输入预设名称。如果已存在同名预设，会询问用户是否替换。
   * @param preset 要保存的预设对象的共享指针。
   * @return SaveStatus 保存操作的状态。
   */
  SaveStatus SavePreset(const PresetPtr& preset) {
    QString preset_name_str;    // 用于存储用户输入的预设名称，避免与成员变量 preset_name_ 混淆
    int existing_preset_index;  // 用于存储已存在预设的索引

    forever {                                            // 无限循环，直到获得有效操作或用户取消
      preset_name_str = GetPresetName(preset_name_str);  // 获取预设名称

      if (preset_name_str.isEmpty()) {
        // 用户取消了对话框 - 完全退出函数
        return kNotSaved;
      }

      // 检查是否存在同名预设
      existing_preset_index = -1;
      for (int i = 0; i < custom_preset_data_.size(); i++) {
        if (custom_preset_data_.at(i)->GetName() == preset_name_str) {
          existing_preset_index = i;
          break;
        }
      }

      // 如果不存在同名预设，或者用户同意替换现有预设，则跳出循环
      if (existing_preset_index == -1 ||
          QMessageBox::question(parent_,
                                QCoreApplication::translate("PresetManager", "Preset exists"),  // 询问对话框标题
                                QCoreApplication::translate("PresetManager",                    // 询问消息
                                                            "A preset with this name already exists. "
                                                            "Would you like to replace it?")) == QMessageBox::Yes) {
        break;
      }
      // 如果用户选择不替换，则循环将继续，提示用户输入新的名称
    }

    preset->SetName(preset_name_str);  // 设置预设对象的名称

    if (existing_preset_index >= 0) {
      // 如果预设已存在，则替换它
      custom_preset_data_.replace(existing_preset_index, preset);
      return kReplaced;
    } else {
      // 否则，追加新的预设
      custom_preset_data_.append(preset);
      return kAppended;
    }
  }

  /**
   * @brief 获取自定义预设文件的完整路径。
   * @return QString 预设文件的路径。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] QString GetCustomPresetFilename() const {
    // FileFunctions::GetConfigurationLocation() 获取应用程序配置目录
    // preset_name_ 是在构造函数中传入的文件名（不含路径）
    return QDir(FileFunctions::GetConfigurationLocation()).filePath(preset_name_);
  }

  /**
   * @brief 根据索引获取一个预设。
   * @param index 预设在列表中的索引。
   * @return PresetPtr 指向请求的预设对象的共享指针。如果索引无效，行为未定义（通常会导致越界访问）。
   */
  PresetPtr GetPreset(int index) { return custom_preset_data_.at(index); }

  /**
   * @brief 根据索引删除一个预设。
   * @param index 要删除的预设在列表中的索引。
   */
  void DeletePreset(int index) { custom_preset_data_.removeAt(index); }

  /**
   * @brief 获取当前加载的自定义预设的数量。
   * @return int 预设的数量。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] int GetNumberOfPresets() const { return custom_preset_data_.size(); }

  /**
   * @brief 获取所有自定义预设数据的常量引用。
   * @return const QVector<PresetPtr>& 指向预设数据 QVector 的常量引用。
   * [[nodiscard]] 属性提示编译器调用者应该使用此函数的返回值。
   */
  [[nodiscard]] const QVector<PresetPtr>& GetPresetData() const { return custom_preset_data_; }

 private:
  /**
   * @brief 存储所有自定义预设数据的 QVector 容器。
   * 每个元素都是一个指向 Preset 派生类对象的共享指针。
   */
  QVector<PresetPtr> custom_preset_data_;

  /**
   * @brief 预设文件的名称（不包含路径）。
   * 用于构造完整的预设文件路径，并区分不同类型的预设。
   */
  QString preset_name_;

  /**
   * @brief 指向父 QWidget 对象的指针。
   * 主要用于在需要时显示对话框（如输入名称、确认替换等）。
   */
  QWidget* parent_;
};

}  // namespace olive

#endif  // PRESETMANAGER_H
