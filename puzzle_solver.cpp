#include "puzzle_solver.h"

puzzle_solver::puzzle_solver(QObject *parent) : QObject(parent){

}

ull puzzle_solver::cal_puzzle_hash(std::vector<int> puzzle){
    ull res = 0;
    for(int i = 0; i < 9; i++)
        res = res * hash_base + (ull)puzzle[i];
    return res;
}

void puzzle_solver::get_random_init_puzzle(){
    init_state.puzzle.clear();
    srand((int)time(0));
    bool vis[9];
    std::memset(vis, false, sizeof vis);
    int random_num;
    for(int i = 0; i < 9; i++) {
        do{
            random_num = rand() % 9;

        } while(vis[random_num]);

        vis[random_num] = true;
        init_state.puzzle.push_back(random_num);
    }
}

void puzzle_solver::get_init_state(){
    init_state.hash = cal_puzzle_hash(init_state.puzzle);
    init_state.depth = 0;
}

void puzzle_solver::get_goal_state(){
    goal_state.puzzle.clear();
    for(int i = 0; i < 9; i++)
        goal_state.puzzle.push_back(i);
    goal_state.hash = cal_puzzle_hash(goal_state.puzzle);
}

bool puzzle_solver::check_state_equal(state a, state b){
    return a.hash == b.hash;
}

bool puzzle_solver::check_equal_to_init_state(state now) {
    return now.hash == init_state.hash;
}

bool puzzle_solver::check_equal_to_goal_state(state now) {
    return now.hash == goal_state.hash;
}

void puzzle_solver::record_ans_step(state now) {
    reverse_ans.clear();
    reverse_ans.push_back(now);
    while(!check_equal_to_init_state(now)){
        now = pre_state[now.hash];
        reverse_ans.push_back(now);
    }
}

bool puzzle_solver::puzzle_solvable(state now) {
    int reverse_pair_cnt = 0;
    for(int i = 1; i < 9; i++)
        for(int j = 0; j < i; j++)
            if(now.puzzle[j] > now.puzzle[i] && now.puzzle[i] != 0 && now.puzzle[j] != 0)
                reverse_pair_cnt++;
    return ((reverse_pair_cnt % 2) == 0);
}

int puzzle_solver::cal_zero_pos(state now){
    for(int i = 0; i < 9; i++)
        if(now.puzzle[i] == 0)
            return i;
    return 0;
}

void puzzle_solver::breadth_first_search() {
    pre_state.clear();
    if(check_equal_to_goal_state(init_state)) {
        pre_state[init_state.hash] = init_state;
        record_ans_step(init_state);
        return;
    }

    std::queue<state> mq;
    mq.push(init_state);

    while(!mq.empty()) {
        state now = mq.front();
        mq.pop();

        if(check_equal_to_init_state(now)){
            if(pre_state.find(now.hash) != pre_state.end())
                continue;
            else
                pre_state[now.hash] = init_state;
        }

        int zero_pos = cal_zero_pos(now);
        int row = zero_pos / 3;
        int col = zero_pos % 3;
        bool flag = true;
        state new_state;

        if(row != 0 && flag) {
            new_state = update_queue(mq, now, zero_pos, -3);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }
        if(row != 2 && flag) {
            new_state = update_queue(mq, now, zero_pos, 3);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }
        if(col != 0 && flag) {
            new_state = update_queue(mq, now, zero_pos, -1);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }
        if(col != 2 && flag) {
            new_state = update_queue(mq, now, zero_pos, 1);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }

        if(!flag) {
            record_ans_step(new_state);
            return;
        }
    }
}


state puzzle_solver::update_queue(std::queue<state> & mq, state now, int zero_pos, int bias) {
    std::vector<int> tmp(now.puzzle);
    std::swap(tmp[zero_pos], tmp[zero_pos + bias]);
    ull hash = cal_puzzle_hash(tmp);
    if(pre_state.find(hash) == pre_state.end()) {
        mq.push({hash, tmp, 0});
        pre_state[hash] = now;
        return {hash, tmp, 0};
    }
    return now;
}

