"""Lists the game's single-player assumptions for online co-op (docs/COOP_PLAN.md): each call that reaches for "the"
player, counted by subsystem (Game/<subsystem>, or the library) and by kind.

    python coop_inventory.py                 counts per subsystem and kind
    python coop_inventory.py --sites [kind]  every site, with its line (one kind, or all)
"""
import collections
import pathlib
import re
import sys

SRC = pathlib.Path(__file__).resolve().parents[1] / "petari" / "src"

# The kinds, in the order the plan works through them. Each pattern matches one site per occurrence.
KINDS = {
    "getPlayer": re.compile(r"\bMR::getPlayer(?!DemoActor\b)\w*\b"),
    "isPlayer": re.compile(r"\bMR::isPlayer\w*\b"),
    "nearPlayer": re.compile(r"\bMR::is\w*NearPlayer\w*\b|\bMR::calcDistanceToPlayer\w*\b"),
    "MarioAccess": re.compile(r"\bMarioAccess::\w+\b"),
    "MarioActor": re.compile(r"\bMarioActor\s*\*"),
    "demoActor": re.compile(r"\bMR::getPlayerDemoActor\b|\bMR::\w*PlayerDemo\w*\b"),
}


def subsystem(path):
    parts = path.relative_to(SRC).parts
    return "/".join(parts[:2]) if parts[0] == "Game" and len(parts) > 2 else parts[0]


def sites():
    for path in sorted(SRC.rglob("*")):
        if path.suffix not in (".cpp", ".c", ".hpp", ".h", ".inc"):
            continue
        text = path.read_text(encoding="utf-8", errors="replace")
        for number, line in enumerate(text.splitlines(), 1):
            stripped = line.strip()
            if stripped.startswith("//"):
                continue
            for kind, pattern in KINDS.items():
                for match in pattern.finditer(line):
                    yield kind, path, number, match.group(0), stripped


def main():
    args = sys.argv[1:]
    if args and args[0] == "--sites":
        wanted = args[1] if len(args) > 1 else None
        if wanted is not None and wanted not in KINDS:
            sys.exit(f"unknown kind {wanted}; kinds: {', '.join(KINDS)}")
        for kind, path, number, name, line in sites():
            if wanted is None or kind == wanted:
                print(f"{kind:12} {path.relative_to(SRC).as_posix()}:{number} {name}: {line[:150]}")
        return
    counts = collections.defaultdict(collections.Counter)
    for kind, path, _, _, _ in sites():
        counts[subsystem(path)][kind] += 1
    width = max([len(name) for name in counts] + [len("total")])
    print(f"{'subsystem':{width}} " + " ".join(f"{kind:>11}" for kind in KINDS) + f" {'all':>6}")
    totals = collections.Counter()
    for name in sorted(counts, key=lambda name: -sum(counts[name].values())):
        row = counts[name]
        totals.update(row)
        print(f"{name:{width}} " + " ".join(f"{row[kind]:>11}" for kind in KINDS) + f" {sum(row.values()):>6}")
    print(f"{'total':{width}} " + " ".join(f"{totals[kind]:>11}" for kind in KINDS) + f" {sum(totals.values()):>6}")


if __name__ == "__main__":
    main()
