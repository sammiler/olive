#include "value.h"
#include <qchar.h>

#include <QCoreApplication>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>
#include <QVector4D>
#include <string>

#include "common/tohex.h"
#include "render/subtitleparams.h"
#include "render/videoparams.h"

namespace olive {

QString NodeValue::ValueToString(Type data_type, const QVariant &value, bool value_is_a_key_track) {
  if (!value_is_a_key_track && data_type == kVec2) {
    auto vec = value.value<QVector2D>();

    return QStringLiteral("%1:%2").arg(QString::number(vec.x()), QString::number(vec.y()));
  } else if (!value_is_a_key_track && data_type == kVec3) {
    auto vec = value.value<QVector3D>();

    return QStringLiteral("%1:%2:%3").arg(QString::number(vec.x()), QString::number(vec.y()), QString::number(vec.z()));
  } else if (!value_is_a_key_track && data_type == kVec4) {
    auto vec = value.value<QVector4D>();

    return QStringLiteral("%1:%2:%3:%4")
        .arg(QString::number(vec.x()), QString::number(vec.y()), QString::number(vec.z()), QString::number(vec.w()));
  } else if (!value_is_a_key_track && data_type == kColor) {
    auto c = value.value<Color>();

    return QStringLiteral("%1:%2:%3:%4")
        .arg(QString::number(c.red()), QString::number(c.green()), QString::number(c.blue()),
             QString::number(c.alpha()));
  } else if (!value_is_a_key_track && data_type == kBezier) {
    auto b = value.value<Bezier>();

    return QStringLiteral("%1:%2:%3:%4:%5:%6")
        .arg(QString::number(b.x()), QString::number(b.y()), QString::number(b.cp1_x()), QString::number(b.cp1_y()),
             QString::number(b.cp2_x()), QString::number(b.cp2_y()));
  } else if (data_type == kRational) {
    return QString::fromStdString(value.value<rational>().toString());
  } else if (data_type == kTexture || data_type == kSamples || data_type == kNone) {
    // These data types need no XML representation
    return {};
  } else if (data_type == kInt) {
    return QString::number(value.value<int64_t>());
  } else if (data_type == kBinary) {
    return value.toByteArray().toBase64();
  } else {
    if (value.canConvert<QString>()) {
      return value.toString();
    }

    if (!value.isNull()) {
      qWarning() << "Failed to convert type" << ToHex(data_type) << "to string";
    }

    return {};
  }
}

QVector<QVariant> NodeValue::split_normal_value_into_track_values(Type type, const QVariant &value) {
  QVector<QVariant> vals(get_number_of_keyframe_tracks(type));

  switch (type) {
    case kVec2: {
      auto vec = value.value<QVector2D>();
      vals.replace(0, vec.x());
      vals.replace(1, vec.y());
      break;
    }
    case kVec3: {
      auto vec = value.value<QVector3D>();
      vals.replace(0, vec.x());
      vals.replace(1, vec.y());
      vals.replace(2, vec.z());
      break;
    }
    case kVec4: {
      auto vec = value.value<QVector4D>();
      vals.replace(0, vec.x());
      vals.replace(1, vec.y());
      vals.replace(2, vec.z());
      vals.replace(3, vec.w());
      break;
    }
    case kColor: {
      auto c = value.value<Color>();
      vals.replace(0, c.red());
      vals.replace(1, c.green());
      vals.replace(2, c.blue());
      vals.replace(3, c.alpha());
      break;
    }
    case kBezier: {
      auto b = value.value<Bezier>();
      vals.replace(0, b.x());
      vals.replace(1, b.y());
      vals.replace(2, b.cp1_x());
      vals.replace(3, b.cp1_y());
      vals.replace(4, b.cp2_x());
      vals.replace(5, b.cp2_y());
      break;
    }
    default:
      vals.replace(0, value);
  }

  return vals;
}

QVariant NodeValue::combine_track_values_into_normal_value(Type type, const QVector<QVariant> &split) {
  if (split.isEmpty()) {
    return {};
  }

  switch (type) {
    case kVec2: {
      return QVector2D(split.at(0).toFloat(), split.at(1).toFloat());
    }
    case kVec3: {
      return QVector3D(split.at(0).toFloat(), split.at(1).toFloat(), split.at(2).toFloat());
    }
    case kVec4: {
      return QVector4D(split.at(0).toFloat(), split.at(1).toFloat(), split.at(2).toFloat(), split.at(3).toFloat());
    }
    case kColor: {
      return QVariant::fromValue(
          Color(split.at(0).toFloat(), split.at(1).toFloat(), split.at(2).toFloat(), split.at(3).toFloat()));
    }
    case kBezier:
      return QVariant::fromValue(Bezier(split.at(0).toDouble(), split.at(1).toDouble(), split.at(2).toDouble(),
                                        split.at(3).toDouble(), split.at(4).toDouble(), split.at(5).toDouble()));
    default:
      return split.first();
  }
}

int NodeValue::get_number_of_keyframe_tracks(Type type) {
  switch (type) {
    case NodeValue::kVec2:
      return 2;
    case NodeValue::kVec3:
      return 3;
    case NodeValue::kVec4:
    case NodeValue::kColor:
      return 4;
    case NodeValue::kBezier:
      return 6;
    default:
      return 1;
  }
}

QVariant NodeValue::StringToValue(Type data_type, const QString &string, bool value_is_a_key_track) {
  if (!value_is_a_key_track && data_type == kVec2) {
    QStringList vals = string.split(':');

    ValidateVectorString(&vals, 2);

    return QVector2D(vals.at(0).toFloat(), vals.at(1).toFloat());
  } else if (!value_is_a_key_track && data_type == kVec3) {
    QStringList vals = string.split(':');

    ValidateVectorString(&vals, 3);

    return QVector3D(vals.at(0).toFloat(), vals.at(1).toFloat(), vals.at(2).toFloat());
  } else if (!value_is_a_key_track && data_type == kVec4) {
    QStringList vals = string.split(':');

    ValidateVectorString(&vals, 4);

    return QVector4D(vals.at(0).toFloat(), vals.at(1).toFloat(), vals.at(2).toFloat(), vals.at(3).toFloat());
  } else if (!value_is_a_key_track && data_type == kColor) {
    QStringList vals = string.split(':');

    ValidateVectorString(&vals, 4);

    return QVariant::fromValue(
        Color(vals.at(0).toDouble(), vals.at(1).toDouble(), vals.at(2).toDouble(), vals.at(3).toDouble()));
  } else if (!value_is_a_key_track && data_type == kBezier) {
    QStringList vals = string.split(':');

    ValidateVectorString(&vals, 6);

    return QVariant::fromValue(Bezier(vals.at(0).toDouble(), vals.at(1).toDouble(), vals.at(2).toDouble(),
                                      vals.at(3).toDouble(), vals.at(4).toDouble(), vals.at(5).toDouble()));
  } else if (data_type == kInt) {
    return QVariant::fromValue(string.toLongLong());
  } else if (data_type == kRational) {
    QString str11("1223124");
    std::string test1 = str11.toUtf8().constData();
    std::string test = string.toUtf8().constData();
    return QVariant::fromValue(rational::fromString(string.toUtf8().constData()));
  } else if (data_type == kBinary) {
    return QByteArray::fromBase64(string.toLatin1());
  } else {
    return string;
  }
}

void NodeValue::ValidateVectorString(QStringList *list, int count) {
  while (list->size() < count) {
    list->append(QStringLiteral("0"));
  }
}

QString NodeValue::GetPrettyDataTypeName(Type type) {
  switch (type) {
    case kNone:
      return QCoreApplication::translate("NodeValue", "None");
    case kInt:
    case kCombo:
      return QCoreApplication::translate("NodeValue", "Integer");
    case kFloat:
      return QCoreApplication::translate("NodeValue", "Float");
    case kRational:
      return QCoreApplication::translate("NodeValue", "Rational");
    case kBoolean:
      return QCoreApplication::translate("NodeValue", "Boolean");
    case kColor:
      return QCoreApplication::translate("NodeValue", "Color");
    case kMatrix:
      return QCoreApplication::translate("NodeValue", "Matrix");
    case kText:
      return QCoreApplication::translate("NodeValue", "Text");
    case kFont:
      return QCoreApplication::translate("NodeValue", "Font");
    case kFile:
      return QCoreApplication::translate("NodeValue", "File");
    case kTexture:
      return QCoreApplication::translate("NodeValue", "Texture");
    case kSamples:
      return QCoreApplication::translate("NodeValue", "Samples");
    case kVec2:
      return QCoreApplication::translate("NodeValue", "Vector 2D");
    case kVec3:
      return QCoreApplication::translate("NodeValue", "Vector 3D");
    case kVec4:
      return QCoreApplication::translate("NodeValue", "Vector 4D");
    case kBezier:
      return QCoreApplication::translate("NodeValue", "Bezier");
    case kVideoParams:
      return QCoreApplication::translate("NodeValue", "Video Parameters");
    case kAudioParams:
      return QCoreApplication::translate("NodeValue", "Audio Parameters");
    case kSubtitleParams:
      return QCoreApplication::translate("NodeValue", "Subtitle Parameters");
    case kBinary:
      return QCoreApplication::translate("NodeValue", "Binary");

    case kDataTypeCount:
      break;
  }

  return QCoreApplication::translate("NodeValue", "Unknown");
}

QString NodeValue::GetDataTypeName(Type type) {
  switch (type) {
    case kNone:
      return QStringLiteral("none");
    case kInt:
      return QStringLiteral("int");
    case kCombo:
      return QStringLiteral("combo");
    case kFloat:
      return QStringLiteral("float");
    case kRational:
      return QStringLiteral("rational");
    case kBoolean:
      return QStringLiteral("bool");
    case kColor:
      return QStringLiteral("color");
    case kMatrix:
      return QStringLiteral("matrix");
    case kText:
      return QStringLiteral("text");
    case kFont:
      return QStringLiteral("font");
    case kFile:
      return QStringLiteral("file");
    case kTexture:
      return QStringLiteral("texture");
    case kSamples:
      return QStringLiteral("samples");
    case kVec2:
      return QStringLiteral("vec2");
    case kVec3:
      return QStringLiteral("vec3");
    case kVec4:
      return QStringLiteral("vec4");
    case kBezier:
      return QStringLiteral("bezier");
    case kVideoParams:
      return QStringLiteral("vparam");
    case kAudioParams:
      return QStringLiteral("aparam");
    case kSubtitleParams:
      return QStringLiteral("sparam");
    case kBinary:
      return QStringLiteral("binary");
    case kDataTypeCount:
      break;
  }

  return {};
}

NodeValue::Type NodeValue::GetDataTypeFromName(const QString &n) {
  // Slow but easy to maintain
  for (int i = 0; i < kDataTypeCount; i++) {
    Type t = static_cast<Type>(i);
    if (GetDataTypeName(t) == n) {
      return t;
    }
  }

  return NodeValue::kNone;
}

NodeValue NodeValueTable::Get(const QVector<NodeValue::Type> &type, const QString &tag) const {
  int value_index = GetValueIndex(type, tag);

  if (value_index >= 0) {
    return values_.at(value_index);
  }

  return {};
}

NodeValue NodeValueTable::Take(const QVector<NodeValue::Type> &type, const QString &tag) {
  int value_index = GetValueIndex(type, tag);

  if (value_index >= 0) {
    return values_.takeAt(value_index);
  }

  return {};
}

bool NodeValueTable::Has(NodeValue::Type type) const {
  for (int i = values_.size() - 1; i >= 0; i--) {
    const NodeValue &v = values_.at(i);

    if (v.type() & type) {
      return true;
    }
  }

  return false;
}

void NodeValueTable::Remove(const NodeValue &v) {
  for (int i = values_.size() - 1; i >= 0; i--) {
    const NodeValue &compare = values_.at(i);

    if (compare == v) {
      values_.removeAt(i);
      return;
    }
  }
}

NodeValueTable NodeValueTable::Merge(QList<NodeValueTable> tables) {
  if (tables.size() == 1) {
    return tables.first();
  }

  int row = 0;

  NodeValueTable merged_table;

  // Slipstreams all tables together
  while (true) {
    bool all_merged = true;

    foreach (const NodeValueTable &t, tables) {
      if (row < t.Count()) {
        all_merged = false;
      } else {
        continue;
      }

      int row_index = t.Count() - 1 - row;

      merged_table.Prepend(t.at(row_index));
    }

    row++;

    if (all_merged) {
      break;
    }
  }

  return merged_table;
}

int NodeValueTable::GetValueIndex(const QVector<NodeValue::Type> &types, const QString &tag) const {
  int index = -1;

  for (int i = values_.size() - 1; i >= 0; i--) {
    const NodeValue &v = values_.at(i);

    if (types.contains(v.type())) {
      index = i;

      if (tag.isEmpty() || tag == v.tag()) {
        break;
      }
    }
  }

  return index;
}

}  // namespace olive
