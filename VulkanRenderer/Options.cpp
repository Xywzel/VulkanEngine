#include "Options.h"

const Options& getOptions()
{
	static Options options;
	// TODO: Load options at start up
	return options;
}
