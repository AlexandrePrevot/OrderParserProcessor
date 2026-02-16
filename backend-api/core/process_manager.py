import os
import subprocess
from typing import Dict, Tuple


class ProcessManager:
    def __init__(self):
        self._processes: Dict[Tuple[str, str], subprocess.Popen] = {}

    def _sanitize(self, name: str) -> str:
        return name.strip().lower().replace(" ", "_")

    def _build_binary_path(self, username: str, title: str) -> str:
        project_root = os.getenv("ORDER_PARSER_PROCESSOR_ROOT")
        if not project_root:
            raise EnvironmentError("ORDER_PARSER_PROCESSOR_ROOT is not set")

        sanitized_user = self._sanitize(username)
        sanitized_title = self._sanitize(title)

        return os.path.join(
            project_root, "..", "output_bin",
            sanitized_user, sanitized_title,
            "build", sanitized_title
        )

    def is_active(self, username: str, title: str) -> bool:
        key = (username, title)
        proc = self._processes.get(key)
        if proc is None:
            return False
        if proc.poll() is not None:
            del self._processes[key]
            return False
        return True

    def activate(self, username: str, title: str) -> None:
        if self.is_active(username, title):
            raise RuntimeError(f"Script '{title}' for user '{username}' is already active")

        binary_path = self._build_binary_path(username, title)
        if not os.path.isfile(binary_path):
            raise FileNotFoundError(f"Binary not found at {binary_path}. Has the script been saved and compiled?")

        proc = subprocess.Popen(
            [binary_path],
            stdout=subprocess.PIPE,
            stderr=subprocess.PIPE,
        )
        self._processes[(username, title)] = proc

    def deactivate(self, username: str, title: str) -> None:
        key = (username, title)
        proc = self._processes.get(key)

        if proc is None or proc.poll() is not None:
            self._processes.pop(key, None)
            raise RuntimeError(f"Script '{title}' for user '{username}' is not active")

        proc.terminate()
        try:
            proc.wait(timeout=5)
        except subprocess.TimeoutExpired:
            proc.kill()
            proc.wait()

        del self._processes[key]

    def toggle(self, username: str, title: str) -> bool:
        if self.is_active(username, title):
            self.deactivate(username, title)
            return False
        else:
            self.activate(username, title)
            return True

    def shutdown_all(self):
        for key in list(self._processes.keys()):
            proc = self._processes[key]
            if proc.poll() is None:
                proc.terminate()
                try:
                    proc.wait(timeout=5)
                except subprocess.TimeoutExpired:
                    proc.kill()
            del self._processes[key]
