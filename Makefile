all: tests/Test_Mytherads tests/example_usage tests/suspend_resume_yield_demo

tests/Test_Mytherads:
	gcc -Wall -g -o tests/Test_Mytherads tests/Test_Mytherads.c Mythreads.c

tests/example_usage:
	gcc -Wall -g -o tests/example_usage tests/example_usage.c Mythreads.c

tests/suspend_resume_yield_demo:
	gcc -Wall -g -o tests/suspend_resume_yield_demo tests/suspend_resume_yield_demo.c Mythreads.c

clean:
	rm -f tests/Test_Mytherads tests/example_usage tests/suspend_resume_yield_demo test