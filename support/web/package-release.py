#!/usr/bin/env python3
"""Package a tested browser client into a checksummed deployment unit."""
import argparse
import gzip
import hashlib
import io
import json
from pathlib import Path
import re
import tarfile


def package(build, output, revision):
    if not re.fullmatch(r'[0-9a-f]{40}', revision):
        raise ValueError('revision must be the full source commit SHA')
    root = Path(__file__).resolve().parents[2]
    files = {}
    for name in ('index.html', 'launcher.js', 'netpanzer.js', 'netpanzer.wasm', 'netpanzer.data'):
        data = (build / name).read_bytes()
        files['client/' + name] = data
        files['client/' + name + '.gz'] = gzip.compress(data, compresslevel=6, mtime=0)
    files['client/COPYING.txt'] = (root / 'COPYING.txt').read_bytes()
    manifest = {'schema': 1, 'release': revision,
                'source_url': f'https://github.com/netpanzer/netpanzer/tree/{revision}',
                'files': {name: {'size': len(data), 'sha256': hashlib.sha256(data).hexdigest()}
                          for name, data in files.items()}}
    files['manifest.json'] = json.dumps(manifest, sort_keys=True, indent=2).encode() + b'\n'
    output.mkdir(parents=True, exist_ok=True)
    archive = output / f'netpanzer-web-{revision}.tar.gz'
    # Include compressed assets and uncompressed fallbacks together.
    with tarfile.open(archive, 'w:gz') as tar:
        for name, data in sorted(files.items()):
            info = tarfile.TarInfo(name)
            info.size, info.mode, info.mtime = len(data), 0o644, 0
            tar.addfile(info, io.BytesIO(data))
    digest = hashlib.file_digest(archive.open('rb'), 'sha256').hexdigest()
    archive.with_suffix(archive.suffix + '.sha256').write_text(f'{digest}  {archive.name}\n')
    print(archive)


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--build', type=Path, default=Path('build-web'))
    parser.add_argument('--output', type=Path, default=Path('dist-web'))
    parser.add_argument('--revision', required=True)
    args = parser.parse_args()
    package(args.build, args.output, args.revision)
