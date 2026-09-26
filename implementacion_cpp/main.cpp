#include <iostream>
#include <algorithm>
#include "TraceLogger.h"
#include "AssociativeQueue.h"

int main(int argc, char *argv[])
{
    std::string outputPath = "../animacion_manim/trace.json";
    if (argc > 1)
    {
        outputPath = argv[1];
    }

    TraceLogger logger(outputPath);

    auto min_op = [](int a, int b)
    { return std::min(a, b); };

    AssociativeQueue q(min_op, "min", logger);

    std::cout << "--- Generando traza de ejecucion para Manim ---" << std::endl;

    try
    {
        q.pop();
    }
    catch (...)
    {
    }

    q.push(8);
    q.push(3);
    q.push(5);

    q.pop();

    q.push(2);

    q.query_with_trace();

    q.pop();
    q.pop();
    q.pop();

    logger.save();
    std::cout << "Proceso completado exitosamente." << std::endl;

    return 0;
}