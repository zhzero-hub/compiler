; ModuleID = 'main.ll'
source_filename = "main.ll"

@0 = private unnamed_addr constant [12 x i8] c"value = %d\0A\00", align 1

declare i32 @printf(i8*, ...)

define i32 @Fibonacci(i32 %x) {
entry:
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = icmp eq i32 %x, 1
  br i1 %1, label %if_then, label %if_else

if_end:                                           ; preds = %if_else, %if_then
  %2 = icmp eq i32 %x, 2
  br i1 %2, label %if_then2, label %if_else3

if_then:                                          ; preds = %entry
  %3 = load i32, i32* %0, align 4
  store i32 1, i32* %0, align 4
  br label %if_end

if_else:                                          ; preds = %entry
  br label %if_end

if_end1:                                          ; preds = %if_else3, %if_then2
  %4 = icmp sgt i32 %x, 2
  br i1 %4, label %if_then5, label %if_else6

if_then2:                                         ; preds = %if_end
  %5 = load i32, i32* %0, align 4
  store i32 1, i32* %0, align 4
  br label %if_end1

if_else3:                                         ; preds = %if_end
  br label %if_end1

if_end4:                                          ; preds = %if_else6, %if_then5
  %6 = load i32, i32* %0, align 4
  ret i32 %6

if_then5:                                         ; preds = %if_end1
  %7 = load i32, i32* %0, align 4
  %8 = sub i32 %x, 2
  %9 = call i32 @Fibonacci(i32 %8)
  %10 = sub i32 %x, 1
  %11 = call i32 @Fibonacci(i32 %10)
  %12 = add i32 %9, %11
  store i32 %12, i32* %0, align 4
  br label %if_end4

if_else6:                                         ; preds = %if_end1
  br label %if_end4
}

define i32 @main() {
entry:
  %0 = alloca i32, align 4
  store i32 0, i32* %0, align 4
  %1 = alloca i32, align 4
  store i32 0, i32* %1, align 4
  %2 = alloca i32, align 4
  store i32 0, i32* %2, align 4
  %3 = load i32, i32* %0, align 4
  store i32 5, i32* %0, align 4
  %4 = load i32, i32* %1, align 4
  store i32 2, i32* %1, align 4
  %5 = load i32, i32* %2, align 4
  %6 = load i32, i32* %0, align 4
  %7 = load i32, i32* %1, align 4
  %8 = mul i32 %6, %7
  %9 = call i32 @Fibonacci(i32 %8)
  store i32 %9, i32* %2, align 4
  %10 = load i32, i32* %2, align 4
  %11 = call i32 (i8*, ...) @printf(i8* getelementptr inbounds ([12 x i8], [12 x i8]* @0, i32 0, i32 0), i32 %10)
  ret i32 0
}
