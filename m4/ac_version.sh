#!/bin/sh

#VERSION=1.2.0-dev
VERSION=1.1.0-rc1

case "$VERSION" in
    *-dev)
        echo "${VERSION}.$(git rev-parse --short HEAD)"
        ;;
    *)
        echo "$VERSION"
        ;;
esac
