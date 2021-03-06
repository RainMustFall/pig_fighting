#include "./field_view.h"

#include <QPainter>
#include <QMouseEvent>
#include <QDebug>
#include <QSound>

#include <cstdlib>

#include "./main_window.h"
#include "source_code/utils/constants.h"

FieldView::FieldView(QWidget *parent)
    : QMainWindow(parent),
      controller_(new FieldController(this, kDefaultMap)),
      parent_(dynamic_cast<MainWindow *>(parent)),
      cur_theme_(utils::TextureType::GRASS) {
  DrawBackground();
  setFocus();
}

void FieldView::SetTimer() {
  is_start_ = true;
  timer_id_ = startTimer(kTimerInterval);
}

void FieldView::StopTimer() { killTimer(timer_id_); }

void FieldView::GameOver(int player) { parent_->GameOver(player); }

void FieldView::NewGame(utils::TextureType type, const QString &map) {
  cur_theme_ = type;
  DrawBackground();
  setFocus();
  delete controller_;
  controller_ = new FieldController(this, kMapsPath + map + ".json", type);
  SetTimer();
}

void FieldView::timerEvent(QTimerEvent *) {
  controller_->UpdateTimer();
  controller_->UpdatePlayers();
  controller_->AddPigs();
  controller_->UpdateFreePigs();
  controller_->UpdateFlyingPigs();
  repaint();
}

void FieldView::paintEvent(QPaintEvent *) {
  QPainter p;
  p.begin(this);
  controller_->OnPaintingStarted(&p);
  p.end();
}

void FieldView::DrawHint(QPainter *painter) {
  QRectF rectangle = {kHintIndentX, kHintIndentY, kHintWidth, kHintHeight};
  QImage image;
  image.load(kTexturesPath + "instruction1.png");
  painter->drawImage(rectangle, image);

  rectangle = {kScreenWidth - kHintIndentX - kHintWidth, kHintIndentY,
               kHintWidth, kHintHeight};
  image.load(kTexturesPath + "instruction2.png");
  painter->drawImage(rectangle, image);
}

void FieldView::DrawBackground() {
  QString bg_dir = background_dirs_[static_cast<unsigned>(cur_theme_)];
  QPixmap bkgnd(kTexturesPath + bg_dir + "/background.png");
  bkgnd = bkgnd.scaled(kScreenWidth, kScreenHeight, Qt::IgnoreAspectRatio,
                       Qt::SmoothTransformation);
  QPalette p(palette());
  p.setBrush(QPalette::Background, bkgnd);
  setAutoFillBackground(true);
  setPalette(p);
}

void FieldView::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Escape) {
    parent_->Pause(false);
  } else {
    controller_->OnKeyPressed(event);
  }
}

void FieldView::keyReleaseEvent(QKeyEvent *event) {
  controller_->OnKeyReleased(event);
}

FieldView::~FieldView() { delete controller_; }
