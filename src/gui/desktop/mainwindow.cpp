#include "mainwindow.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), selectedGame(Snake), gameStarted(false) {
  snakeModel = new SnakeModel();
  controller = new SnakeController(snakeModel);
  setupUI();
  renderGameSelection();
  gameTimer = new QTimer(this);
  connect(gameTimer, &QTimer::timeout, this, &MainWindow::updateGame);
  gameTimer->start(10);
  restoreFocus();
}

MainWindow::~MainWindow() {
  delete controller;
  delete snakeModel;
}

void MainWindow::setupUI() {
  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

  gameView = new QGraphicsView(this);
  gameScene = new QGraphicsScene(0, 0, FIELD_WIDTH, FIELD_HEIGHT, this);
  nextBlockView = new QGraphicsView(this);
  nextBlockScene =
      new QGraphicsScene(0, 0, NEXT_BLOCK_SIZE, NEXT_BLOCK_SIZE, this);

  scoreLabel = new QLabel("Очки: 0", this);
  highScoreLabel = new QLabel("Рекорд: 0", this);
  levelLabel = new QLabel("Уровень: 1", this);
  speedLabel = new QLabel("Скорость: 300", this);
  infoLabel = new QLabel(this);
  infoLabel->setText(
      "CONTROLS:\n"
      "Start:    Enter\n"
      "Pause:    P\n"
      "Action:   Space\n"
      "Left:     ←\n"
      "Right:    →\n"
      "Down:     ↓\n"
      "Exit:     Esc");
  gameView->setScene(gameScene);
  gameView->setFixedSize(FIELD_WIDTH + 2, FIELD_HEIGHT + 2);
  gameView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  gameView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  nextBlockView->setScene(nextBlockScene);
  nextBlockView->setFixedSize(NEXT_BLOCK_SIZE + 2, NEXT_BLOCK_SIZE + 2);
  nextBlockView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  nextBlockView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
  QVBoxLayout *infoLayout = new QVBoxLayout();
  infoLayout->addWidget(scoreLabel);
  infoLayout->addWidget(highScoreLabel);
  infoLayout->addWidget(levelLabel);
  infoLayout->addWidget(speedLabel);
  infoLayout->addWidget(nextBlockView);
  infoLayout->addWidget(infoLabel);

  QHBoxLayout *gameLayout = new QHBoxLayout();
  gameLayout->addWidget(gameView);
  gameLayout->addLayout(infoLayout);

  mainLayout->addLayout(gameLayout);
  setFixedSize(centralWidget->sizeHint());
}
void MainWindow::updateGame() {
  State game_state = controller->getGameState();
  if (game_state == SelectGame) {
    renderGameSelection();
    return;
  }

  GameInfo_t state = controller->getCurrentState();
  if (!state.field || !state.next) {
    return;
  }

  if (game_state == EndGame || game_state == WinGame) {
    renderGameOver();
  } else {
    renderGame();
    updateGameInfo(state);
    if (!state.pause && controller->getSelectedGame() == Tetris) {
      unsigned long long time_left = controller->getTimeUntilStep();
      if (time_left == 0) {
        controller->userInput(Down, false);
      }
    }
    if (!state.pause && controller->getSelectedGame() == Snake) {
      controller->updateGame();
    }
  }

  if (state.field) {
    for (int i = 0; i < HEIGHT; i++) {
      if (state.field[i]) {
        delete[] state.field[i];
      }
    }
    delete[] state.field;
    state.field = nullptr;
  }
  if (state.next) {
    for (int i = 0; i < 4; i++) {
      if (state.next[i]) {
        delete[] state.next[i];
      }
    }
    delete[] state.next;
    state.next = nullptr;
  }
}

