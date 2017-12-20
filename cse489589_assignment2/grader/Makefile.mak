all: exp1 exp2

exp1:
	 ./run_experiments -p ../rohinkum/abt -o abt.txt -m 1000 -t 50 -c 0.2 -w 0 -l .1
	 ./run_experiments -p ../rohinkum/abt -o abt.txt -m 1000 -t 50 -c 0.2 -w 0 -l .2
	 ./run_experiments -p ../rohinkum/abt -o abt.txt -m 1000 -t 50 -c 0.2 -w 0 -l .4
	 ./run_experiments -p ../rohinkum/abt -o abt.txt -m 1000 -t 50 -c 0.2 -w 0 -l .6
	 ./run_experiments -p ../rohinkum/abt -o abt.txt -m 1000 -t 50 -c 0.2 -w 0 -l .8
	 
	 ./run_experiments -p ../rohinkum/gbn -o gbn10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .1
	 ./run_experiments -p ../rohinkum/gbn -o gbn10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .2
	 ./run_experiments -p ../rohinkum/gbn -o gbn10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .4
	 ./run_experiments -p ../rohinkum/gbn -o gbn10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .6
	 ./run_experiments -p ../rohinkum/gbn -o gbn10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .8
	 
	 ./run_experiments -p ../rohinkum/gbn -o gbn50.txt -m 1000 -t 50 -c 0.2 -w 50 -l .1
	 ./run_experiments -p ../rohinkum/gbn -o gbn50.txt -m 1000 -t 50 -c 0.2 -w 50 -l .2
	 ./run_experiments -p ../rohinkum/gbn -o gbn50.txt -m 1000 -t 50 -c 0.2 -w 50 -l .4
	 ./run_experiments -p ../rohinkum/gbn -o gbn50.txt -m 1000 -t 50 -c 0.2 -w 50 -l .6
	 ./run_experiments -p ../rohinkum/gbn -o gbn50.txt -m 1000 -t 50 -c 0.2 -w 50 -l .8
	 
	 ./run_experiments -p ../rohinkum/sr -o sr10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .1
	 ./run_experiments -p ../rohinkum/sr -o sr10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .2
	 ./run_experiments -p ../rohinkum/sr -o sr10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .4
	 ./run_experiments -p ../rohinkum/sr -o sr10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .6
	 ./run_experiments -p ../rohinkum/sr -o sr10.txt -m 1000 -t 50 -c 0.2 -w 10 -l .8
	 
	 ./run_experiments -p ../rohinkum/sr -o sr50.txt -m 1000 -t 50 -c 0.2 -w 10 -l .1
	 ./run_experiments -p ../rohinkum/sr -o sr50.txt -m 1000 -t 50 -c 0.2 -w 10 -l .2
	 ./run_experiments -p ../rohinkum/sr -o sr50.txt -m 1000 -t 50 -c 0.2 -w 10 -l .4
	 ./run_experiments -p ../rohinkum/sr -o sr50.txt -m 1000 -t 50 -c 0.2 -w 10 -l .6
	 ./run_experiments -p ../rohinkum/sr -o sr50.txt -m 1000 -t 50 -c 0.2 -w 10 -l .8

exp2:
	./run_experiments -p ../rohinkum/abt -o abt2.txt -m 1000 -t 50 -c 0.2 -w 0 -l .2
	./run_experiments -p ../rohinkum/abt -o abt2.txt -m 1000 -t 50 -c 0.2 -w 0 -l .5
	./run_experiments -p ../rohinkum/abt -o abt2.txt -m 1000 -t 50 -c 0.2 -w 0 -l .8

	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .2
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .5
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .8

	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .2
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .5
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .8
	
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .2
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .5
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .8

	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .2
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .5
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .8
	
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .2
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .5
	./run_experiments -p ../rohinkum/gbn -o gbn2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .8
	
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .2
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .5
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 10 -l .8

	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .2
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .5
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 50 -l .8
	
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .2
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .5
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 100 -l .8

	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .2
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .5
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 200 -l .8
	
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .2
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .5
	./run_experiments -p ../rohinkum/sr -o sr2.txt -m 1000 -t 50 -c 0.2 -w 500 -l .8
	 	 
clean:
	rm abt.txt
	rm gbn10.txt
	rm gbn50.txt
	rm sr10.txt
	rm sr50.txt
	rm abt2.txt
	rm gbn2.txt
	rm sr2.txt
