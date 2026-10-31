# matrix_builder.py
#
# Reads a laptop keyboard pin connection file and finds the row/column
# pins and base-layer keymap for the Pi Pico with or without an MCP23018 port expander.
# The results are geared towards QMK but the data can also be used for tinyUSB C++ code (and others).
# Pin list format (one key per line, tabs or spaces between fields):
#     KC_A        MCP_GPB5 + MCP_GPA1
#     KC_B        Pico_GP8 + Pico_GP19
#     KC_F13                              <- key not on this keyboard: skipped
#
# Pin numbering:
#     Pico_GP0 - GP28   ->   0 - 28
#     MCP_GPA0 - GPA7   -> 100 - 107 (port expander A register)
#     MCP_GPB0 - GPB7   -> 108 - 115 (port expander B register)
#
# How rows and columns are found:
# Every key connects exactly one row pin to one column pin, so no key ever
# connects two rows or two columns. The program follows the key connections
# and splits the pins into two groups where every key has one pin in each
# group. The smaller group becomes the columns and the larger group the rows.
#
# Results are printed in the Thonny console and saved to a text file next to
# the pin list file (<pin list name>_results.txt).
#
# Written for Thonny on Windows (standard Python, no extra packages needed).

import os
import re
import sys

# ---------------------------------------------------------------------------
# Old QMK key names that were removed from current QMK, and their new names.
# Names on the left are translated automatically so the keymap compiles.
# ---------------------------------------------------------------------------
RENAMED_KEYCODES = {
    "KC_LCTRL": "KC_LCTL",
    "KC_RCTRL": "KC_RCTL",
    "KC_LSHIFT": "KC_LSFT",
    "KC_RSHIFT": "KC_RSFT",
    "KC_LCMD": "KC_LGUI",
    "KC_RCMD": "KC_RGUI",
    "KC_LWIN": "KC_LGUI",
    "KC_RWIN": "KC_RGUI",
    "KC_BSPACE": "KC_BSPC",
    "KC_SCOLON": "KC_SCLN",
    "KC_BSLASH": "KC_BSLS",
    "KC_ZKHK": "KC_GRV",
    "KC_LOCKING_CAPS": "KC_LCAP",
    "KC_CAPSLOCK": "KC_CAPS",
    "KC_NUMLOCK": "KC_NUM",
    "KC_SCROLLLOCK": "KC_SCRL",
    "KC_PSCREEN": "KC_PSCR",
    "KC_PGDOWN": "KC_PGDN",
    "KC_NONUS_BSLASH": "KC_NUBS",
    "KC__MUTE": "KC_KB_MUTE",
    "KC__VOLUP": "KC_KB_VOLUME_UP",
    "KC__VOLDOWN": "KC_KB_VOLUME_DOWN",
#    "FN": "MO(1)",       # Fn key: hold for layer 1
}

EMPTY_KEY = "KC_NO"

PIN_PATTERN = re.compile(r"(Pico_GP|MCP_GPA|MCP_GPB)(\d+)", re.IGNORECASE)


def pin_number(prefix, bit):
    """Convert a pin name to the 0 - 115 numbering. Returns None if invalid."""
    prefix = prefix.upper()
    if prefix == "PICO_GP":
        if 0 <= bit <= 28 and bit not in (23, 24, 25):   # 23-25 are internal
            return bit
        return None
    if 0 <= bit <= 7:
        return (100 if prefix == "MCP_GPA" else 108) + bit
    return None


def pin_label(num):
    """Readable label for a pin number, e.g. 7 -> 'GP7', 105 -> 'GPA5'."""
    if num >= 108:
        return "GPB%d" % (num - 108)
    if num >= 100:
        return "GPA%d" % (num - 100)
    return "GP%d" % num


def choose_input_file():
    """Use the file name given on the command line, a file picker, or typed input."""
    if len(sys.argv) > 1:
        return sys.argv[1]
    try:
        import tkinter
        from tkinter import filedialog
        root = tkinter.Tk()
        root.withdraw()
        root.attributes("-topmost", True)
        path = filedialog.askopenfilename(
            title="Select the keyboard pin list text file",
            filetypes=[("Text files", "*.txt"), ("All files", "*.*")])
        root.destroy()
        if path:
            return path
    except Exception:
        pass
    return input("Enter the full path of the pin list text file: ").strip().strip('"')


def read_pin_list(path, report):
    """Return a list of (key_name, pin_a, pin_b) for keys that have two pins."""
    keys = []
    with open(path, "r", encoding="utf-8-sig", errors="replace") as f:
        for line_no, raw in enumerate(f, start=1):
            line = raw.strip()
            if not line or line.startswith("#") or line.startswith("//"):
                continue
            name = line.split()[0]
            found = PIN_PATTERN.findall(line)
            if not found:
                continue                      # key not present on this keyboard
            if len(found) != 2:
                report("WARNING line %d: expected 2 pins, found %d - skipped: %s"
                       % (line_no, len(found), line))
                continue
            pins = []
            for prefix, bit in found:
                num = pin_number(prefix, int(bit))
                if num is None:
                    report("WARNING line %d: invalid pin %s%s - skipped"
                           % (line_no, prefix, bit))
                    break
                pins.append(num)
            else:
                if pins[0] == pins[1]:
                    report("WARNING line %d: both pins are the same - skipped: %s"
                           % (line_no, line))
                    continue
                keys.append((name, pins[0], pins[1]))
    return keys


