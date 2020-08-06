
# disabled (not compiling on 5.0+): sbull snull short
SUBDIRS =  scull scullp

all: subdirs

subdirs:
	for n in $(SUBDIRS); do $(MAKE) -C $$n || exit 1; done

clean:
	for n in $(SUBDIRS); do $(MAKE) -C $$n clean; done
