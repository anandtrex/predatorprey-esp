#!/lusr/bin/bash
rm -rf src/
mkdir src
cp *.cpp src/
cp *.h src/
cp -r Experiment/ src/
cp -r Domain/ src/
cp -r Esp/ src/
tar czf anand.tgz src/
