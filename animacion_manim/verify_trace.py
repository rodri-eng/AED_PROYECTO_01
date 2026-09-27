"""Check that the rendered demo's trace follows the C++ queue contract."""

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
        assert step["step"] == number, f"step {number}: wrong sequence number"
        op = step["op"]
        assert op in expected_ops, f"step {number}: unknown op {op}"
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

        assert step["stack_in"] == in_stack, f"step {number}: IN snapshot mismatch"
        assert step["stack_out"] == out_stack, f"step {number}: OUT snapshot mismatch"
        values = [item["val"] for item in in_stack + out_stack]
        expected_min = min(values) if values else None
        assert step["total_agg"] == expected_min, f"step {number}: aggregate mismatch"

    assert transfer_remaining is None
    assert len(steps) == 18, "canonical demo must contain 18 steps"
    assert [step["op"] for step in steps].count("TRANSFER_ITEM") == 4
    assert [step["op"] for step in steps].count("QUERY") == 1
    assert removed == [8, 3, 5, 2], "queue order is not FIFO"
    assert not in_stack and not out_stack
    return len(steps)


if __name__ == "__main__":
    count = validate_trace(Path(__file__).with_name("trace.json"))
    print(f"Valid trace: {count} steps, 4 transfers, 1 query, FIFO preserved")
