#ifndef PUZZLE_SOLVER_H
#define PUZZLE_SOLVER_H

#include <QObject>
#include <QWidget>
#include<bits/stdc++.h>
typedef unsigned long long ull;
typedef long long ll;

struct state {
    ull hash;
    std::vector<int> puzzle;
    int depth;
};

struct cmp_best {
    bool operator () (state a, state b)  {
        int cnt_dis_a = 0, cnt_dis_b = 0;

        for(int i = 0; i < 9; i++) {
            int row_a = a.puzzle[i] / 3;
            int col_a = a.puzzle[i] % 3;
            cnt_dis_a += abs(row_a - i / 3) + abs(col_a - i % 3);

            int row_b = b.puzzle[i] / 3;
            int col_b = b.puzzle[i] % 3;
            cnt_dis_b += abs(row_b - i / 3) + abs(col_b - i % 3);
        }

        return cnt_dis_a > cnt_dis_b;
    }
};

struct cmp_brunch_and_bound {
    bool operator () (state a, state b)  {
        int cnt_dis_a = 0, cnt_dis_b = 0;

        for(int i = 0; i < 9; i++) {
            cnt_dis_a += std::abs(a.puzzle[i] - i);
            cnt_dis_b += std::abs(b.puzzle[i] - i);
        }

        return cnt_dis_a + a.depth > cnt_dis_b + b.depth;
    }
};

struct cmp_A_star {
    bool operator () (state a, state b)  {
        int cnt_dis_a = 0, cnt_dis_b = 0;

        for(int i = 0; i < 9; i++) {
            cnt_dis_a += (a.puzzle[i] != i);
            cnt_dis_b += (b.puzzle[i] != i);
        }

        return cnt_dis_a + a.depth > cnt_dis_b + b.depth;
    }

};


class puzzle_solver : public QObject
{
    Q_OBJECT
public:
    explicit puzzle_solver(QObject *parent = nullptr);
    state init_state, goal_state;
    ull hash_base = 291143;
    int step = 0;
    std::vector<state> reverse_ans;
    std::map<ull, state> pre_state;
    std::map<ull, bool> dfs_state_vis;

    //----------function--------------
    ull cal_puzzle_hash(std::vector<int>);
    void get_random_init_puzzle();
    void get_init_state();
    void get_goal_state();
    bool check_equal_to_init_state(state now);
    bool check_equal_to_goal_state(state now);
    void record_ans_step(state final_state);
    bool puzzle_solvable(state now);
    int cal_zero_pos(state now);
    bool check_state_equal(state a, state b);

    void breadth_first_search();
    state update_queue(std::queue<state> & mq, state now, int zero_pos, int bias);

    state limited_depth_first_search(state now, int dfs_depth, int max_dfs_depth);
    state update_dfs_state(state now, int zero_pos, int bias);
    void iterative_deepening_depth_first_search();

    void best_first_search();
    state update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_best> & pq, state now, int zero_pos, int bias);
    void update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_A_star> & pq, state now, int zero_pos, int bias);
    void update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_brunch_and_bound> & pq, state now, int zero_pos, int bias);

    void brunch_and_bound();
    int predict_left_steps(state now);

    void A_star();

signals:

};

#endif // PUZZLE_SOLVER_H
