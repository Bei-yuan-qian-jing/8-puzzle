#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <puzzle_solver.h>
#include <QTimer>
#include <QTime>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void show_next_state();



private slots:
    void on_radom_input_clicked();

    void on_run_clicked();

    void on_pushButton_clicked();

    void on_time_interval_editingFinished();

    void on_DFS_toggled(bool checked);

    void on_DFS_clicked();


private:
    Ui::MainWindow *ui;

    puzzle_solver * my_solver;
    QTimer * timer_next_state;
    QTime * algorithm_time;
};
#endif // MAINWINDOW_H
