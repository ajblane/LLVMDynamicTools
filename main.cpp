#include "main.h"
#include "interpreter.h"

#include <string>

using namespace llvm;

namespace {
	cl::opt<std::string> bitcodeFile(cl::desc("<input bitcode (*.ll or *.bc)>"), cl::Positional, cl::Required);
	cl::list<std::string>  Argv(cl::ConsumeAfter, cl::desc("<program arguments>..."));
}	// end of anonymous namespace

int main(int argc, char **argv, char * const *envp) {
	cl::ParseCommandLineOptions(argc, argv, "Jia's execution tracer\n");
	errs() << "Hello, " << bitcodeFile << " !\n";

	// Load the bitcode
	LLVMContext& context = getGlobalContext();
	SMDiagnostic errDiag;
	Module* module = ParseIRFile(bitcodeFile, errDiag, context);
	if (module == NULL)
	{
		errs() << "Bitcode parsing failed!\n";
		return -1;
	}

	// Check the bitcode
	std::string errMsg;
	if (module->MaterializeAllPermanently(&errMsg))
	{
		errs() << "bitcode read error: " << errMsg << "\n";
		return -1;
	}

	MyInterpreter* interp = new MyInterpreter(module);
	Function* startFunc = module->getFunction("main");
	if (startFunc == NULL)
	{
		errs() << "Cannot find main function!\n";
		return -1;
	}
	Argv.insert(Argv.begin(), bitcodeFile);
	int retValue = interp->runFunctionAsMain(startFunc, Argv, envp);
	errs() << "Interpreter returns " << retValue << "\n";

	errs() << "Bye-bye!\n";

	return 0;
}
