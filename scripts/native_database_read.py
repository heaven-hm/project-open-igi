#!/usr/bin/env python3
"""Read SQLite native evidence as a Markdown table; never writes the database."""
import argparse
from pathlib import Path
from native_database import ROOT, read_table

if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--db', type=Path, default=ROOT / 'build/native-auto/natives.db')
    parser.add_argument('--verified', action='store_true', help='Scoped evidence only; does not mean current-build acceptance')
    parser.add_argument('--address')
    args = parser.parse_args()
    print(read_table(args.db, args.verified, args.address))