def split_rows_and_columns(keys, report):
    """Two-color the pin connection graph. Smaller side of each group = columns."""
    neighbors = {}
    for _, a, b in keys:
        neighbors.setdefault(a, set()).add(b)
        neighbors.setdefault(b, set()).add(a)

    color = {}
    rows, cols = set(), set()
    ok = True
    for start in sorted(neighbors):
        if start in color:
            continue
        # Walk one connected group of pins and give alternate pins sides 0 and 1
        color[start] = 0
        group = [start]
        stack = [start]
        while stack:
            p = stack.pop()
            for q in neighbors[p]:
                if q not in color:
                    color[q] = 1 - color[p]
                    group.append(q)
                    stack.append(q)
                elif color[q] == color[p]:
                    ok = False
        side0 = {p for p in group if color[p] == 0}
        side1 = {p for p in group if color[p] == 1}
        if len(side0) <= len(side1):
            cols |= side0
            rows |= side1
        else:
            cols |= side1
            rows |= side0

    if not ok:
        report("WARNING: some keys connect two pins that ended up on the same side.")
        report("         Check the pin list for typing errors (see conflicts below).")
    return sorted(rows), sorted(cols)


def build_matrix(keys, rows, cols, report):
    row_index = {p: i for i, p in enumerate(rows)}
    col_index = {p: i for i, p in enumerate(cols)}
    matrix = [[EMPTY_KEY] * len(cols) for _ in rows]
    renamed = []

    for name, a, b in keys:
        if a in row_index and b in col_index:
            r, c = row_index[a], col_index[b]
        elif b in row_index and a in col_index:
            r, c = row_index[b], col_index[a]
        else:
            report("CONFLICT: %s connects %s and %s, which are both %s - not placed"
                   % (name, pin_label(a), pin_label(b),
                      "rows" if a in row_index else "columns"))
            continue
        qmk_name = RENAMED_KEYCODES.get(name.upper(), name)
        if qmk_name != name:
            renamed.append((name, qmk_name))
        if matrix[r][c] != EMPTY_KEY:
            report("CONFLICT: %s and %s are both at row %s / column %s"
                   % (matrix[r][c], qmk_name, pin_label(rows[r]), pin_label(cols[c])))
        matrix[r][c] = qmk_name
    return matrix, renamed


def format_keymap(matrix, rows, cols):
    width = max([len(k) for line in matrix for k in line] + [7]) + 2  # name + ", "
    header = "    //  " + "".join(pin_label(p).ljust(width) for p in cols).rstrip()
    lines = [header]
    for r, row in enumerate(matrix):
        last_row = (r == len(matrix) - 1)
        cells = []
        for c, key in enumerate(row):
            last = last_row and c == len(row) - 1
            cells.append((key if last else key + ",").ljust(width))
        lines.append("        " + "".join(cells) + "// " + pin_label(rows[r]))
    return lines


def main():
    path = choose_input_file()
    if not path or not os.path.isfile(path):
        print("File not found:", path)
        return

    output = []

    def report(text=""):
        print(text)
        output.append(text)

    report("Pin list file: " + path)
    report()

    keys = read_pin_list(path, report)
    if not keys:
        report("No keys with pin connections were found in the file.")
        return
    report("Keys with pin connections: %d" % len(keys))

    rows, cols = split_rows_and_columns(keys, report)
    matrix, renamed = build_matrix(keys, rows, cols, report)

    report()
    report("Number of row pins:    %d" % len(rows))
    report("Number of column pins: %d" % len(cols))
    report()
    report("Row pins:    " + ", ".join(str(p) for p in rows))
    report("Column pins: " + ", ".join(str(p) for p in cols))
    report()
    report("(Pico GP0-28 = 0-28, MCP23018 GPA0-7 = 100-107, GPB0-7 = 108-115)")
    report("Rows:    " + ", ".join(pin_label(p) for p in rows))
    report("Columns: " + ", ".join(pin_label(p) for p in cols))

    report()
    report("---- matrix.c ----")
    report("static const uint8_t row_pins[MATRIX_ROWS] = {%s};"
           % ", ".join(str(p) for p in rows))
    report("static const uint8_t col_pins[MATRIX_COLS] = {%s};"
           % ", ".join(str(p) for p in cols))

    report()
    report("---- keyboard.json ----")
    report('    "matrix_size": {"rows": %d, "cols": %d},' % (len(rows), len(cols)))

    if renamed:
        report()
        report("Old key names translated to current QMK names:")
        for old, new in renamed:
            report("    %-12s -> %s" % (old, new))

    report()
    report("---- keymap.c base layer ----")
    report("    [0] = LAYOUT(")
    for line in format_keymap(matrix, rows, cols):
        report(line)
    report("    ),")

    out_path = os.path.splitext(path)[0] + "_Results.txt"
    try:
        with open(out_path, "w", encoding="utf-8") as f:
            f.write("\n".join(output) + "\n")
        print()
        print("Saved to: " + out_path)
    except OSError as e:
        print()
        print("Could not save the output file:", e)


if __name__ == "__main__":
    main()
