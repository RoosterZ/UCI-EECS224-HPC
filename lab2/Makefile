CC = icpc
CPPFLAG = -openmp

serial:
	$(CC) #(CPPFLAG) -o $@ pi_serial.cpp

par_false:
	$(CC) #(CPPFLAG) -o $@ pi_parallel_falseSharing.cpp

par_critical:
	$(CC) #(CPPFLAG) -o $@ pi_parallel_critical.cpp

par_reduction:
	$(CC) #(CPPFLAG) -o $@ pi_parallel_reduction.cpp

clean:
	rm -rf serial par_*
