#include <iostream>
#include <atomic>

class lf_node;

struct lf_node_wrap {
    uint64_t counter;
    lf_node* node;
};

struct lf_node {
    int val;
    lf_node_wrap next;
    lf_node(int i) : val(i), next({}) {}
};

class LFStack {
public:

    void push(int i) {
        auto old_wrap = this->node.load();
        lf_node_wrap new_wrap = {.counter = 0, .node = new lf_node(i)};
        do {
            new_wrap.node->next = old_wrap;
            new_wrap.counter = old_wrap.counter + 1;
        }while (!this->node.compare_exchange_strong(old_wrap, new_wrap));
    }

    lf_node* pop() {
        auto old_wrap = this->node.load();
        lf_node_wrap new_wrap;
        do {
            auto old = old_wrap.node;
            if (old == nullptr) return nullptr;
            new_wrap = old->next;
            new_wrap.counter = old_wrap.counter + 1;
        }while (!this->node.compare_exchange_strong(old_wrap, new_wrap));
        return old_wrap.node;
    }

    std::atomic<lf_node_wrap> node;
};

#include <thread>

LFStack stack;
int count1 = 0;
int count2 = 0;
int count3 = 0;

int main() {
    std::thread t1([]() {
        for (int i = 0; i < 5000000; ++i) {
            stack.push(i);
        }
    });
    std::thread t4([]() {
        for (int i = 0; i < 20000000; ++i) {
            auto p = stack.pop();
            if (p) {
                count1++;
            }
        }
    });
    std::thread t6([]() {
        for (int i = 0; i < 20000000; ++i) {
            auto p = stack.pop();
            if (p) {
                count2++;
            }
        }
    });
    std::thread t7([]() {
        for (int i = 0; i < 20000000; ++i) {
            auto p = stack.pop();
            if (p) {
                count3++;
            }
        }
    });
    std::thread t2([]() {
        for (int i = 0; i < 5000000; ++i) {
            stack.push(i);
        }
    });
    std::thread t3([]() {
        for (int i = 0; i < 5000000; ++i) {
            stack.push(i);
        }
    });
    std::thread t5([]() {
        for (int i = 0; i < 5000000; ++i) {
            stack.push(i);
        }
    });
    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    std::cout << count1 + count2 + count3 << std::endl;
    return 0;
}
