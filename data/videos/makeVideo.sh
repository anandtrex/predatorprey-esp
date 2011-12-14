# cd test
#rm *.webm
#rm *.jpg
# find . -name \*.png | sed 's/png//g' | xargs -n 1 -I {} convert -verbose {}png {}jpg
# ffmpeg -f image2 -i vid-%d.png test.webm
ffmpeg -r 5 -f image2 -i vid-%d.png test.mp4
echo "test.webm written!"
