#!/bin/sh

VERSION=1.2.0-dev

case "$VERSION" in
    *-dev)
        echo "${VERSION}.$(git rev-parse --short HEAD)"
        ;;
    *)
        echo "$VERSION"
        ;;
esac
