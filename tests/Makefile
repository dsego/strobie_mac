# Copyright (c) 2013 Davorin Šego. All rights reserved.

UNITY = -I../../Unity/src ../../Unity/src/unity.c
INCL =  -I../src/engine/ ../src/engine/trial.c
WARN =  -Wall -Wno-unused-variable -Wno-unused-function -Wno-deprecated-declarations

.PHONY: test

test:
	make runner
	cc $(UNITY) $(INCL) $(WARN) *.c -o test

runner:
	ruby ../../Unity/auto/generate_test_runner.rb test.c
