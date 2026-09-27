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
#include <stdexcept>
#include <utility>

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

    static std::string escapeJson(const std::string& input) {
        static const char hex[] = "0123456789abcdef";
        std::string escaped;
        for (unsigned char character : input) {
            switch (character) {
                case '"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\b': escaped += "\\b"; break;
                case '\f': escaped += "\\f"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default:
                    if (character < 0x20) {
                        escaped += "\\u00";
                        escaped += hex[character >> 4];
                        escaped += hex[character & 0x0f];
                    } else {
                        escaped += static_cast<char>(character);
                    }
            }
        }
        return escaped;
    }

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
            throw std::runtime_error("No se pudo abrir el archivo de traza: " + filepath);
        }

        out << "[\n";
        for (size_t i = 0; i < steps.size(); ++i) {
            const auto& s = steps[i];
            out << "  {\n";
            out << "    \"step\": " << s.step << ",\n";
            out << "    \"op\": \"" << escapeJson(s.op) << "\",\n";
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
            out << "    \"desc\": \"" << escapeJson(s.desc) << "\"\n";
            out << "  }" << (i + 1 < steps.size() ? ",\n" : "\n");
        }
        out << "]\n";
        out.close();
        if (!out) {
            throw std::runtime_error("No se pudo escribir el archivo de traza: " + filepath);
        }
        std::cout << "Archivo de traza guardado en: " << filepath << std::endl;
    }
};

#endif //IMPLEMENTACION_CPP_TRACELOGGER_H
