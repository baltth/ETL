#!/usr/bin/env bash

set -euo pipefail

gcovr -e tests/ $@
