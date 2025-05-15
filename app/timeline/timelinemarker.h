#ifndef TIMELINEMARKER_H
#define TIMELINEMARKER_H

#include <olive/core/core.h>
#include <QPainter>
#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "undo/undocommand.h"

using namespace olive::core;

namespace olive {

class TimelineMarker : public QObject {
  Q_OBJECT
 public:
  explicit TimelineMarker(QObject* parent = nullptr);
  TimelineMarker(int color, const TimeRange& time, QString name = QString(), QObject* parent = nullptr);

  [[nodiscard]] const TimeRange& time() const { return time_; }
  void set_time(const TimeRange& time);
  void set_time(const rational& time);

  [[nodiscard]] bool has_sibling_at_time(const rational& t) const;

  [[nodiscard]] const QString& name() const { return name_; }
  void set_name(const QString& name);

  [[nodiscard]] int color() const { return color_; }
  void set_color(int c);

  static int GetMarkerHeight(const QFontMetrics& fm);
  QRect Draw(QPainter* p, const QPoint& pt, int max_right, double scale, bool selected);

  bool load(QXmlStreamReader* reader);
  void save(QXmlStreamWriter* writer) const;

 signals:
  void TimeChanged(const TimeRange& time);

  void NameChanged(const QString& name);

  void ColorChanged(int c);

 private:
  TimeRange time_;

  QString name_;

  int color_;
};

class TimelineMarkerList : public QObject {
  Q_OBJECT
 public:
  explicit TimelineMarkerList(QObject* parent = nullptr) : QObject(parent) {}

  [[nodiscard]] inline bool empty() const { return markers_.empty(); }
  inline std::vector<TimelineMarker*>::iterator begin() { return markers_.begin(); }
  inline std::vector<TimelineMarker*>::iterator end() { return markers_.end(); }
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator cbegin() const { return markers_.cbegin(); }
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator cend() const { return markers_.cend(); }
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator begin() const { return markers_.begin(); }
  [[nodiscard]] inline std::vector<TimelineMarker*>::const_iterator end() const { return markers_.end(); }

  [[nodiscard]] inline TimelineMarker* back() const { return markers_.back(); }
  [[nodiscard]] inline TimelineMarker* front() const { return markers_.front(); }
  [[nodiscard]] inline size_t size() const { return markers_.size(); }

  bool load(QXmlStreamReader* reader);
  void save(QXmlStreamWriter* writer) const;

  [[nodiscard]] TimelineMarker* GetMarkerAtTime(const rational& t) const {
    for (auto m : markers_) {
      if (m->time().in() == t) {
        return m;
      }
    }

    return nullptr;
  }

  [[nodiscard]] TimelineMarker* GetClosestMarkerToTime(const rational& t) const {
    TimelineMarker* closest = nullptr;

    for (auto m : markers_) {
      rational this_diff = rational::qAbs(m->time().in() - t);

      if (closest) {
        rational stored_diff = rational::qAbs(closest->time().in() - t);

        if (this_diff > stored_diff) {
          // Since the list is organized by time, if the diff increases, assume we are only going
          // to move further away from here and there's no need to check
          break;
        }
      }

      closest = m;
    }

    return closest;
  }

 signals:
  void MarkerAdded(TimelineMarker* marker);

  void MarkerRemoved(TimelineMarker* marker);

  void MarkerModified(TimelineMarker* marker);

 protected:
  void childEvent(QChildEvent* e) override;

 private:
  void InsertIntoList(TimelineMarker* m);
  bool RemoveFromList(TimelineMarker* m);

  std::vector<TimelineMarker*> markers_;

 private slots:
  void HandleMarkerModification();

  void HandleMarkerTimeChange();
};

class MarkerAddCommand : public UndoCommand {
 public:
  MarkerAddCommand(TimelineMarkerList* marker_list, const TimeRange& range, const QString& name, int color);
  MarkerAddCommand(TimelineMarkerList* marker_list, TimelineMarker* marker);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  TimelineMarkerList* marker_list_;

  TimelineMarker* added_marker_;
  QObject memory_manager_;
};

class MarkerRemoveCommand : public UndoCommand {
 public:
  explicit MarkerRemoveCommand(TimelineMarker* marker);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  TimelineMarker* marker_;
  QObject* marker_list_{};

  QObject memory_manager_;
};

class MarkerChangeColorCommand : public UndoCommand {
 public:
  MarkerChangeColorCommand(TimelineMarker* marker, int new_color);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  TimelineMarker* marker_;
  int old_color_{};
  int new_color_;
};

class MarkerChangeNameCommand : public UndoCommand {
 public:
  MarkerChangeNameCommand(TimelineMarker* marker, QString name);

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  TimelineMarker* marker_;
  QString old_name_;
  QString new_name_;
};

class MarkerChangeTimeCommand : public UndoCommand {
 public:
  MarkerChangeTimeCommand(TimelineMarker* marker, const TimeRange& time, const TimeRange& old_time);
  MarkerChangeTimeCommand(TimelineMarker* marker, const TimeRange& time)
      : MarkerChangeTimeCommand(marker, time, marker->time()) {}

  [[nodiscard]] Project* GetRelevantProject() const override;

 protected:
  void redo() override;
  void undo() override;

 private:
  TimelineMarker* marker_;
  TimeRange old_time_;
  TimeRange new_time_;
};

}  // namespace olive

#endif  // TIMELINEMARKER_H
