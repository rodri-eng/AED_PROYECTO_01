"""Comprueba que la traza de la demostración respete el contrato de la cola en C++."""

import json
from pathlib import Path


def validate_trace(path):
    steps = json.loads(path.read_text(encoding="utf-8"))
    expected_ops = {
        "ERROR_POP", "PUSH", "START_TRANSFER", "TRANSFER_ITEM",
        "END_TRANSFER", "POP", "QUERY",
    }
    in_stack = []
    out_stack = []
    transfer_remaining = None
    removed = []

    for number, step in enumerate(steps, start=1):
        assert step["step"] == number, f"paso {number}: número de secuencia incorrecto"
        op = step["op"]
        assert op in expected_ops, f"paso {number}: operación desconocida {op}"
        assert isinstance(step["desc"], str)

        if op == "ERROR_POP":
            assert not in_stack and not out_stack
            assert step["value"] is None
        elif op == "PUSH":
            value = step["value"]
            assert isinstance(value, int)
            acc = min(in_stack[-1]["acc"], value) if in_stack else value
            in_stack.append({"val": value, "acc": acc})
        elif op == "START_TRANSFER":
            assert not out_stack and in_stack and transfer_remaining is None
            assert step["value"] is None
            transfer_remaining = len(in_stack)
        elif op == "TRANSFER_ITEM":
            assert transfer_remaining is not None and transfer_remaining > 0
            value = in_stack.pop()["val"]
            assert step["value"] == value
            acc = min(value, out_stack[-1]["acc"]) if out_stack else value
            out_stack.append({"val": value, "acc": acc})
            transfer_remaining -= 1
        elif op == "END_TRANSFER":
            assert transfer_remaining == 0 and not in_stack
            assert step["value"] is None
            transfer_remaining = None
        elif op == "POP":
            assert out_stack and transfer_remaining is None
            value = out_stack.pop()["val"]
            assert step["value"] == value
            removed.append(value)
        elif op == "QUERY":
            assert in_stack and out_stack and transfer_remaining is None
            assert step["value"] is None

        assert step["stack_in"] == in_stack, f"paso {number}: estado de IN incorrecto"
        assert step["stack_out"] == out_stack, f"paso {number}: estado de OUT incorrecto"
        values = [item["val"] for item in in_stack + out_stack]
        expected_min = min(values) if values else None
        assert step["total_agg"] == expected_min, f"paso {number}: acumulado incorrecto"

    assert transfer_remaining is None
    assert len(steps) == 18, "la demostración debe contener 18 pasos"
    assert [step["op"] for step in steps].count("TRANSFER_ITEM") == 4
    assert [step["op"] for step in steps].count("QUERY") == 1
    assert removed == [8, 3, 5, 2], "el orden de salida no respeta FIFO"
    assert not in_stack and not out_stack
    return len(steps)


if __name__ == "__main__":
    count = validate_trace(Path(__file__).with_name("trace.json"))
    print(f"Traza válida: {count} pasos, 4 transferencias, 1 consulta; orden FIFO conservado")
