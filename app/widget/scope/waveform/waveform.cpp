#include "waveform.h"

#include <QDebug>
#include <QPainter>
#include <QVector2D>
#include <QVector3D>
#include <QtMath>

#include "common/qtutils.h"
#include "config/config.h"
#include "node/node.h"

namespace olive {

#define super ScopeBase

WaveformScope::WaveformScope(QWidget* parent) : super(parent) {}

ShaderCode WaveformScope::GenerateShaderCode() {
  return ShaderCode(FileFunctions::ReadFileAsString(":/shaders/rgbwaveform.frag"),
                    FileFunctions::ReadFileAsString(":/shaders/rgbwaveform.vert"));
}

void WaveformScope::DrawScope(TexturePtr managed_tex, QVariant pipeline) {
  float waveform_scale = 0.80f;

  // Draw waveform through shader
  ShaderJob job;

  // Set viewport size
  job.Insert(QStringLiteral("viewport"), NodeValue(NodeValue::kVec2, QVector2D(width(), height())));

  // Set luma coefficients
  double luma_coeffs[3] = {0.0f, 0.0f, 0.0f};
  color_manager()->GetDefaultLumaCoefs(luma_coeffs);
  job.Insert(QStringLiteral("luma_coeffs"),
             NodeValue(NodeValue::kVec3, QVector3D(luma_coeffs[0], luma_coeffs[1], luma_coeffs[2])));

  // Scale of the waveform relative to the viewport surface.
  job.Insert(QStringLiteral("waveform_scale"), NodeValue(NodeValue::kFloat, waveform_scale));

  // Insert source texture
  job.Insert(QStringLiteral("ove_maintex"), NodeValue(NodeValue::kTexture, QVariant::fromValue(managed_tex)));

  renderer()->Blit(pipeline, job, GetViewportParams());

  float waveform_dim_x = ceil((width() - 1.0) * waveform_scale);
  float waveform_dim_y = ceil((height() - 1.0) * waveform_scale);
  float waveform_start_dim_x = ((width() - 1.0) - waveform_dim_x) / 2.0f;
  float waveform_start_dim_y = ((height() - 1.0) - waveform_dim_y) / 2.0f;
  float waveform_end_dim_x = (width() - 1.0) - waveform_start_dim_x;

  // Draw line overlays
  QPainter p(paint_device());
  QFont font;
  font.setPixelSize(10);
  QFontMetrics font_metrics = QFontMetrics(font);
  QString label;
  float ire_increment = 0.1f;
  int ire_steps = qRound(1.0 / ire_increment);
  QVector<QLine> ire_lines(ire_steps + 1);
  int font_x_offset = 0;
  int font_y_offset = font_metrics.capHeight() / 2.0f;

  p.setCompositionMode(QPainter::CompositionMode_Plus);

  p.setPen(QColor(0.0, 0.6 * 255.0, 0.0));
  p.setFont(font);

  for (int i = 0; i <= ire_steps; i++) {
    ire_lines[i].setLine(waveform_start_dim_x, (waveform_dim_y * (i * ire_increment)) + waveform_start_dim_y,
                         waveform_end_dim_x, (waveform_dim_y * (i * ire_increment)) + waveform_start_dim_y);
    label = QString::number(1.0 - (i * ire_increment), 'f', 1);
    font_x_offset = QtUtils::QFontMetricsWidth(font_metrics, label) + 4;

    p.drawText(waveform_start_dim_x - font_x_offset,
               (waveform_dim_y * (i * ire_increment)) + waveform_start_dim_y + font_y_offset, label);
  }

  p.drawLines(ire_lines);
}

}  // namespace olive
