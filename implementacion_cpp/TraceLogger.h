//
// Created by rodri on 22/09/2026.
//

#ifndef IMPLEMENTACION_CPP_TRACELOGGER_H
#define IMPLEMENTACION_CPP_TRACELOGGER_H

#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

struct StackElement {
    int val;
    int acc;
};

struct TraceStep {
    int step;
    std::string op;
    int value;
    bool has_value;
    std::vector<StackElement> stack_in;
    std::vector<StackElement> stack_out;
    int total_agg;
    bool has_agg;
    std::string desc;
};

class TraceLogger {
private:
    std::string filepath;
    std::vector<TraceStep> steps;

public:
    explicit TraceLogger(std::string path) : filepath(std::move(path)) {}

    void record(const std::string& op, int value, bool has_val,
                const std::vector<StackElement>& in_s,
                const std::vector<StackElement>& out_s,
                int total_agg, bool has_agg, const std::string& desc) {
        TraceStep s;
        s.step = static_cast<int>(steps.size()) + 1;
        s.op = op;
        s.value = value;
        s.has_value = has_val;
        s.stack_in = in_s;
        s.stack_out = out_s;
        s.total_agg = total_agg;
        s.has_agg = has_agg;
        s.desc = desc;
        steps.push_back(s);
    }

    void save() {
        std::ofstream out(filepath);
        if (!out.is_open()) {
            std::cerr << "Error: No se pudo abrir el archivo " << filepath << std::endl;
            return;
        }

        out << "[\n";
        for (size_t i = 0; i < steps.size(); ++i) {
            const auto& s = steps[i];
            out << "  {\n";
            out << "    \"step\": " << s.step << ",\n";
            out << "    \"op\": \"" << s.op << "\",\n";
            out << "    \"value\": " << (s.has_value ? std::to_string(s.value) : "null") << ",\n";

            out << "    \"stack_in\": [";
            for (size_t j = 0; j < s.stack_in.size(); ++j) {
                out << "{\"val\": " << s.stack_in[j].val << ", \"acc\": " << s.stack_in[j].acc << "}";
                if (j + 1 < s.stack_in.size()) out << ", ";
            }
            out << "],\n";

            out << "    \"stack_out\": [";
            for (size_t j = 0; j < s.stack_out.size(); ++j) {
                out << "{\"val\": " << s.stack_out[j].val << ", \"acc\": " << s.stack_out[j].acc << "}";
                if (j + 1 < s.stack_out.size()) out << ", ";
            }
            out << "],\n";

            out << "    \"total_agg\": " << (s.has_agg ? std::to_string(s.total_agg) : "null") << ",\n";
            out << "    \"desc\": \"" << s.desc << "\"\n";
            out << "  }" << (i + 1 < steps.size() ? ",\n" : "\n");
        }
        out << "]\n";
        out.close();
        std::cout << "Archivo de traza guardado en: " << filepath << std::endl;
    }
};

#endif //IMPLEMENTACION_CPP_TRACELOGGER_H