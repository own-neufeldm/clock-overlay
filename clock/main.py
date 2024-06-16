import time
import tkinter as tk
import tkinter.ttk as ttk


class App(tk.Tk):
    def __init__(self, *args, **kwargs) -> None:
        super().__init__(*args, **kwargs)
        self.overrideredirect(True)
        self.attributes('-topmost', True)
        self.resizable(False, False)
        self.clock = ttk.Label(self, background="black", foreground="lime", font="Consolas")
        self.clock.grid(column=0, row=0)

        self.init_x, self.init_y = 0, 0
        self.bind("<ButtonPress-1>", self._save_position)
        self.bind("<B1-Motion>", self._move_window)
        self.bind("<ButtonRelease-3>", self._reset_position)
        self.bind("<ButtonRelease-2>", lambda e: self.quit())

        self._reset_position()
        self._draw_time()
        return None

    def _draw_time(self) -> None:
        self.clock.configure(text=time.strftime("%H:%M"))
        self.clock.after(60000, self._draw_time)
        return None

    def _save_position(self, event: tk.Event) -> None:
        self.init_x, self.init_y = event.x, event.y
        return None

    def _reset_position(self, event: tk.Event | None = None) -> None:
        x, y = self.winfo_screenwidth() - 49, 0
        self.geometry(f"+{x}+{y}")
        return None

    def _move_window(self, event: tk.Event) -> None:
        delta_x = event.x - self.init_x
        delta_y = event.y - self.init_y
        x = self.winfo_x() + delta_x
        y = self.winfo_y() + delta_y
        self.geometry(f"+{x}+{y}")
        return None


def main() -> None:
    app = App()
    app.mainloop()
    return None


if __name__ == "__main__":
    main()