state puzzle_solver::limited_depth_first_search(state now, int depth, int max_dfs_depth) {

    if(check_equal_to_goal_state(now)) {
        reverse_ans.push_back(now);
        return now;
    }

    if(depth > max_dfs_depth) {
        dfs_state_vis[now.hash] = 0;
        return init_state;
    }

    else {
        int zero_pos = cal_zero_pos(now);
        int row = zero_pos / 3;
        int col = zero_pos % 3;
        state new_state, final_state;

        if(row != 0) {
            new_state = update_dfs_state(now, zero_pos, -3);
            if(!check_state_equal(now, new_state)) {

                final_state = limited_depth_first_search(new_state, depth + 1, max_dfs_depth);
                if(check_equal_to_goal_state(final_state)) {
                    reverse_ans.push_back(new_state);
                    return final_state;
                }
            }
        }

        if(row != 2) {
            new_state = update_dfs_state(now, zero_pos, 3);
            if(!check_state_equal(now, new_state)) {

                final_state = limited_depth_first_search(new_state, depth + 1, max_dfs_depth);
                if(check_equal_to_goal_state(final_state)) {
                    reverse_ans.push_back(new_state);
                    return final_state;
                }
            }
        }

        if(col != 0) {
            new_state = update_dfs_state(now, zero_pos, -1);
            if(!check_state_equal(now, new_state)) {

                final_state = limited_depth_first_search(new_state, depth + 1, max_dfs_depth);
                if(check_equal_to_goal_state(final_state)) {
                    reverse_ans.push_back(new_state);
                    return final_state;
                }
            }
        }

        if(col != 2) {
            new_state = update_dfs_state(now, zero_pos, 1);
            if(!check_state_equal(now, new_state)) {

                final_state = limited_depth_first_search(new_state, depth + 1, max_dfs_depth);
                if(check_equal_to_goal_state(final_state)) {
                    reverse_ans.push_back(new_state);
                    return final_state;
                }
            }
        }

        dfs_state_vis[now.hash] = 0;
        return init_state;
    }
}

state puzzle_solver::update_dfs_state(state now, int zero_pos, int bias){
    std::vector<int> tmp(now.puzzle);
    std::swap(tmp[zero_pos], tmp[zero_pos + bias]);
    ull hash = cal_puzzle_hash(tmp);
    if(!dfs_state_vis[hash]) {
        dfs_state_vis[hash] = true;
        return {hash, tmp, 0};
    }
    return now;
}

void puzzle_solver::iterative_deepening_depth_first_search() {
    for(int max_dfs_depth = 20; max_dfs_depth <= 200; max_dfs_depth += 10) {
        reverse_ans.clear();
        dfs_state_vis.clear();
        limited_depth_first_search(init_state, 0, max_dfs_depth);
        if(reverse_ans.size() > 0)
            return;
    }
}

state puzzle_solver::update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_best> & pq, state now, int zero_pos, int bias) {
    std::vector<int> tmp(now.puzzle);
    std::swap(tmp[zero_pos], tmp[zero_pos + bias]);
    ull hash = cal_puzzle_hash(tmp);
    if(pre_state.find(hash) == pre_state.end()) {
        pq.push({hash, tmp, now.depth + 1});
        pre_state[hash] = now;
        return {hash, tmp, now.depth + 1};
    }
    return now;
}

void puzzle_solver::update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_A_star> &pq, state now, int zero_pos, int bias) {
    std::vector<int> tmp(now.puzzle);
    std::swap(tmp[zero_pos], tmp[zero_pos + bias]);
    ull hash = cal_puzzle_hash(tmp);
    if(pre_state.find(hash) == pre_state.end()) {
        pq.push({hash, tmp, now.depth + 1});
        pre_state[hash] = now;
    }
    return;
}

void puzzle_solver::update_priority_queue(std::priority_queue<state, std::vector<state>, cmp_brunch_and_bound> &pq, state now, int zero_pos, int bias){
    std::vector<int> tmp(now.puzzle);
    std::swap(tmp[zero_pos], tmp[zero_pos + bias]);
    ull hash = cal_puzzle_hash(tmp);
    if(pre_state.find(hash) == pre_state.end()) {
        pq.push({hash, tmp, now.depth + 1});
        pre_state[hash] = now;
    }
    return;
}

