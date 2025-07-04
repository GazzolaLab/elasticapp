from pathlib import Path


def get_data_path(data_dir: str | None = None) -> Path:
    if data_dir is not None:
        path = Path(data_dir).expanduser().resolve()
    else:
        path = Path.cwd() / "data"

    path.mkdir(parents=True, exist_ok=True)
    return path


def get_most_recent_data_file(data_dir: str, file_pattern: str) -> str:
    data_path = Path(data_dir).expanduser().resolve()
    files = sorted(data_path.glob(file_pattern))

    if not files:
        raise FileNotFoundError(
            f"No files matching pattern '{file_pattern}' in {data_path}"
        )

    return str(files[-1])