void MainWindow::renderGame() {
  gameScene->clear();
  GameInfo_t state = controller->getCurrentState();
  if (!state.field || !state.next) {
    return;
  }
  updateGameInfo(state);

  for (int i = 0; i < HEIGHT; ++i) {
    for (int j = 0; j < WIDTH; ++j) {
      int value = state.field[i][j];
      if (value == 1 ||
          (controller->getSelectedGame() == Snake && value == 2)) {
        QBrush brush = (value == 1) ? QBrush(Qt::green) : QBrush(Qt::red);
        QPen pen(Qt::black);
        gameScene->addRect(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE,
                           BLOCK_SIZE, pen, brush);
      }
    }
  }
  if (state.pause == 2) {
    QFont font("Arial", 20, QFont::Bold);
    QGraphicsTextItem *pauseText = gameScene->addText("Pause", font);
    pauseText->setPos((FIELD_WIDTH - pauseText->boundingRect().width()) / 2,
                      (FIELD_HEIGHT - pauseText->boundingRect().height()) / 2);
  }
  if (controller->getSelectedGame() == Tetris) {
    nextBlockScene->clear();
    for (int i = 0; i < 4; ++i) {
      for (int j = 0; j < 4; ++j) {
        if (state.next[i][j] == 1) {
          QPen pen(Qt::black);
          QBrush brush(Qt::green);
          nextBlockScene->addRect(j * BLOCK_SIZE, i * BLOCK_SIZE, BLOCK_SIZE,
                                  BLOCK_SIZE, pen, brush);
        }
      }
    }
  } else {
    nextBlockScene->clear();
  }
}

void MainWindow::renderGameSelection() {
  gameScene->clear();
  nextBlockScene->clear();
  QFont font("Arial", 14);
  gameScene->addText(
      "Choose a game: " + QString((controller->getSelectedGame() == Tetris)
                                      ? "TETRIS"
                                      : "SNAKE"),
      font);
  QGraphicsTextItem *text =
      gameScene->addText("Press Enter to start\n←/→ to change the game", font);
  text->setPos(0, 30);
}

void MainWindow::renderGameOver() {
  gameScene->clear();
  nextBlockScene->clear();
  QFont font("Arial", 14);
  gameScene->addText("Game over!", font);
  QGraphicsTextItem *scoreText = gameScene->addText(
      "Score: " + QString::number(controller->getCurrentState().score), font);
  scoreText->setPos(0, 30);
  QGraphicsTextItem *highScoreText = gameScene->addText(
      "Record: " + QString::number(controller->getCurrentState().high_score),
      font);
  highScoreText->setPos(0, 60);
  QGraphicsTextItem *restartText =
      gameScene->addText("Press Enter to restart\nEsc to exit", font);
  restartText->setPos(0, 90);
}

void MainWindow::updateGameInfo(const GameInfo_t &info) {
  scoreLabel->setText("Score: " + QString::number(info.score));
  highScoreLabel->setText("Record: " + QString::number(info.high_score));
  levelLabel->setText("Level: " + QString::number(info.level));
  speedLabel->setText("Speed: " + QString::number(info.speed));
}

void MainWindow::quitApplication() { QApplication::quit(); }

void MainWindow::restoreFocus() {
  setFocus();
  activateWindow();
  raise();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
  handleUserInput(event->key());
  event->accept();
}

void MainWindow::handleUserInput(int key) {
  UserAction_t action = convertKeyToAction(key);

  if (action == Terminate) {
    controller->userInput(action, false);
    quitApplication();
    return;
  }
  if (controller->getGameState() == SelectGame) {
    if (action == Left || action == Right) {
      controller->switchGame();
      renderGameSelection();
      restoreFocus();
      return;
    }
    if (action == Start) {
      gameStarted = true;
      controller->userInput(action, false);
      renderGame();
      restoreFocus();
      return;
    }
  }
  controller->userInput(action, key == Qt::Key_Space);
}

UserAction_t MainWindow::convertKeyToAction(int key) const {
  switch (key) {
    case Qt::Key_Up:
      return Up;
    case Qt::Key_Down:
      return Down;
    case Qt::Key_Left:
      return Left;
    case Qt::Key_Right:
      return Right;
    case Qt::Key_Space:
      return Action;
    case Qt::Key_Return:
      return Start;
    case Qt::Key_P:
      return Pause;
    case Qt::Key_Escape:
      return Terminate;
    default:
      return Start;
  }
}