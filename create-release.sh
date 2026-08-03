#!/bin/bash

# Check if version argument is provided
if [ -z "$1" ]; then
  echo "Usage: $0 <version>"
  exit 1
fi

VERSION=$1
RELEASE_DIR="wlang-${VERSION}_linux_amd64"

# Create release folder
mkdir -p "${RELEASE_DIR}"

# Compile wlangc and libw
cd compiler || exit 1
make clean || exit 1
make release || exit 1
cd ../stdlib || exit 1
make clean || exit 1
make default || exit 1  # Build stdlib

cd ..

# Move the generated files into the release folder
mv ./wlangc "./${RELEASE_DIR}/wlangc"
mv ./libw.a "./${RELEASE_DIR}/libw.a"

# Copy additional files to the release folder
cp ./CHANGELOG.txt "./${RELEASE_DIR}/CHANGELOG.txt"
cp ./release-data/README-release.md "./${RELEASE_DIR}/README.md"

# Copy the hello-world example
cp -r ./release-data/hello-world "./${RELEASE_DIR}/"

# Compress release folder into a tar.gz archive
tar -czvf "${RELEASE_DIR}.tar.gz" "${RELEASE_DIR}"

# Clean up the release directory (optional)
rm -rf "${RELEASE_DIR}"

echo "Release ${VERSION} created: ${RELEASE_DIR}.tar.gz"
