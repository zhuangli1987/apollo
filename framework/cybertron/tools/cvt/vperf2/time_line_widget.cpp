/******************************************************************************
 * Copyright 2017 The Apollo Authors. All Rights Reserved.
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

#include "time_line_widget.h"
#include <QWheelEvent>
#include <iostream>
#include "time_block_item.h"
#include "time_line_legend.h"
#include "time_line_row.h"
#include "time_line_table.h"
#include "ui_find_timelength.h"
#include "ui_find_timepoint.h"
#include "ui_find_widget.h"
#include "ui_time_line_widget.h"

TimeLineWidget::TimeLineWidget(QWidget *parent)
    : QWidget(parent),
      ui(new Ui::TimeLineWidget),
      _scene(new TimeLineScene),
      _resultIndex(0),
      _currentIndex(-1),
      _findResults() {
  ui->setupUi(this);
  ui->findWidget->setVisible(false);

  ui->graphicsView->setScene(_scene);

  ui->timeHScrollBar->setValue(0);
  ui->timeHScrollBar->setMinimum(0);

  connect(ui->timeHScrollBar, SIGNAL(valueChanged(int)), this,
          SLOT(lrScroll(int)));
  connect(ui->graphicsView,
          SIGNAL(viewDoubleClicked(QGraphicsItem *, QPointF &)), this,
          SLOT(viewDoubleClicked(QGraphicsItem *, QPointF &)));

  connect(ui->findWidget, SIGNAL(isHiding()), this, SLOT(clearFindResults()));

  connect(ui->findWidget->timeLengthUi->nextButton, SIGNAL(clicked(bool)), this,
          SLOT(findTimeLength()));
  connect(ui->findWidget->timeLengthUi->preButton, SIGNAL(clicked(bool)), this,
          SLOT(findTimeLength()));
  connect(ui->findWidget->timeLengthUi->procComboBox,
          SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
  connect(ui->findWidget->timeLengthUi->operatorComboBox,
          SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
  connect(ui->findWidget->timeLengthUi->timeSpinBox, SIGNAL(valueChanged(int)),
          this, SLOT(clearFindResults()));
}

TimeLineWidget::~TimeLineWidget() { delete ui; }

void TimeLineWidget::resizeEvent(QResizeEvent *e) {
  QWidget::resizeEvent(e);
  ui->graphicsView->setSceneRect(0, 0, ui->graphicsView->width() - 2,
                                 ui->graphicsView->height());
  ui->timeHScrollBar->setMaximum(_scene->sceneWidth() - width() +
                                 _scene->rowHeaderWidth());
  lrScroll(ui->timeHScrollBar->value());

  _scene->destroyTimeGrid();
  _scene->createTimeGrid();
}

void TimeLineWidget::wheelEvent(QWheelEvent *e) {
  QWidget::wheelEvent(e);

  int scrollDeltaValue = -e->delta();
  int absValue = std::abs(scrollDeltaValue);
  absValue /= 10;
  if (absValue == 0) absValue = 1;
  if (scrollDeltaValue < 0) absValue = -absValue;

  absValue += ui->timeHScrollBar->value();
  if (absValue < 0) absValue = 0;
  ui->timeHScrollBar->setValue(absValue);
}

void TimeLineWidget::lrScroll(int v) {
  _scene->keepStill(v);
  ui->graphicsView->setSceneRect(v, 0, ui->graphicsView->width() - 2,
                                 ui->graphicsView->height());
}

void TimeLineWidget::setTimeRatio(int tr) { _scene->setTimeRatio(tr); }
void TimeLineWidget::setSceneScale(int s) {
  _scene->setSceneScale(s);  
  ui->timeHScrollBar->setMaximum(_scene->sceneWidth() - width() +
                                 _scene->rowHeaderWidth());
}

void TimeLineWidget::setSceneSparsity(int s) { _scene->setSceneSparsity(s); }

void TimeLineWidget::setRowHeaderWidth(int w) { _scene->setRowHeaderWidth(w); }

void TimeLineWidget::setRowHeight(int h) { _scene->setRowHeight(h); }

void TimeLineWidget::viewDoubleClicked(QGraphicsItem *item, QPointF &scenePos) {
  _scene->sceneDoubleClicked(item, scenePos);
}

void TimeLineWidget::showFindWindow(void) {
  ui->findWidget->setVisible(true);
  if (ui->findWidget->timePointUi->nextButton) {
    disconnect(ui->findWidget->timePointUi->nextButton, SIGNAL(clicked(bool)),
               this, SLOT(findTimeLength()));
    disconnect(ui->findWidget->timePointUi->preButton, SIGNAL(clicked(bool)),
               this, SLOT(findTimeLength()));
    disconnect(ui->findWidget->timePointUi->procComboBox,
               SIGNAL(currentIndexChanged(int)), this,
               SLOT(clearFindResults()));
    disconnect(ui->findWidget->timePointUi->typeComboBox,
               SIGNAL(currentIndexChanged(int)), this,
               SLOT(clearFindResults()));
    disconnect(ui->findWidget->timePointUi->timePointLineEdit,
               SIGNAL(textChanged(QString)), this, SLOT(clearFindResults()));
  }

  FindWidget::FindMode m = ui->findWidget->findMode();
  ui->findWidget->switchFindMode(FindWidget::FindTimeLengthMode);
  ui->findWidget->setProcessorCount(_scene->processorCount());
  if (m != FindWidget::FindTimeLengthMode) {
    connect(ui->findWidget->timeLengthUi->nextButton, SIGNAL(clicked(bool)),
            this, SLOT(findTimeLength()));
    connect(ui->findWidget->timeLengthUi->preButton, SIGNAL(clicked(bool)),
            this, SLOT(findTimeLength()));
    connect(ui->findWidget->timeLengthUi->procComboBox,
            SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
    connect(ui->findWidget->timeLengthUi->operatorComboBox,
            SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
    connect(ui->findWidget->timeLengthUi->timeSpinBox,
            SIGNAL(valueChanged(int)), this, SLOT(clearFindResults()));

    clearFindResults();
  }
}

void TimeLineWidget::showFindTimePointWindow(void) {
  ui->findWidget->setVisible(true);

  if (ui->findWidget->timeLengthUi->nextButton) {
    disconnect(ui->findWidget->timeLengthUi->nextButton, SIGNAL(clicked(bool)),
               this, SLOT(findTimeLength()));
    disconnect(ui->findWidget->timeLengthUi->preButton, SIGNAL(clicked(bool)),
               this, SLOT(findTimeLength()));
    disconnect(ui->findWidget->timeLengthUi->procComboBox,
               SIGNAL(currentIndexChanged(int)), this,
               SLOT(clearFindResults()));
    disconnect(ui->findWidget->timeLengthUi->operatorComboBox,
               SIGNAL(currentIndexChanged(int)), this,
               SLOT(clearFindResults()));
    disconnect(ui->findWidget->timeLengthUi->timeSpinBox,
               SIGNAL(valueChanged(int)), this, SLOT(clearFindResults()));
  }

  FindWidget::FindMode m = ui->findWidget->findMode();
  ui->findWidget->switchFindMode(FindWidget::FindTimePointMode);
  ui->findWidget->setProcessorCount(_scene->processorCount());

  if (m != FindWidget::FindTimePointMode) {
    connect(ui->findWidget->timePointUi->nextButton, SIGNAL(clicked(bool)),
            this, SLOT(findTimeLength()));
    connect(ui->findWidget->timePointUi->preButton, SIGNAL(clicked(bool)), this,
            SLOT(findTimeLength()));
    connect(ui->findWidget->timePointUi->procComboBox,
            SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
    connect(ui->findWidget->timePointUi->typeComboBox,
            SIGNAL(currentIndexChanged(int)), this, SLOT(clearFindResults()));
    connect(ui->findWidget->timePointUi->timePointLineEdit,
            SIGNAL(textChanged(QString)), this, SLOT(clearFindResults()));

    clearFindResults();
  }
}

void TimeLineWidget::clearFindResults(void) {
  if (_currentIndex > -1) {
    TimeBlockItem *item = _findResults.at(_currentIndex);
    item->clearHighlight();
  }

  _findResults.clear();
  _resultIndex = 0;
  _currentIndex = -1;
  ui->findWidget->ui->countLabel->setText(QString());
}

void TimeLineWidget::refindTimeLength(void) {
  int index;
  FindWidget::FindMode m = ui->findWidget->findMode();
  bool isFindingTimeLength = (m == FindWidget::FindTimeLengthMode);
  if (isFindingTimeLength) {
    index = ui->findWidget->timeLengthUi->procComboBox->currentIndex();
  } else {
    index = ui->findWidget->timePointUi->procComboBox->currentIndex();
  }

  TimeLineRow *timeLine = _scene->_processorTable->rowAt(index);

  for (auto iter = timeLine->_itemsMap.begin();
       iter != timeLine->_itemsMap.end(); ++iter) {
    QList<TimeBlockItem *> *list = iter.value();
    for (TimeBlockItem *item : *list) {
      double tmp;
      if (isFindingTimeLength) {
        QRectF rect = item->rect();
        tmp = rect.width() / _scene->axisScale();

      } else {
        if (ui->findWidget->timePointUi->typeComboBox->currentIndex()) {
          tmp = item->endTimeStamp();
        } else {
          tmp = item->startTimeStamp();
        }
      }

      if (ui->findWidget->isFound(tmp)) {
        _findResults.append(item);
      }
    }
  }
}

void TimeLineWidget::findTimeLength() {
  if (_findResults.isEmpty()) {
    refindTimeLength();
    _resultIndex = 0;
    _currentIndex = -1;

    if (_findResults.isEmpty()) return;

    ui->findWidget->ui->countLabel->setText(tr("%1").arg(_findResults.count()));
  }

  if (_currentIndex > -1) {
    TimeBlockItem *item = _findResults.at(_currentIndex);
    item->clearHighlight();
  }

  QObject *obj = sender();
  TimeBlockItem *item;

  QPushButton *nextButton = nullptr;
  if (ui->findWidget->findMode() == FindWidget::FindTimeLengthMode) {
    nextButton = ui->findWidget->timeLengthUi->nextButton;
  } else {
    nextButton = ui->findWidget->timePointUi->nextButton;
  }

  if (obj == nextButton) {
    if (_resultIndex < _currentIndex) {
      _resultIndex = (_currentIndex + 1) % _findResults.count();
    }

    item = _findResults.at(_resultIndex);
    _currentIndex = _resultIndex;
    ++_resultIndex;
  } else {
    if (_currentIndex < _resultIndex) {
      _resultIndex =
          (_currentIndex - 1 + _findResults.count()) % _findResults.count();
    }

    item = _findResults.at(_resultIndex);
    _currentIndex = _resultIndex;
    --_resultIndex;
  }

  _resultIndex += _findResults.count();
  _resultIndex %= _findResults.count();

  item->highlight();
  qreal x = item->x() - 5;
  if (x < 0) x = 0.0f;
  ui->timeHScrollBar->setValue(x);
}

void TimeLineWidget::showTimeGrid(bool b) { _scene->_timeGrid->setVisible(b); }
