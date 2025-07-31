#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../../brick_game/brick_game.h"
#include "../../brick_game/snake/snakeController.h"
#include "../../brick_game/snake/snakeModel.h"
#include "../../brick_game/tetris/backend.h"

#ifdef border
#undef border
#endif
#ifdef scroll
#undef scroll
#endif
#ifdef timeout
#undef timeout
#endif

#include <QApplication>
#include <QGraphicsScene>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QVBoxLayout>

using namespace s21;
class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget *parent = nullptr);
  ~MainWindow();

 public slots:
  void quitApplication();
  void restoreFocus();

 private slots:
  void updateGame();

 private:
  void setupUI();
  void renderGame();
  void renderGameSelection();
  void renderGameOver();
  void handleUserInput(int key);
  UserAction_t convertKeyToAction(int key) const;
  void updateGameInfo(const GameInfo_t &info);

  void keyPressEvent(QKeyEvent *event) override;
  QLabel *infoLabel;
  QGraphicsView *gameView;
  QGraphicsScene *gameScene;
  QLabel *scoreLabel;
  QLabel *highScoreLabel;
  QLabel *levelLabel;
  QLabel *speedLabel;
  QGraphicsView *nextBlockView;
  QGraphicsScene *nextBlockScene;
  QTimer *gameTimer;
  SnakeController *controller;
  SnakeModel *snakeModel;
  Game selectedGame;
  bool gameStarted;

  static constexpr int BLOCK_SIZE = 30;
  static constexpr int FIELD_WIDTH = WIDTH * BLOCK_SIZE;
  static constexpr int FIELD_HEIGHT = HEIGHT * BLOCK_SIZE;
  static constexpr int NEXT_BLOCK_SIZE = 4 * BLOCK_SIZE;
};

#endif  // MAINWINDOW_H