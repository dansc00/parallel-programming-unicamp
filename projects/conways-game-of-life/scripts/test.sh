make  
#TEST=random-1000-1000-50
TEST=random-1000-1000-1
mkdir bench/ -p
./serial-matrix   ../tests/$TEST.in >| ./serial-matrix.$TEST.out 2>| ./bench/serial-matrix.$TEST.yaml
./parallel-matrix ../tests/$TEST.in >| ./parallel-matrix.$TEST.out 2>| ./bench/parallel-matrix.$TEST.yaml
diff -u ./parallel-matrix.$TEST.out ./serial-matrix.$TEST.out
./parallel-set    ../tests/$TEST.in >| ./parallel-set.$TEST.out 2>| ./bench/parallel-set.$TEST.yaml
diff -u ./parallel-set.$TEST.out ./serial-matrix.$TEST.out
./serial-set      ../tests/$TEST.in >| ./serial-set.$TEST.out 2>| ./bench/serial-set.$TEST.yaml
diff -u ./serial-set.$TEST.out ./serial-matrix.$TEST.out
./serial-vecmatrix      ../tests/$TEST.in >| ./serial-vecmatrix.$TEST.out 2>| ./bench/serial-vecmatrix.$TEST.yaml
diff -u ./serial-vecmatrix.$TEST.out ./serial-matrix.$TEST.out