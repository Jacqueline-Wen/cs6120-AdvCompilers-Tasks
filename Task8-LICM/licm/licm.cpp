#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/ValueTracking.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Instruction.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"
#include <vector>

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (Function &F : M) {
            if (F.isDeclaration())
                continue; // skip external functions

            FunctionAnalysisManager FAM;
            PassBuilder PB;
            PB.registerFunctionAnalyses(FAM);

            LoopAnalysis::Result &LI = FAM.getResult<LoopAnalysis>(F);

            for (Loop *L : LI) {
                errs() << "starting loop\n";
                BasicBlock *preheader = L->getLoopPreheader();
                if (!preheader) {
                    errs() << "preheader is equal to null\n";
                    return PreservedAnalyses::none();
                }
                std::vector<const Instruction *> lIInstructions;
                bool notConverged = true;
                while (notConverged) {
                    notConverged = false;
                    // iterating through basic blocks
                    for (auto *B : L->blocks()) {
                        // iterating through instructions of basic block B
                        for (const auto &I : *B) {
                            if (L->hasLoopInvariantOperands(&I) &&
                                // L->isLoopInvariant(&I) &&
                                !I.mayReadOrWriteMemory() &&
                                isSafeToSpeculativelyExecute(&I)) {
                                errs() << "adding somehting to LIInstr\n";
                                lIInstructions.push_back(&I);
                                notConverged = true;
                            }
                        }
                    }

                    for (auto &a : lIInstructions) {
                        const_cast<Instruction *>(a)->moveBefore(
                            preheader->getTerminator());
                        errs() << "Moving LI: " << *a << "\n";
                    }
                    lIInstructions.clear();
                }
            }
        }
        return PreservedAnalyses::all();
    }
};

} // namespace

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {.APIVersion = LLVM_PLUGIN_API_VERSION,
            .PluginName = "LICM pass",
            .PluginVersion = "v0.1",
            .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
                PB.registerPipelineStartEPCallback(
                    [](ModulePassManager &MPM, OptimizationLevel Level) {
                        MPM.addPass(LICMPass());
                    });
            }};
}