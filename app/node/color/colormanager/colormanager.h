

#ifndef COLORSERVICE_H
#define COLORSERVICE_H

#include <QMutex>
#include <memory>

#include "codec/frame.h"
#include "node/node.h"
#include "render/colorprocessor.h"

namespace olive {

class ColorManager : public QObject {
  Q_OBJECT
 public:
  explicit ColorManager(Project* project);

  void Init();

  [[nodiscard]] OCIO::ConstConfigRcPtr GetConfig() const;

  static OCIO::ConstConfigRcPtr CreateConfigFromFile(const QString& filename);

  [[nodiscard]] QString GetConfigFilename() const;

  static OCIO::ConstConfigRcPtr GetDefaultConfig();

  static void SetUpDefaultConfig();

  void SetConfigFilename(const QString& filename) const;

  QStringList ListAvailableDisplays();

  QString GetDefaultDisplay();

  QStringList ListAvailableViews(const QString& display);

  QString GetDefaultView(const QString& display);

  QStringList ListAvailableLooks();

  [[nodiscard]] QStringList ListAvailableColorspaces() const;

  [[nodiscard]] QString GetDefaultInputColorSpace() const;

  void SetDefaultInputColorSpace(const QString& s) const;

  [[nodiscard]] QString GetReferenceColorSpace() const;

  [[nodiscard]] QString GetCompliantColorSpace(const QString& s) const;

  ColorTransform GetCompliantColorSpace(const ColorTransform& transform, bool force_display = false);

  static QStringList ListAvailableColorspaces(const OCIO::ConstConfigRcPtr& config);

  void GetDefaultLumaCoefs(double* rgb) const;

  [[nodiscard]] Project* project() const;

  void UpdateConfigFromFilename();

 signals:
  void ConfigChanged(const QString& s);

  void ReferenceSpaceChanged(const QString& s);

  void DefaultInputChanged(const QString& s);

 private:
  OCIO::ConstConfigRcPtr config_;

  static OCIO::ConstConfigRcPtr default_config_;
};

}  // namespace olive

#endif  // COLORSERVICE_H
