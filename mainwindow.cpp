#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <windows.h>

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent), ui(new Ui::MainWindow) {

    //设置窗口
    this->setWindowTitle("8-puzzle");
    setFixedSize(600, 650);
    ui->setupUi(this);

    //启动游戏求解器
    my_solver = new puzzle_solver();

    //计时器设置
    timer_next_state = new QTimer(this);
    algorithm_time = new QTime();


    // 信号timeout的槽函数，倒计时至0时，展示step by step的下一个状态
    connect(timer_next_state, &QTimer::timeout, [=]() {
        srand(time(NULL));
        this->show_next_state();
    });
}

MainWindow::~MainWindow(){
    delete ui;
}


void MainWindow::on_radom_input_clicked(){
    //随机初始化数字位置
    my_solver->get_random_init_puzzle();
    //将数字显示在界面上
    ui->t0->setText(QString::number(my_solver->init_state.puzzle[0]));
    ui->t1->setText(QString::number(my_solver->init_state.puzzle[1]));
    ui->t2->setText(QString::number(my_solver->init_state.puzzle[2]));
    ui->t3->setText(QString::number(my_solver->init_state.puzzle[3]));
    ui->t4->setText(QString::number(my_solver->init_state.puzzle[4]));
    ui->t5->setText(QString::number(my_solver->init_state.puzzle[5]));
    ui->t6->setText(QString::number(my_solver->init_state.puzzle[6]));
    ui->t7->setText(QString::number(my_solver->init_state.puzzle[7]));
    ui->t8->setText(QString::number(my_solver->init_state.puzzle[8]));
}

void MainWindow::on_run_clicked(){
    //清除上一轮的显示
    ui->goal_node_val->setText(" ");
    ui->goal_node_val->repaint();

    ui->steps_val->setText(" ");
    ui->steps_val->repaint();

    ui->time_val->setText(" ");
    ui->time_val->repaint();

    ui->step->setText(" ");
    ui->step->repaint();

    ui->step->setText(" ");
    ui->step->repaint();

    my_solver->step = 0;


    //从屏幕输入获取初始状态
    my_solver->init_state.puzzle.clear();
    my_solver->init_state.puzzle.push_back(ui->t0->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t1->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t2->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t3->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t4->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t5->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t6->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t7->text().toUInt());
    my_solver->init_state.puzzle.push_back(ui->t8->text().toUInt());
    my_solver->get_init_state();

    //获取最终状态
    my_solver->get_goal_state();

    //可解
    if(my_solver->puzzle_solvable(my_solver->init_state)){
        ui->goal_node_val->setText("Solvable");

        algorithm_time->start();

        //以下为不同解法被选择时的调用
        if(ui->BFS->isChecked())
            my_solver->breadth_first_search();

        if(ui->DFS->isChecked()) {
            my_solver->iterative_deepening_depth_first_search();
        }

        if(ui->Best->isChecked())
            my_solver->best_first_search();

        if(ui->Branch->isChecked())
            my_solver->brunch_and_bound();

        if(ui->A_star->isChecked())
            my_solver->A_star();

        ui->time_val->setNum(algorithm_time->elapsed() / 1000.0);
        ui->steps_val->setNum((int)my_solver->reverse_ans.size() - 1);
    }
    //不可解
    else {
        ui->goal_node_val->setText("not solvable");
    }
}


//设置step by step演示时的时间间隔（演示速度）
void MainWindow::on_time_interval_editingFinished(){
    timer_next_state->setInterval(ui->time_interval->value());
}


//启动定时器，当定时器倒计时至0时，发送信号timeout，对应的槽函数写在MainWindow类的初始化中
void MainWindow::on_pushButton_clicked(){
    timer_next_state->setInterval(ui->time_interval->value());
    timer_next_state->start();
}

//step by step展示中的某一个状态
void MainWindow::show_next_state(){
    int i = my_solver->reverse_ans.size() - 1 - my_solver->step;
    if(i >= 0) {
        ui->step->setText(QString::number(my_solver->step));

        std::vector<int> print_puzzle = my_solver->reverse_ans[i].puzzle;

        ui->t0->setText(print_puzzle[0] != 0 ? QString::number(print_puzzle[0]) : " ");
        ui->t1->setText(print_puzzle[1] != 0 ? QString::number(print_puzzle[1]) : " ");
        ui->t2->setText(print_puzzle[2] != 0 ? QString::number(print_puzzle[2]) : " ");
        ui->t3->setText(print_puzzle[3] != 0 ? QString::number(print_puzzle[3]) : " ");
        ui->t4->setText(print_puzzle[4] != 0 ? QString::number(print_puzzle[4]) : " ");
        ui->t5->setText(print_puzzle[5] != 0 ? QString::number(print_puzzle[5]) : " ");
        ui->t6->setText(print_puzzle[6] != 0 ? QString::number(print_puzzle[6]) : " ");
        ui->t7->setText(print_puzzle[7] != 0 ? QString::number(print_puzzle[7]) : " ");
        ui->t8->setText(print_puzzle[8] != 0 ? QString::number(print_puzzle[8]) : " ");
        my_solver->step++;
    }

    else{
        timer_next_state->stop();
    }
}

//当上一轮选择DFS解法，但本轮未选择DFS解法时，clear上一轮的警告
void MainWindow::on_DFS_toggled(bool checked){
    if(!checked) {
        ui->caution_ifo->setText("");
        ui->caution_ifo->repaint();
    }
}

//用户选择DFS解法时，展示警告
void MainWindow::on_DFS_clicked(){
    ui->caution_ifo->setText("Caution: DFS may takes more than 20s. Please wait patiently.");

}

