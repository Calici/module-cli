#!python3
import subprocess
from concurrent.futures import ThreadPoolExecutor
from typing_extensions import List, Literal
import argparse


def get_args(
    platform: Literal["linux/amd64", "linux/arm64"], version: str
) -> List[str]:
    return [
        "docker",
        "build",
        f"--platform={platform}",
        "-o",
        "dist",
        "--build-arg",
        f"PYTHON_VERSION=3.{version}",
        ".",
    ]


subprocess_args = [
    get_args(platform, str(version))  # type: ignore
    for version in range(7, 12)
    for platform in ["linux/amd64", "linux/arm64"]
]


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "--parallel",
        type=bool,
        default=False,
        help="Use False to see more clearly",
        required=False,
    )
    parallel = parser.parse_args().parallel
    if parallel:
        with ThreadPoolExecutor() as t:
            t.map(subprocess.run, subprocess_args)
    else:
        for args in subprocess_args:
            subprocess.run(args)
