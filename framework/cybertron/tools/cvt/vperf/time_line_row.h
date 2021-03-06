/******************************************************************************
 * Copyright 2018 The Apollo Authors. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#ifndef TOOLS_CVT_VPERF_TIMELINEROW_H_
#define TOOLS_CVT_VPERF_TIMELINEROW_H_

#include "perf_data_base.h"

#include <QGraphicsItemGroup>
#include <QGraphicsLineItem>
#include <QGraphicsTextItem>
#include <QList>
#include <QMap>
#include <string>

class TimeBlockItem;
class TimeLineScene;
class TimeLineWidget;
class TimeLineTable;

class TimeLineRow : public QGraphicsItemGroup {
 public:
  enum { Type = QGraphicsItemGroup::UserType + QGraphicsItemGroup::Type + 1 };

  explicit TimeLineRow(QGraphicsItem *parent,
                       const QString &rowHeaderText = QString());
  ~TimeLineRow();

  int type() const { return Type; }
  QRectF boundingRect() const override;

  bool hasRowHeaderText(void) const { return _headerText.isVisible(); }
  QString rowHeaderText(void) const { return _headerText.toolTip(); }
  void setRowHeaderText(const std::string &rowName) {
    setRowHeaderText(QString(rowName.c_str()));
  }
  void setRowHeaderText(const QString &rowName);

  void setHeight(qreal h);
  void setHeaderWidth(qreal delta);

  void setWidth(qreal w) {
    QRectF r = _backgroud.rect();
    if (r.width() == w) return;
    r.setWidth(w);
    _backgroud.setRect(r);
  }

  void adjustHeaderTextPos(qreal headerWidth, qreal rowHeight) {
    QRectF rect = _headerText.boundingRect();
    qreal x = (headerWidth - rect.width()) / 2.0f;
    qreal y = (rowHeight - rect.height()) / 2.0f;
    if (x < 0) x = 0.0f;
    if (y < 0) y = 0.0f;

    _headerText.setPos(x, y);
  }

  int taskCount(void) const { return _procDataPtr->taskCount(); }

  void clear(void);

 private:
  TimeLineRow(const TimeLineRow &) = delete;
  TimeLineRow &operator=(const TimeLineRow &) = delete;

  TimeBlockItem *addTimeBlock(const TimeBlockData *);

  void moveTimeBlockLr(qreal offsetX);
  void recalculateTimeBlock(int64_t &base, int timeRatio, int scale,
                            qreal rowHeaderWidth);
  void filterEventId(int eventId);

  void setProcessorData(const ProcessorData *ptr) { _procDataPtr = ptr; }

  //  int _colorIndex;
  const ProcessorData *_procDataPtr;
  QGraphicsTextItem _headerText;
  QGraphicsRectItem _backgroud;

  friend class TimeLineScene;
  friend class TimeLineTable;
  friend class TimeLineWidget;
};

#endif  // TIMELINEROW_H
