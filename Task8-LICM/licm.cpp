#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Module.h"
#include "llvm/Pass.h"
#include "llvm/Passes/PassBuilder.h"
#include "llvm/Passes/PassPlugin.h"
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

namespace {

struct LICMPass : public PassInfoMixin<LICMPass> {
    bool isZeroValue(llvm::Value *V) {
        if (auto *ci = llvm::dyn_cast<llvm::ConstantInt>(V)) {
            return ci->isZero();
        }
        if (auto *cf = llvm::dyn_cast<llvm::ConstantFP>(V)) {
            return cf->isZero();
        }
        return false;
    }

    PreservedAnalyses run(Module &M, ModuleAnalysisManager &AM) {
        for (auto &F : M) {
            for (auto &B : F) {
                for (auto it = B.begin(), end = B.end(); it != end;) {
                    Instruction &I = *it++;
                    if (auto *op = dyn_cast<BinaryOperator>(&I)) {
                        if (op->getOpcode() == Instruction::SDiv ||
                            op->getOpcode() == Instruction::UDiv) {

                            Value *lhs = op->getOperand(0);
                            Value *rhs = op->getOperand(1);
                            IRBuilder<> Builder(op);

                            Value *zero = ConstantInt::get(rhs->getType(), 0);
                            Value *isZero =
                                Builder.CreateICmpEQ(rhs, zero, "iszero");

                            Value *one = ConstantInt::get(rhs->getType(), 1);
                            Value *safeRHS = Builder.CreateSelect(
                                isZero, one, rhs, "safe_rhs");

                            Value *safeDiv = nullptr;
                            if (op->getOpcode() == Instruction::SDiv) {
                                safeDiv = Builder.CreateSDiv(lhs, safeRHS,
                                                             "safe_div");
                            } else {
                                safeDiv = Builder.CreateUDiv(lhs, safeRHS,
                                                             "safe_div");
                            }

                            op->replaceAllUsesWith(safeDiv);
                            op->eraseFromParent();

                            llvm::errs()
                                << "Inserted runtime div-by-zero check in "
                                << F.getName() << "\n";
                        }
                    }
                }
            }
        }
        return PreservedAnalyses::all();
    };
};

}

extern "C" LLVM_ATTRIBUTE_WEAK ::llvm::PassPluginLibraryInfo
llvmGetPassPluginInfo() {
    return {
        .APIVersion = LLVMm_PLUGIN_API_VERSION,
        .PluginName = "LIC pass",
        .PluginVersion = "v0.1",
        .RegisterPassBuilderCallbacks = [](PassBuilder &PB) {
            PB.registerPipelineStartEPCallback(
                [](ModulePassManager &MPM, OptimizationLevel Level) {
                    MPM.addPass(LICMPass());
                });
        }
    };
}
