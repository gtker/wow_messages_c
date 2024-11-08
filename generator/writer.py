import typing


class Writer:
    __inner: str = ""
    __indentation: int = 0

    def w(self, s: str):
        for _ in range(0, self.__indentation):
            self.__inner += "    "

        self.__inner += s

    def wln(self, s):
        self.w(s)
        self.newline()

    def open_curly(self, s: str):
        self.wln(f"{s} {{")
        self.inc_indent()

    def closing_curly(self, s: str = ""):
        self.dec_indent()
        self.wln(f"}}{s}")

    def write_block(self, s: str):
        for i, line in enumerate(s.splitlines()):
            if i == 0:
                continue

            if line.strip() == "":
                self.newline()
                continue

            self.wln(line)

    def column(self) -> int:
        for i, ch in enumerate(reversed(self.__inner)):
            if ch == "\n":
                return i

        return len(self.__inner)

    def w_break_at(self, s: str):
        if self.column() >= 80:
            self.newline()
        self.w_no_indent(s)

    def wln_no_indent(self, s):
        self.w_no_indent(s)
        self.newline()

    def w_no_indent(self, s):
        self.__inner += s

    def newline(self):
        self.__inner += "\n"

    def double_newline(self):
        self.newline()
        self.newline()

    def prepend(self, s):
        s.__inner += self.__inner
        self.__inner = s.__inner

    def append(self, s):
        self.__inner += s.__inner

    def inner(self) -> str:
        assert self.__indentation == 0
        return self.__inner

    def inc_indent(self):
        if self.__indentation == 255:
            raise AssertionError("indentation overflow")

        self.__indentation += 1

    def dec_indent(self):
        if self.__indentation == 0:
            raise AssertionError("indentation underflow")

        self.__indentation -= 1
