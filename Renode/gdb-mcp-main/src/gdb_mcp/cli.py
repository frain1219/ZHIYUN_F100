#!/usr/bin/env python3
"""CLI entry point for the GDB MCP server."""

from .server import run


def main():
    """Synchronous wrapper for the server run function."""
    run()


if __name__ == "__main__":
    main()
