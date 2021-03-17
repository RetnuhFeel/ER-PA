er:
	g++ -g -O0 -std=c++11 -Wall -Werror -o er ./src/er.cpp;

pa:
	g++ -g -O0 -std=c++11 -Wall -Werror -o pa ./src/pa.cpp;

all:
	g++ -g -O0 -std=c++11 -Wall -Werror -o er ./src/er.cpp;
	g++ -g -O0 -std=c++11 -Wall -Werror -o pa ./src/pa.cpp;

clean-er:
	rm er;

clean-pa:
	rm pa;

clean-all:
	rm er;
	rm pa;