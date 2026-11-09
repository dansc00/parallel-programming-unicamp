make  
TEST=random-10000-10000-1
hyperfine -r 1 --export-markdown results.md \
  "./parallel-set    ../tests/$TEST.in" \
  "./serial-set      ../tests/$TEST.in" \
  "./parallel-matrix ../tests/$TEST.in" \
  "./serial-matrix   ../tests/$TEST.in" 

#  "./serial-vecmatrix   ../tests/$TEST.in" \