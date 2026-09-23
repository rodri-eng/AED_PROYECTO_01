//
// Created by rodri on 22/09/2026.
//

#ifndef IMPLEMENTACION_CPP_ASSOCIATIVEQUEUE_H
#define IMPLEMENTACION_CPP_ASSOCIATIVEQUEUE_H

#pragma once

#include <functional>
#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include "TraceLogger.h"

template<typename data_type>
struct AssociativeStack {
    struct StackNode {
        data_type val;
        data_type acc;
        StackNode* next;

        StackNode(data_type v, data_type a, StackNode* n = nullptr)
            : val(v), acc(a), next(n) {}
    };

    StackNode* _top;
    int _size;

    AssociativeStack() : _top(nullptr), _size(0) {}

    ~AssociativeStack() {
        while (!empty()) {
            pop();
        }
    }

    bool empty() const {
        return _top == nullptr;
    }

    int size() const {
        return _size;
    }

    void push(data_type val, data_type acc) {
        StackNode* new_node = new StackNode(val, acc, _top);
        _top = new_node;
        _size++;
    }

    void pop() {
        if (_top != nullptr) {
            StackNode* temp = _top;
            _top = _top->next;
            delete temp;
            _size--;
        }
    }

    StackNode* top() const {
        return _top;
    }

    std::vector<StackElement> to_vector() const {
        std::vector<StackElement> elems;
        for (StackNode* curr = _top; curr != nullptr; curr = curr->next) {
            elems.push_back({curr->val, curr->acc});
        }
        std::reverse(elems.begin(), elems.end());
        return elems;
    }
};

class AssociativeQueue {
private:
    AssociativeStack<int> _stack_in;
    AssociativeStack<int> _stack_out;
    std::function<int(int, int)> op;
    std::string op_symbol;
    TraceLogger& logger;

    int combine(int a, int b) const {
        return op(a, b);
    }

public:
    AssociativeQueue(std::function<int(int, int)> operation, std::string symbol, TraceLogger& log)
        : op(std::move(operation)), op_symbol(std::move(symbol)), logger(log) {}

    bool empty() const {
        return _stack_in.empty() && _stack_out.empty();
    }

    size_t size() const {
        return _stack_in.size() + _stack_out.size();
    }

    void push(int x) {
        int new_acc = _stack_in.empty() ? x : combine(_stack_in.top()->acc, x);
        _stack_in.push(x, new_acc);

        logger.record("PUSH", x, true,
                      _stack_in.to_vector(), _stack_out.to_vector(),
                      query(), true,
                      "Push(" + std::to_string(x) + "): ingresa a _stack_in con acumulado " + std::to_string(new_acc));
    }

    int pop() {
        if (empty()) {
            logger.record("ERROR_POP", 0, false,
                          _stack_in.to_vector(), _stack_out.to_vector(),
                          0, false,
                          "Caso borde: intento de pop() en cola vacia");
            throw std::underflow_error("Cola vacia");
        }

        if (_stack_out.empty()) {
            logger.record("START_TRANSFER", 0, false,
                          _stack_in.to_vector(), _stack_out.to_vector(),
                          query(), true,
                          "_stack_out vacia: traspasando elementos invirtiendo acumulados");

            while (!_stack_in.empty()) {
                int val = _stack_in.top()->val;
                _stack_in.pop();

                int new_acc = _stack_out.empty() ? val : combine(val, _stack_out.top()->acc);
                _stack_out.push(val, new_acc);
            }

            logger.record("END_TRANSFER", 0, false,
                          _stack_in.to_vector(), _stack_out.to_vector(),
                          query(), true,
                          "Traspaso finalizado. Elementos listos para salir por _stack_out");
        }

        int removed = _stack_out.top()->val;
        _stack_out.pop();

        bool has_agg = !empty();
        int agg = has_agg ? query() : 0;

        logger.record("POP", removed, true,
                      _stack_in.to_vector(), _stack_out.to_vector(),
                      agg, has_agg,
                      "Pop(): sale " + std::to_string(removed) + " del frente de la cola");

        return removed;
    }

    int query() const {
        if (empty()) {
            throw std::underflow_error("Cola vacia, no hay acumulado");
        }
        if (_stack_in.empty()) {
            return _stack_out.top()->acc;
        }
        if (_stack_out.empty()) {
            return _stack_in.top()->acc;
        }
        return combine(_stack_out.top()->acc, _stack_in.top()->acc);
    }
};

#endif //IMPLEMENTACION_CPP_ASSOCIATIVEQUEUE_H