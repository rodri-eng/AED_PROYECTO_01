#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <type_traits>
#include "../AssociativeQueue.h"

static_assert(!std::is_copy_constructible_v<AssociativeStack<int>>);
static_assert(!std::is_copy_assignable_v<AssociativeStack<int>>);
static_assert(!std::is_copy_constructible_v<AssociativeQueue>);

void require(bool condition) {
    if (!condition) {
        throw std::runtime_error("Se incumplió el contrato de AssociativeQueue");
    }
}

int main() {
    TraceLogger logger("unused.json");
    AssociativeQueue queue([](int a, int b) { return std::min(a, b); }, "min", logger);

    bool empty_query_failed = false;
    try {
        queue.query_with_trace();
    } catch (const std::underflow_error&) {
        empty_query_failed = true;
    }
    require(empty_query_failed);

    queue.push(8);
    queue.push(3);
    queue.push(5);
    require(queue.size() == 3 && queue.query() == 3);
    require(queue.pop() == 8);
    queue.push(2);
    require(queue.query_with_trace() == 2 && queue.size() == 3);
    require(queue.pop() == 3);
    require(queue.pop() == 5);
    require(queue.pop() == 2 && queue.empty());

    // Las operaciones de proyección son asociativas, pero no conmutativas;
    // permiten verificar que el agregado respete el orden de la cola.
    AssociativeQueue first([](int a, int) { return a; }, "first", logger);
    first.push(8);
    first.push(3);
    first.push(5);
    require(first.query() == 8);
    require(first.pop() == 8);
    first.push(2);
    require(first.query() == 3);

    AssociativeQueue last([](int, int b) { return b; }, "last", logger);
    last.push(8);
    last.push(3);
    last.push(5);
    require(last.query() == 5);
    require(last.pop() == 8);
    last.push(2);
    require(last.query() == 2);

    TraceLogger escaped_logger("trace_logger_test.json");
    escaped_logger.record("TEST", 0, false, {}, {}, 0, false, "comilla \" y salto de linea\n");
    escaped_logger.save();
    std::ifstream saved("trace_logger_test.json");
    std::string json((std::istreambuf_iterator<char>(saved)), std::istreambuf_iterator<char>());
    require(json.find("comilla \\\" y salto de linea\\n") != std::string::npos);
}
