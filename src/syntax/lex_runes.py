
cc = [
    (1, " \r\n\t\b\f"),
    (2, "0123456789"),
    (4, "+-*/=<>"),
    (8, "{}[](),.;"),
    ]

print("namespace lex {")
print("namespace runes {")

print("char char_class[128] = {", end="")
for i in range(128):
    if i % 16 == 0:
        print("\n    ", end="")

    flags = 0
    for f,space in cc:
        if chr(i) in space:
            flags = flags | f
    print("%d," % flags, end="")

print("\n};")
print("}}")
