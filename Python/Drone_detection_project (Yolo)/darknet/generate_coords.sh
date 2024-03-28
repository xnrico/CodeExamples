for file in ../frames/cam0/*; do
	./darknet detector test ./data/obj.data ./cfg/yolov4-obj.cfg ../backup/yolov4-obj_3000.weights "$file" -dont_show -thresh 0.95 -out ./result.txt
    
    mv ./result.txt ../results/cam0/$(basename "$file" .jpg).txt
done

#$(basename "$file" .jpg)


