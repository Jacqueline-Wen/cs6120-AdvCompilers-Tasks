; ModuleID = 'simpleTest2.ll'
source_filename = "simpleTest2.c"
target datalayout = "e-m:o-p270:32:32-p271:32:32-p272:64:64-i64:64-i128:128-n32:64-S128-Fn32"
target triple = "arm64-apple-macosx14.0.0"

@.str = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define void @test() #0 {
  %1 = alloca i32, align 4
  %2 = alloca i32, align 4
  %3 = alloca i32, align 4
  %4 = alloca i32, align 4
  %5 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  store i32 0, ptr %2, align 4
  %.promoted = load i32, ptr %2, align 4
  %.promoted1 = load i32, ptr %3, align 4
  %.promoted3 = load i32, ptr %4, align 4
  %.promoted5 = load i32, ptr %5, align 4
  %.promoted7 = load i32, ptr %1, align 4
  br label %6

6:                                                ; preds = %17, %0
  %7 = phi i32 [ %16, %17 ], [ %.promoted7, %0 ]
  %8 = phi i32 [ %15, %17 ], [ %.promoted5, %0 ]
  %9 = phi i32 [ %14, %17 ], [ %.promoted3, %0 ]
  %10 = phi i32 [ 5, %17 ], [ %.promoted1, %0 ]
  %11 = phi i32 [ %18, %17 ], [ %.promoted, %0 ]
  %12 = icmp slt i32 %11, 5
  br i1 %12, label %13, label %19

13:                                               ; preds = %6
  %14 = mul nsw i32 5, 4
  %15 = sub nsw i32 %14, 7
  %16 = add nsw i32 %7, %15
  br label %17

17:                                               ; preds = %13
  %18 = add nsw i32 %11, 1
  br label %6, !llvm.loop !6

19:                                               ; preds = %6
  %.lcssa8 = phi i32 [ %7, %6 ]
  %.lcssa6 = phi i32 [ %8, %6 ]
  %.lcssa4 = phi i32 [ %9, %6 ]
  %.lcssa2 = phi i32 [ %10, %6 ]
  %.lcssa = phi i32 [ %11, %6 ]
  store i32 %.lcssa, ptr %2, align 4
  store i32 %.lcssa2, ptr %3, align 4
  store i32 %.lcssa4, ptr %4, align 4
  store i32 %.lcssa6, ptr %5, align 4
  store i32 %.lcssa8, ptr %1, align 4
  %20 = load i32, ptr %1, align 4
  %21 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %20)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind ssp uwtable(sync)
define i32 @main() #0 {
  %1 = alloca i32, align 4
  store i32 0, ptr %1, align 4
  call void @test()
  ret i32 0
}

attributes #0 = { noinline nounwind ssp uwtable(sync) "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }
attributes #1 = { "frame-pointer"="non-leaf" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="apple-m1" "target-features"="+aes,+altnzcv,+ccdp,+ccidx,+ccpp,+complxnum,+crc,+dit,+dotprod,+flagm,+fp-armv8,+fp16fml,+fptoint,+fullfp16,+jsconv,+lse,+neon,+pauth,+perfmon,+predres,+ras,+rcpc,+rdm,+sb,+sha2,+sha3,+specrestrict,+ssbs,+v8.1a,+v8.2a,+v8.3a,+v8.4a,+v8a" }

!llvm.module.flags = !{!0, !1, !2, !3, !4}
!llvm.ident = !{!5}

!0 = !{i32 2, !"SDK Version", [2 x i32] [i32 14, i32 2]}
!1 = !{i32 1, !"wchar_size", i32 4}
!2 = !{i32 8, !"PIC Level", i32 2}
!3 = !{i32 7, !"uwtable", i32 1}
!4 = !{i32 7, !"frame-pointer", i32 1}
!5 = !{!"Homebrew clang version 21.1.2"}
!6 = distinct !{!6, !7}
!7 = !{!"llvm.loop.mustprogress"}
