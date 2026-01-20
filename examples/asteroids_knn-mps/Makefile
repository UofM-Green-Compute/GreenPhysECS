#!/bin/bash

# Base Working Directory
BWD := $(shell pwd)

BWDMOUNT := -v $(BWD):$(BWD):ro
BUILDMOUNT := -v $(BWD)/build:$(BWD)/build
BINMOUNT := -v $(BWD)/bin:$(BWD)/bin
INPUTSMOUNT := -v $(BWD)/inputs:$(BWD)/inputs
OUTPUTSMOUNT := -v $(BWD)/outputs:$(BWD)/outputs

INCLUDEMOUNT := -v $(BWD)/../../include/:$(BWD)/include


MOUNTS := $(BWDMOUNT) $(BUILDMOUNT) $(BINMOUNT) $(INPUTSMOUNT) $(OUTPUTSMOUNT) $(INCLUDEMOUNT)

all:
	@echo "make docker - build docker container"
	@echo "make prepare - create build location"
	@echo "make dockerbash - run bash inside the container"
	@echo "make dockerbuild - build the code inside the container"
	@echo "make clean - wipe the build"
	@echo
	@echo "NB: final artefacts live in 'bin'"

env:
	@echo "$(BWD)"

src/flecs.c:
	cp ../../src/flecs.c src

Dockerfile:
	cp ../../Dockerfile .

docker: Dockerfile
	docker build -t buildenv -f Dockerfile .

prepare:
	mkdir -p $(BWD)/build
	mkdir -p $(BWD)/bin
	mkdir -p $(BWD)/include

clean:
	rm -rf $(BWD)/build
	rm -rf $(BWD)/bin
	rm -rf $(BWD)/include
	rm -f Dockerfile
	rm -f src/flecs.c

dockerbash: prepare Dockerfile
	docker run -it --rm -u1000:1000 -e BWD=$(BWD) \
	           $(MOUNTS) \
	           buildenv \
	           /bin/bash

run: prepare
	docker run -it --rm -u1000:1000 -e BWD=$(BWD) \
	           $(MOUNTS) \
	           buildenv \
	           make BWD=$(BWD) -f $(BWD)/src/Makefile run

dockerbuild: prepare Dockerfile src/flecs.c
	docker run -it --rm -u1000:1000 -e BWD=$(BWD) \
	           $(MOUNTS) \
	           buildenv \
	           make -f $(BWD)/src/Makefile

dockerpandoc: prepare Dockerfile
	docker run -it --rm -u1000:1000 -e BWD=$(BWD) \
	           $(MOUNTS) \
	           -v $(BWD)/docs/gravity_presentation/:$(BWD)/docs/gravity_presentation/ \
	           buildenv \
	           make -C $(BWD)/docs/gravity_presentation/ -f $(BWD)/docs/gravity_presentation/Makefile

devloop:
	make clean
	make prepare
	make dockerbuild
	make run
