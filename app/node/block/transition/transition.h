

#ifndef TRANSITIONBLOCK_H
#define TRANSITIONBLOCK_H

#include "node/block/block.h"

namespace olive {

class ClipBlock;

class TransitionBlock : public Block {
  Q_OBJECT
 public:
  TransitionBlock();

  void Retranslate() override;

  [[nodiscard]] rational in_offset() const;
  [[nodiscard]] rational out_offset() const;

  /**
   * @brief Return the "middle point" of the transition, relative to the transition
   *
   * Used to calculate in/out offsets.
   *
   * 0 means the center of the transition is right in the middle and the in and out offsets will
   * be equal.
   */
  [[nodiscard]] rational offset_center() const;
  void set_offset_center(const rational &r);

  void set_offsets_and_length(const rational &in_offset, const rational &out_offset);

  [[nodiscard]] bool is_dual_transition() const { return connected_out_block() && connected_in_block(); }

  [[nodiscard]] Block *connected_out_block() const;
  [[nodiscard]] Block *connected_in_block() const;

  [[nodiscard]] double GetTotalProgress(const double &time) const;
  [[nodiscard]] double GetOutProgress(const double &time) const;
  [[nodiscard]] double GetInProgress(const double &time) const;

  void Value(const NodeValueRow &value, const NodeGlobals &globals, NodeValueTable *table) const override;

  void InvalidateCache(const TimeRange &range, const QString &from, int element = -1,
                       InvalidateCacheOptions options = InvalidateCacheOptions()) override;

  static const QString kOutBlockInput;
  static const QString kInBlockInput;
  static const QString kCurveInput;
  static const QString kCenterInput;

 protected:
  virtual void ShaderJobEvent(const NodeValueRow &value, ShaderJob *job) const {}

  virtual void SampleJobEvent(const SampleBuffer &from_samples, const SampleBuffer &to_samples,
                              SampleBuffer &out_samples, double time_in) const {}

  [[nodiscard]] double TransformCurve(double linear) const;

  void InputConnectedEvent(const QString &input, int element, Node *output) override;

  void InputDisconnectedEvent(const QString &input, int element, Node *output) override;

  [[nodiscard]] TimeRange InputTimeAdjustment(const QString &input, int element, const TimeRange &input_time,
                                              bool clamp) const override;

  [[nodiscard]] TimeRange OutputTimeAdjustment(const QString &input, int element,
                                               const TimeRange &input_time) const override;

 private:
  enum CurveType { kLinear, kExponential, kLogarithmic };

  [[nodiscard]] static double GetInternalTransitionTime(const double &time);

  void InsertTransitionTimes(AcceleratedJob *job, const double &time) const;

  ClipBlock *connected_out_block_;

  ClipBlock *connected_in_block_;
};

}  // namespace olive

#endif  // TRANSITIONBLOCK_H