void puzzle_solver::best_first_search() {
    pre_state.clear();
    if(check_equal_to_goal_state(init_state)) {
        pre_state[init_state.hash] = init_state;
        record_ans_step(init_state);
        return;
    }

    std::priority_queue<state, std::vector<state>, cmp_best> pq;
    pq.push(init_state);

    while(!pq.empty()) {
        state now = pq.top();
        pq.pop();

        if(check_equal_to_init_state(now)){
            if(pre_state.find(now.hash) != pre_state.end())
                continue;
            else
                pre_state[now.hash] = init_state;
        }

        int zero_pos = cal_zero_pos(now);
        int row = zero_pos / 3;
        int col = zero_pos % 3;
        state new_state;

        bool flag = true;

        if(row != 0 && flag) {
            new_state = update_priority_queue(pq, now, zero_pos, -3);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }

        if(row != 2 && flag) {
            new_state = update_priority_queue(pq, now, zero_pos, 3);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }

        if(col != 0 && flag) {
            new_state = update_priority_queue(pq, now, zero_pos, -1);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }

        if(col != 2 && flag) {
            new_state = update_priority_queue(pq, now, zero_pos, 1);
            if(check_equal_to_goal_state(new_state)) flag = false;
        }

        if(!flag) {
            record_ans_step(new_state);
            return;
        }
    }
}


void puzzle_solver::brunch_and_bound() {
    int min_goal_depth = 100000;
    state min_depth_goal_state;

    pre_state.clear();
    if(check_equal_to_goal_state(init_state)) {
        pre_state[init_state.hash] = init_state;
        record_ans_step(init_state);
        return;
    }

    std::priority_queue<state, std::vector<state>, cmp_brunch_and_bound> pq;
    pq.push(init_state);

    while(!pq.empty()) {
        state now = pq.top();
        pq.pop();

        if(check_equal_to_init_state(now)){
            if(pre_state.find(now.hash) != pre_state.end())
                continue;
            else
                pre_state[now.hash] = init_state;
        }

        if(now.depth + predict_left_steps(now) > min_goal_depth) {
            continue;
        }

        if(check_equal_to_goal_state(now)) {
            if(now.depth < min_goal_depth) {
                min_goal_depth = now.depth;
                min_depth_goal_state = now;
            }
            continue;
        }

        int zero_pos = cal_zero_pos(now);
        int row = zero_pos / 3;
        int col = zero_pos % 3;

        if(row != 0)
            update_priority_queue(pq, now, zero_pos, -3);

        if(row != 2)
            update_priority_queue(pq, now, zero_pos, 3);

        if(col != 0)
            update_priority_queue(pq, now, zero_pos, -1);

        if(col != 2)
            update_priority_queue(pq, now, zero_pos, 1);
    }

    record_ans_step(min_depth_goal_state);
}

int puzzle_solver::predict_left_steps(state now){
    int cnt = 0;
    for(int i = 0; i < 9; i++)
        cnt += (now.puzzle[i] != i);
    return cnt;
}

void puzzle_solver::A_star(){
    pre_state.clear();
    if(check_equal_to_goal_state(init_state)) {
        pre_state[init_state.hash] = init_state;
        record_ans_step(init_state);
        return;
    }

    std::priority_queue<state, std::vector<state>, cmp_A_star> pq;
    pq.push(init_state);

    while(!pq.empty()) {
        state now = pq.top();
        pq.pop();

        if(check_equal_to_init_state(now)){
            if(pre_state.find(now.hash) != pre_state.end())
                continue;
            else
                pre_state[now.hash] = init_state;
        }

        if(check_equal_to_goal_state(now)) {
            record_ans_step(now);
            return;
        }

        int zero_pos = cal_zero_pos(now);
        int row = zero_pos / 3;
        int col = zero_pos % 3;

        if(row != 0)
            update_priority_queue(pq, now, zero_pos, -3);

        if(row != 2)
            update_priority_queue(pq, now, zero_pos, 3);

        if(col != 0)
            update_priority_queue(pq, now, zero_pos, -1);

        if(col != 2)
            update_priority_queue(pq, now, zero_pos, 1);

    }
}



