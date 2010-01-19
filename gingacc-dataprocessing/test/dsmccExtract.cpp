/* dsmcc-extract -- extract a dsmcc file system from a transport stream. */

#include <stdlib.h>
#include <stdio.h>

#include "../include/DataProcessor.h"
using namespace ::br::pucrio::telemidia::ginga::core::dataprocessing;

#include "../../gingacc-tsparser/include/Demuxer.h"
using namespace ::br::pucrio::telemidia::ginga::core::tsparser;

#include "../../gingacc-tuner/include/Tuner.h"
#include "tuner/ITuner.h"
using namespace ::br::pucrio::telemidia::ginga::core::tuning;




#define PROGRAM_NAME   "dsmccExtract"
#define BUFFER_SIZE    ( 188 * 10 ) + 1


#define err(args...)					\
	do {						\
		fprintf(stderr, "%s: ", PROGRAM_NAME);	\
		fprintf(stderr, args);			\
	} while (0)

void usage()
{
	fputs("\
Usage: dsmccExtract FILE\n\
Extract a DSMCC file system from a TS file.\n\
", stdout);
}

int main(int argc, char** argv)
{
	char* filename = NULL;
	char* buf = NULL;
	int fd = -1;
	int numbytes = 0;
	IDemuxer* demuxer = NULL;
	IDataProcessor* dataproc = NULL;
	ITuner* tuner;

	if (argc < 2) {
		usage();
		exit(EXIT_FAILURE);
	}

	filename = argv[1];
	if ((fd = open(filename, O_RDONLY | O_LARGEFILE)) < 0) {
		err("unable to open %s\n", filename);
		exit(EXIT_FAILURE);
	}

	if ((buf = (char*) malloc(BUFFER_SIZE)) == NULL) {
		err("buffer allocation failed\n");
		exit(EXIT_FAILURE);
	}

	// Register dsmcc carousel filter.
	tuner = new Tuner();
	demuxer = new Demuxer(tuner);
	dataproc = new DataProcessor();
	dataproc->createStreamTypeSectionFilter(0x0B, demuxer);


	int k = 0;
    while ((numbytes = read(fd, buf, BUFFER_SIZE)) > 0) {
		if(numbytes < BUFFER_SIZE)
			break;
		//_debug("Passando buffer comecando no byte %d\n",k);
		demuxer->receiveData(buf, numbytes);
		lseek(fd,-1,SEEK_CUR);
		k = k + numbytes;
	}

	delete dataproc;
	delete demuxer;
	delete tuner;
	close(fd);
	exit(EXIT_SUCCESS);
}
