; ModuleID = 'test'
source_filename = "test"

define internal i32 @add(i32 %one, i32 %two) {
entry:
  %0 = sdiv i32 %one, %two
  ret i32 %0
}

define internal i32 @main() {
entry:
  ret i32 0
}
